#include "sixel.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define mas(m, x_size, y_size, x, y) m[(x)*(y_size) + (y)]

#define max(a,b) \
({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

bool sixel_color(int *color_cnt, sixel_color_t *color_map) { return true; }

bool sixel_init(sixel_t **sixel, unsigned int max_size_x, unsigned int max_size_y) {
    if (sixel == NULL) {
        return false;
    }
    if (*sixel != NULL) {
        return false;
    }
    if (max_size_x > SIXEL_MAX_SIZE_X) {
        return false;
    }
    if (max_size_y > SIXEL_MAX_SIZE_Y) {
        return false;
    }

    (*sixel) = (sixel_t *)calloc(1, sizeof(sixel_t));

    (*sixel)->out_buff_len = max_size_x * max_size_y * 2 + 1;
    (*sixel)->out_buff_shift = 0;
    (*sixel)->out_buff_shift_max = 0;
    (*sixel)->out_buff = (char *)calloc((*sixel)->out_buff_len, sizeof(char));

    (*sixel)->tmp_buff_len = max_size_x + 1;
    (*sixel)->tmp_buff_shift = 0;
    (*sixel)->tmp_buff_shift_max = 0;
    (*sixel)->tmp_buff = (char *)calloc((*sixel)->tmp_buff_len, sizeof(char));
    return true;
}

bool sixel_init_colormap(sixel_t *sixel, sixel_color_model_e color_mod, unsigned int color_cnt, sixel_color_t color_map[color_cnt]) {
    if (sixel == NULL) {
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
    free((*sixel)->tmp_buff);
    free((*sixel));
    *sixel = NULL;
    return true;
}

#define BIT_TO_SUMBOL(a1, a2, a3, a4, a5, a6) (char)(((a1) * 32 + (a2) * 16 + (a3) * 8 + (a4) * 4 + (a5) * 2 + (a6) * 1) + '?')

bool sixel_draw(sixel_t *sixel, int size_x, int size_y, sixel_color_t image[size_x * size_y]) {
    if (sixel == NULL) {
        return false;
    }

    bool subline_bitmap[size_x][6];
    sixel_color_t subline_color[size_x][6];

    int out_shift = sixel->out_buff_shift = 0;
    int out_shift_max = sixel->out_buff_shift_max;
    int out_len = sixel->out_buff_len;
    char *out_buff = sixel->out_buff;

    int tmp_shift = sixel->tmp_buff_shift = 0;
    int tmp_shift_max = sixel->tmp_buff_shift_max;
    int tmp_len = sixel->tmp_buff_len;
    char *tmp_buff = sixel->tmp_buff;

    out_shift += sprintf(&out_buff[out_shift], "%s", "\e");        // Запуск последовательности Sixel
    out_shift += sprintf(&out_buff[out_shift], "%s", "P");         // Начало глобальных параметров
    out_shift += sprintf(&out_buff[out_shift], "%s", "7;");        // Параметры пикселя
    out_shift += sprintf(&out_buff[out_shift], "%s", "0;");        // Цвет фона
    out_shift += sprintf(&out_buff[out_shift], "%s", "0;");        // Размер сетки
    out_shift += sprintf(&out_buff[out_shift], "%s", "q\"");       // Начало команд sixel
    out_shift += sprintf(&out_buff[out_shift], "%s", "1;1;10;10"); // Какие-то параметры..

    // Установка цветовой палитры
    for (int i = 0; i < sixel->color_cnt; i++) {
        out_shift += sprintf(&out_buff[out_shift],
                             "#%" PRIu8 ";%" PRIu8 ";%" PRIu8 ";%" PRIu8 ";%" PRIu8 "",
                             i,
                             sixel->color_mod,
                             sixel->color_map[i].model.PX * 255 + sixel->color_map[i].model.Px,
                             sixel->color_map[i].model.Py,
                             sixel->color_map[i].model.Pz);
    }

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
                            subline_bitmap[i][jj] = 1;
                        } else {
                            subline_bitmap[i][jj] = 0;
                        }
                    } else {
                        subline_bitmap[i][jj] = 0;
                    }
                }
            }

            out_shift += sprintf(&out_buff[out_shift], "#%u", color);

            tmp_shift_max = max(tmp_shift_max, tmp_shift);
            tmp_shift = 0;

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
                        out_shift += sprintf(&out_buff[out_shift], "!%u%c", cnt, first);
                    } else {
                        out_shift += sprintf(&out_buff[out_shift], "%c", first);
                    }

                    cnt = 0;
                }
            }
            out_shift += sprintf(&out_buff[out_shift], "%s", "$");
        }
        out_shift += sprintf(&out_buff[out_shift], "%s", "-");
    }

    out_shift += sprintf(&out_buff[out_shift], "\e\\");


    out_shift_max = max(out_shift_max, out_shift);


    sixel->out_buff = out_buff;
    sixel->out_buff_shift = out_shift;
    sixel->out_buff_shift_max = out_shift_max;

    sixel->tmp_buff = tmp_buff;
    sixel->tmp_buff_shift = tmp_shift;
    sixel->tmp_buff_shift_max = tmp_shift_max;

    printf("%s", out_buff);
    fflush(stdout);

    return true;
}
