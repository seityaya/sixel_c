#include <inttypes.h>
#include <stdio.h>

#include "sixel.h"
#include "image.h"

#define mas(m, x_size, y_size, x, y) m[(x)*(y_size) + (y)]

void image_clear(int x, int y, sixel_color_t *image) {
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            mas(image, x, y, i, j).color = 0;
        }
    }
}

void image_print(int x, int y, sixel_color_t *image) {
    for (int j = 0; j < y; j++) {
        for (int i = 0; i < x; i++) {
            printf("%10"PRIu32" ", mas(image, x, y, i, j).color);
        }
        printf("\n");
    }
    printf("\n");
}

void build_image_map(int x, int y, sixel_color_t *image, int color_cnt, sixel_color_t colormap[color_cnt]) {
    static int k = 0;
    k++;
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            mas(image, x, y, i, j).color = colormap[(i + j + k) % color_cnt].color;
            // mas(image, x, y, i, j) = colormap[i % color_cnt];
            // mas(image, x, y, i, j) = colormap[j % color_cnt];
            // mas(image, x, y, i, j) = colormap[(i + j + k) % color_cnt];
        }
    }
}

void build_image_mod(int x, int y, sixel_color_t *image){

}

void build_image_cat(int x, int y, sixel_color_t *image){

}
