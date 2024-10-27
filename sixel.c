#include "sixel.h"

#include <stdio.h>
#include <stdlib.h>

bool sixel_init(sixel_t **sixel, uint_fast32_t max_size_x, uint_fast32_t max_size_y) {
    if (sixel == NULL) {
        return false;
    }
    if (*sixel != NULL) {
        return false;
    }
    if (max_size_x > SIXEL_SIZE_MAX_IMAGE_X) {
        return false;
    }
    if (max_size_y > SIXEL_SIZE_MAX_IMAGE_Y) {
        return false;
    }

    (*sixel) = (sixel_t *)calloc(1, sizeof(sixel_t));

    (*sixel)->out_buff_shift = 0;
    (*sixel)->out_buff_shift_init = 0;
    (*sixel)->out_buff_len = (max_size_x * max_size_y) * 20 + 1; // TODO: Придумать эвристику лучше

    (*sixel)->out_buff = (char *)calloc((*sixel)->out_buff_len, sizeof(char));

    (*sixel)->max_size_x = max_size_x;
    (*sixel)->max_size_y = max_size_y;

    (*sixel)->color_mod = 0;
    (*sixel)->color_cnt = 0;
    (*sixel)->color_map = NULL;

    return true;
}

bool sixel_cmap_init(sixel_t *sixel, sixel_color_model_e color_mod, uint_fast8_t color_cnt, sixel_color_model_t *color_map) {
    if (sixel == NULL) {
        return false;
    }
    if (color_cnt > (SIXEL_COLOR_MAX_COUNT - 1)) {
        return false;
    }

    sixel->color_mod = color_mod;
    sixel->color_cnt = color_cnt;
    sixel->color_map = color_map;

    return true;
}

bool sixel_free(sixel_t **sixel) {
    if (sixel == NULL) {
        return false;
    }
    if (*sixel == NULL) {
        return false;
    }

    free((*sixel)->out_buff);
    free((*sixel));
    *sixel = NULL;

    return true;
}

static uint_fast32_t uint_to_str(char *buff, uint_fast32_t uint_value) {
    uint_fast32_t temp_value = uint_value;
    uint_fast32_t n = 0;
    do {
        temp_value /= 10;
        n++;
    } while (temp_value != 0);

    buff += n + 1;
    *--buff = 0;
    do {
        *--buff = uint_value % 10 + '0';
        uint_value /= 10;
    } while (uint_value != 0);
    return n;
}

static uint_fast32_t char_to_str(char *buff, char char_value) {
    buff[0] = char_value;
    buff[1] = '\0';
    return 1;
}

bool sixel_frame_init(sixel_t *sixel) {
    if (sixel == NULL) {
        return false;
    }

    uint_fast32_t out_shift = 0;
    char *out_buff = sixel->out_buff;

    // Запуск последовательности Sixel
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_ESCAPE);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_BEG_PARAM_CHARACTERS);
    // Параметры пикселя
    out_shift += uint_to_str(&out_buff[out_shift], SIXEL_PIXEL_ASPERO_RATIO_DEF);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Цвет фона
    out_shift += uint_to_str(&out_buff[out_shift], SIXEL_PIXEL_BACKGRAUND_COLOR_DEF);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Размер сетки
    out_shift += uint_to_str(&out_buff[out_shift], SIXEL_HORIZONTAL_GRID_SIZE_DEF);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Начало команд sixel
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_BEG_COMAND_CHARACTERS);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_RASTER_ATTRIBUTES);
    // Pan
    out_shift += uint_to_str(&out_buff[out_shift], SIXEL_RASTER_ATTRIBUTES_PAN_DEF);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Pad
    out_shift += uint_to_str(&out_buff[out_shift], SIXEL_RASTER_ATTRIBUTES_PAD_DEF);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Ph max size
    out_shift += uint_to_str(&out_buff[out_shift], sixel->max_size_x);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Pv max size
    out_shift += uint_to_str(&out_buff[out_shift], sixel->max_size_y);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);

    // Установка цветовой палитры
    for (uint_fast8_t i = 0; i < sixel->color_cnt; i++) {
        out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_COLOR_INTRODUCER);
        out_shift += uint_to_str(&out_buff[out_shift], i);
        out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
        out_shift += uint_to_str(&out_buff[out_shift], sixel->color_mod);
        out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
        out_shift += uint_to_str(&out_buff[out_shift], sixel->color_map[i].PX * 255 + sixel->color_map[i].Px);
        out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
        out_shift += uint_to_str(&out_buff[out_shift], sixel->color_map[i].Py);
        out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
        out_shift += uint_to_str(&out_buff[out_shift], sixel->color_map[i].Pz);
    }

    sixel->out_buff_shift_init = out_shift;

    return true;
}

bool sixel_frame_draw(sixel_t *sixel, sixel_image_t *image) {
    if (sixel == NULL) {
        return false;
    }
    if (sixel->max_size_x < image->size_x) {
        return false;
    }
    if (sixel->max_size_y < image->size_y) {
        return false;
    }

    bool sub_buff[image->size_x][SIXEL_PIXEL_IN_ONE_LINE];

    uint_fast32_t tmp_shift = 0;
    char tmp_buff[image->size_x + 1];

    uint_fast32_t out_shift = sixel->out_buff_shift_init;
    char *out_buff = sixel->out_buff;

    for (uint_fast32_t j = 0; j < image->size_y; j += SIXEL_PIXEL_IN_ONE_LINE) {
        for (uint_fast8_t color = 0; color < sixel->color_cnt; color++) {

            bool color_flag = false;
            for (uint_fast32_t jj = 0; jj < SIXEL_PIXEL_IN_ONE_LINE; jj++) {
                for (uint_fast32_t i = 0; i < image->size_x; i++) {
                    if (j + jj < image->size_y) {
                        if (SIXEL_MAS(image->image, image->size_x, image->size_y, i, j + jj) == color) {
                            sub_buff[i][jj] = true;
                            color_flag = true;
                        } else {
                            sub_buff[i][jj] = false;
                        }
                    } else {
                        sub_buff[i][jj] = false;
                    }
                }
            }

            if (color_flag) {
                out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_COLOR_INTRODUCER);
                out_shift += uint_to_str(&out_buff[out_shift], color);

                tmp_shift = 0;
                for (uint_fast32_t i = 0; i < image->size_x; i++) {
                    tmp_shift += char_to_str(&tmp_buff[tmp_shift],
                                             sub_buff[i][0] * 1 + sub_buff[i][1] * 2 + sub_buff[i][2] * 4 + sub_buff[i][3] * 8 + sub_buff[i][4] * 16 +
                                                 sub_buff[i][5] * 32 + SIXEL_CONTROL_FUNCTIONS_BEG_DATA_CHARACTERS);
                }

                {
                    uint_fast32_t i = 0;
                    uint_fast32_t j = i;
                    for (i = 0; i < tmp_shift; i = j) {
                        uint_fast32_t cnt = 0;
                        for (j = i; j < tmp_shift; j++) {
                            if (tmp_buff[i] == tmp_buff[j]) {
                                cnt++;
                                continue;
                            }
                            break;
                        }

                        if (cnt > 1) {
                            out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_REPET);
                            out_shift += uint_to_str(&out_buff[out_shift], cnt);
                            out_shift += char_to_str(&out_buff[out_shift], tmp_buff[i]);
                        } else {
                            out_shift += char_to_str(&out_buff[out_shift], tmp_buff[i]);
                        }
                    }
                }
            }
            out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_CARRIAGE_RETURN);
        }
        out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_NEW_LINE);
    }

    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_ESCAPE);
    out_shift += char_to_str(&out_buff[out_shift], SIXEL_CONTROL_FUNCTIONS_STRING_TERMINATOR);

    sixel->out_buff_shift = out_shift;

    printf("%s", out_buff);
    fflush(stdout);

    return true;
}

bool sixel_build_color_map_palete(sixel_color_palete_e model, uint_fast8_t *out_color_count, sixel_color_model_t *out_color_map) {
    switch (model) {
    case SIXEL_COLOR_PALETE_GREYSCALE: {
        *out_color_count = SIXEL_COLOR_MAX_COUNT - 1;
        for (uint_fast32_t k = 0; k < SIXEL_COLOR_MAX_COUNT; k++) {
            uint_fast32_t c = (k * 256) / 100;
            out_color_map[k] = SIXEL_RGB(c, c, c);
        }
        break;
    }
    case SIXEL_COLOR_PALETE_RGB216: {
        uint_fast32_t n = 0;
        for (uint_fast32_t k = 0; k <= SIXEL_COLOR_MODEL_RGB_MAX_B; k += 20) {
            for (uint_fast32_t j = 0; j <= SIXEL_COLOR_MODEL_RGB_MAX_G; j += 20) {
                for (uint_fast32_t i = 0; i <= SIXEL_COLOR_MODEL_RGB_MAX_R; i += 20) {
                    out_color_map[n++] = SIXEL_RGB(i, j, k);
                }
            }
        }
        *out_color_count = n;
        break;
    }
    default:
        break;
    }
    return true;
}

static sixel_color_t rgb_to_greyscale(color_t color) {
    float Y = (0.299 * color.R) + (0.587 * color.G) + (0.114 * color.B);
    float U = (color.B - Y) * 0.565;
    float V = (color.R - Y) * 0.713;

    float UV = (U + V);

    float R1 = color.R * 0.299;
    float R2 = color.R * 0.587;
    float R3 = color.R * 0.114;

    float G1 = color.G * 0.299;
    float G2 = color.G * 0.587;
    float G3 = color.G * 0.114;

    float B1 = color.B * 0.299;
    float B2 = color.B * 0.587;
    float B3 = color.B * 0.114;

    float R4 = (R1 + R2 + R3) / 3;
    float G4 = (G1 + G2 + G3) / 3;
    float B4 = (B1 + B2 + B3) / 3;

    float G = (R4 + G4 + B4 + UV) / 4;

    return (sixel_color_t)(G);
}

static sixel_color_t rgb_to_color216(color_t color) {
    uint8_t R = (uint8_t)(((((uint_fast32_t)(color.R) * 100) / 256) + 10) / 20);
    uint8_t G = (uint8_t)(((((uint_fast32_t)(color.G) * 100) / 256) + 10) / 20);
    uint8_t B = (uint8_t)(((((uint_fast32_t)(color.B) * 100) / 256) + 10) / 20);

    return (sixel_color_t)((R) + (6 * G) + (6 * 6 * B));
}

bool sixel_frame_conv(sixel_color_palete_e model, image_t *in_image, sixel_image_t *out_image) {
    if (out_image->size_x < in_image->size_x) {
        return false;
    }
    if (out_image->size_x < in_image->size_x) {
        return false;
    }

    switch (model) {
    case SIXEL_COLOR_PALETE_GREYSCALE: {
        for (uint_fast32_t y = 0; y < in_image->size_y; y++) {
            for (uint_fast32_t x = 0; x < in_image->size_x; x++) {
                SIXEL_MAS(out_image->image, out_image->size_x, out_image->size_y, x, y) =
                    rgb_to_greyscale(SIXEL_MAS(in_image->image, in_image->size_x, in_image->size_y, x, y));
            }
        }
        break;
    }
    case SIXEL_COLOR_PALETE_RGB216: {
        for (uint_fast32_t y = 0; y < in_image->size_y; y++) {
            for (uint_fast32_t x = 0; x < in_image->size_x; x++) {
                SIXEL_MAS(out_image->image, out_image->size_x, out_image->size_y, x, y) =
                    rgb_to_color216(SIXEL_MAS(in_image->image, in_image->size_x, in_image->size_y, x, y));
            }
        }
        break;
    }
    default:
        break;
    }
    return true;
}
