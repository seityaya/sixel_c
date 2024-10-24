#include "sixel.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool sixel_init(sixel_t **sixel, uint32_t max_size_x, uint32_t max_size_y) {
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

    (*sixel)->out_buff_len = (max_size_x * max_size_y) * 20 + 1;  // TODO: Придумать эвристику лучше
    (*sixel)->out_buff = (char *)calloc((*sixel)->out_buff_len, sizeof(char));

    (*sixel)->max_size_x = max_size_x;
    (*sixel)->max_size_y = max_size_y;

    return true;
}

bool sixel_cmap_init(sixel_t *sixel, sixel_color_model_e color_mod, uint32_t color_cnt, sixel_color_model_t *color_map) {
    if (sixel == NULL) {
        return false;
    }
    if (color_cnt > SIXEL_COLOR_MAX_COUNT) {
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

#define BIT_TO_SUMBOL(a1, a2, a3, a4, a5, a6) (char)(((a1) * 32 + (a2) * 16 + (a3) * 8 + (a4) * 4 + (a5) * 2 + (a6) * 1) + SIXEL_CONTROL_FUNCTIONS_BEG_DATA_CHARACTERS)

bool sixel_draw_init(sixel_t *sixel) {
    if (sixel == NULL) {
        return false;
    }

    int out_shift = 0;
    char *out_buff = sixel->out_buff;

    // Запуск последовательности Sixel
    out_shift += sprintf(&out_buff[out_shift], "%c%c", SIXEL_CONTROL_FUNCTIONS_ESCAPE, SIXEL_CONTROL_FUNCTIONS_BEG_PARAM_CHARACTERS);
    // Параметры пикселя
    out_shift += sprintf(&out_buff[out_shift], "%d%c", SIXEL_PIXEL_ASPERO_RATIO_DEF, SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Цвет фона
    out_shift += sprintf(&out_buff[out_shift], "%d%c", SIXEL_PIXEL_BACKGRAUND_COLOR_DEF, SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Размер сетки
    out_shift += sprintf(&out_buff[out_shift], "%d%c", SIXEL_HORIZONTAL_GRID_SIZE_DEF, SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Начало команд sixel
    out_shift += sprintf(&out_buff[out_shift], "%c%c", SIXEL_CONTROL_FUNCTIONS_BEG_COMAND_CHARACTERS, SIXEL_CONTROL_FUNCTIONS_RASTER_ATTRIBUTES);
    // Pan
    out_shift += sprintf(&out_buff[out_shift], "%d%c", SIXEL_RASTER_ATTRIBUTES_PAN_DEF, SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Pad
    out_shift += sprintf(&out_buff[out_shift], "%d%c", SIXEL_RASTER_ATTRIBUTES_PAD_DEF, SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Ph max size
    out_shift += sprintf(&out_buff[out_shift], "%d%c", sixel->max_size_x, SIXEL_CONTROL_FUNCTIONS_SEPARATOR);
    // Pv max size
    out_shift += sprintf(&out_buff[out_shift], "%d%c", sixel->max_size_y, SIXEL_CONTROL_FUNCTIONS_SEPARATOR);

    // Установка цветовой палитры
    for (uint32_t i = 0; i < sixel->color_cnt; i++) {
        out_shift += sprintf(&out_buff[out_shift],
                             "%c%" PRIu8 "%c%" PRIu8 "%c%" PRIu8 "%c%" PRIu8 "%c%" PRIu8 "",
                             SIXEL_CONTROL_FUNCTIONS_COLOR_INTRODUCER,
                             i,
                             SIXEL_CONTROL_FUNCTIONS_SEPARATOR,
                             sixel->color_mod,
                             SIXEL_CONTROL_FUNCTIONS_SEPARATOR,
                             sixel->color_map[i].PX * 255 + sixel->color_map[i].Px,
                             SIXEL_CONTROL_FUNCTIONS_SEPARATOR,
                             sixel->color_map[i].Py,
                             SIXEL_CONTROL_FUNCTIONS_SEPARATOR,
                             sixel->color_map[i].Pz);
    }

    sixel->out_buff_shift_init = (uint32_t)(out_shift);

    return true;
}

bool sixel_draw(sixel_t *sixel, sixel_image_t *image) {
    if (sixel == NULL) {
        return false;
    }
    if (sixel->max_size_x < image->size_x) {
        return false;
    }
    if (sixel->max_size_y < image->size_y) {
        return false;
    }

    bool subline_bitmap[image->size_x][6];

    int out_shift = (int)(sixel->out_buff_shift_init);
    char *out_buff = sixel->out_buff;

    for (uint32_t j = 0; j < image->size_y; j += 6) {

        for (uint32_t color = 0; color < sixel->color_cnt; color++) {

            for (uint32_t i = 0; i < image->size_x; i++) {

                for (uint32_t jj = 0; jj < 6; jj++) {

                    if (j + jj < image->size_y) {
                        if (SIXEL_MAS(image->image, image->size_x, image->size_y, i, j + jj) == color) {
                            subline_bitmap[i][jj] = true;
                        } else {
                            subline_bitmap[i][jj] = false;
                        }
                    } else {
                        subline_bitmap[i][jj] = false;
                    }

                }
            }

            out_shift += sprintf(&out_buff[out_shift], "%c%" PRIu32 "", SIXEL_CONTROL_FUNCTIONS_COLOR_INTRODUCER, color);

            char tmp_buff[image->size_x + 1];
            int tmp_shift = 0;

            for (uint32_t i = 0; i < image->size_x; i++) {
                tmp_shift += sprintf(&tmp_buff[tmp_shift],
                                     "%c",
                                     BIT_TO_SUMBOL(subline_bitmap[i][5],
                                                   subline_bitmap[i][4],
                                                   subline_bitmap[i][3],
                                                   subline_bitmap[i][2],
                                                   subline_bitmap[i][1],
                                                   subline_bitmap[i][0]));
            }

            {
                uint32_t cnt = 0;
                char first = 0;
                char current = 0;

                int i = 0;
                int j = i;
                for (i = 0; i < tmp_shift; i = j) {
                    first = tmp_buff[i];
                    for (j = i; j < tmp_shift; j++) {
                        current = tmp_buff[j];
                        if (current == first) {
                            cnt++;
                            continue;
                        }
                        break;
                    }

                    if (cnt > 1) {
                        out_shift += sprintf(&out_buff[out_shift], "%c%u%c", SIXEL_CONTROL_FUNCTIONS_REPET, cnt, first);
                    } else {
                        out_shift += sprintf(&out_buff[out_shift], "%c", first);
                    }

                    cnt = 0;
                }
            }
            out_shift += sprintf(&out_buff[out_shift], "%c", SIXEL_CONTROL_FUNCTIONS_CARRIAGE_RETURN);
        }
        out_shift += sprintf(&out_buff[out_shift], "%c", SIXEL_CONTROL_FUNCTIONS_NEW_LINE);
    }

    out_shift += sprintf(&out_buff[out_shift], "%c%c", SIXEL_CONTROL_FUNCTIONS_ESCAPE, SIXEL_CONTROL_FUNCTIONS_STRING_TERMINATOR);

    sixel->out_buff_shift = (uint32_t)(out_shift);

    printf("%s", out_buff);
    fflush(stdout);

    return true;
}

bool sixel_image_color_map_palete_build(sixel_color_palete_e model, uint32_t *out_color_count, sixel_color_model_t *color_map) {
    switch (model) {
    case SIXEL_COLOR_PALETE_GREYSCALE: {
        *out_color_count = SIXEL_COLOR_MAX_COUNT;
        for (int k = 0; k < SIXEL_COLOR_MAX_COUNT; k++) {
            int c = (k / 2.56);
            color_map[k] = SIXEL_RGB(c, c, c);
        }
        break;
    }
    case SIXEL_COLOR_PALETE_COLOR216: {
        int n = 0;
        for (int k = 0; k <= SIXEL_COLOR_MODEL_RGB_MAX_B; k += 20) {
            for (int j = 0; j <= SIXEL_COLOR_MODEL_RGB_MAX_G; j += 20) {
                for (int i = 0; i <= SIXEL_COLOR_MODEL_RGB_MAX_R; i += 20) {
                    color_map[n++] = SIXEL_RGB(i, j, k);
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

    uint8_t R = (uint8_t)((float)(color.R) / (float)(2.56) / (float)(20));
    uint8_t G = (uint8_t)((float)(color.G) / (float)(2.56) / (float)(20));
    uint8_t B = (uint8_t)((float)(color.B) / (float)(2.56) / (float)(20));

    uint8_t C = (sixel_color_t)((R) + (6 * G) + (6 * 6 * B));

    return (sixel_color_t)(C);
}

bool sixel_image_color_img_build(sixel_color_palete_e model, image_t *in_image, sixel_image_t *out_image) {
    if (out_image->size_x < in_image->size_x) {
        return false;
    }
    if (out_image->size_x < in_image->size_x) {
        return false;
    }

    switch (model) {
    case SIXEL_COLOR_PALETE_GREYSCALE: {
        for (int x = 0; x < in_image->size_x; x++) {
            for (int y = 0; y < in_image->size_y; y++) {
                SIXEL_MAS(out_image->image, out_image->size_x, out_image->size_y, x, y) =
                    rgb_to_greyscale(SIXEL_MAS(in_image->image, in_image->size_x, in_image->size_y, x, y));
            }
        }
        break;
    }
    case SIXEL_COLOR_PALETE_COLOR216: {
        for (int x = 0; x < in_image->size_x; x++) {
            for (int y = 0; y < in_image->size_y; y++) {
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
