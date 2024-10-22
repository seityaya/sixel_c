#include "sixel.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define mas(m, x_size, y_size, x, y) m[(x)*(y_size) + (y)]

bool sixel_color(int *color_cnt, sixel_color_t *color_map) {
    return true;
}

bool sixel_init(sixel_t **sixel, unsigned int max_size_x, unsigned int max_size_y) {
    if (sixel == NULL) {
        return false;
    }
    if (*sixel != NULL) {
        return false;
    }
    if (max_size_x > SIXEL_MAX_SIZE_IMAGE_X) {
        return false;
    }
    if (max_size_y > SIXEL_MAX_SIZE_IMAGE_Y) {
        return false;
    }

    (*sixel) = (sixel_t *)calloc(1, sizeof(sixel_t));

    (*sixel)->out_buff_len = max_size_x * max_size_y * 2 + 1;
    (*sixel)->out_buff = (char *)calloc((*sixel)->out_buff_len, sizeof(char));

    (*sixel)->max_size_x = max_size_x;
    (*sixel)->max_size_y = max_size_y;

    return true;
}

bool sixel_cmap_init(sixel_t *sixel, sixel_color_model_e color_mod, unsigned int color_cnt, sixel_color_t color_map[color_cnt]) {
    if (sixel == NULL) {
        return false;
    }
    if (color_cnt > SIXEL_MAX_COLOR_COUNT){
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
    for (int i = 0; i < sixel->color_cnt; i++) {
        out_shift += sprintf(&out_buff[out_shift],
                             "%c%" PRIu8 "%c%" PRIu8 "%c%" PRIu8 "%c%" PRIu8 "%c%" PRIu8 "",
                             SIXEL_CONTROL_FUNCTIONS_COLOR_INTRODUCER,
                             i,
                             SIXEL_CONTROL_FUNCTIONS_SEPARATOR,
                             sixel->color_mod,
                             SIXEL_CONTROL_FUNCTIONS_SEPARATOR,
                             sixel->color_map[i].model.PX * 255 + sixel->color_map[i].model.Px,
                             SIXEL_CONTROL_FUNCTIONS_SEPARATOR,
                             sixel->color_map[i].model.Py,
                             SIXEL_CONTROL_FUNCTIONS_SEPARATOR,
                             sixel->color_map[i].model.Pz);
    }

    sixel->out_buff_shift_init = out_shift;

    return true;
}

bool sixel_draw(sixel_t *sixel, int size_x, int size_y, sixel_color_t image[size_x * size_y]) {
    if (sixel == NULL) {
        return false;
    }
    if (sixel->max_size_x < size_x) {
        return false;
    }
    if (sixel->max_size_y < size_y) {
        return false;
    }

    bool          subline_bitmap[size_x][6];
    sixel_color_t subline_color [size_x][6];

    int out_shift = sixel->out_buff_shift_init;
    char *out_buff = sixel->out_buff;

    // Создание изображения
    for (uint32_t j = 0; j < size_y; j += 6) {

        for (uint32_t i = 0; i < size_x; i++) {
            for (uint32_t jj = 0; jj < 6; jj++) {
                if (j + jj < size_y) {
                    subline_color[i][jj].color = mas(image, size_x, size_y, i, j + jj).color;
                } else {
                    subline_color[i][jj].color = 0;
                }
            }
        }

        for (uint32_t color = 0; color < sixel->color_cnt; color++) {

            for (uint32_t i = 0; i < size_x; i++) {
                for (uint32_t jj = 0; jj < 6; jj++) {
                    if (j + jj < size_y) {
                        if (subline_color[i][jj].color == sixel->color_map[color].color) {
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

            char tmp_buff[size_x + 1];
            int tmp_shift = 0;

            for (uint32_t i = 0; i < size_x; i++) {
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
                int cnt = 0;
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

    sixel->out_buff_shift = out_shift;

    printf("%s", out_buff);
    fflush(stdout);

    return true;
}
