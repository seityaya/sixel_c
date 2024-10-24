#include <inttypes.h>
#include <stdio.h>

#include "image.h"
#include "sixel.h"

void image_clear(sixel_image_t *image) {
    for (uint32_t i = 0; i < image->size_x; i++) {
        for (uint32_t j = 0; j < image->size_y; j++) {
            SIXEL_MAS(image->image, image->size_x, image->size_y, i, j) = 0;
        }
    }
}

void image_print(sixel_image_t *image) {
    for (uint32_t i = 0; i < image->size_x; i++) {
        for (uint32_t j = 0; j < image->size_y; j++) {
            printf("%3" PRIu8 " ", SIXEL_MAS(image->image, image->size_x, image->size_y, i, j));
        }
        printf("\n");
    }
    printf("\n");
}

void build_image_map(sixel_image_t *image, uint32_t color_count, uint32_t type) {
    static uint32_t k = 0;
    k+=1;

    uint32_t count_x = image->size_x / color_count;
    uint32_t count_y = image->size_y / color_count;

    if(count_x == 0){ count_x = 1; }
    if(count_y == 0){ count_y = 1; }

    uint32_t color_x = 0;
    for (uint32_t i = 0; i < image->size_x; i++) {

        if (i % count_x == 0 && i != 0) {
            color_x += 1;
            color_x %= color_count;
        }

        uint32_t color_y = 0;
        for (uint32_t j = 0; j < image->size_y; j++) {

            if (j % count_y == 0 && j != 0) {
                color_y += 1;
                color_y %= color_count;
            }

            switch (type) {
            case 0: {
                SIXEL_MAS(image->image, image->size_x, image->size_y, i, j) = (sixel_color_t)(j);
                break;
            }

            case 1: {
                SIXEL_MAS(image->image, image->size_x, image->size_y, i, j) = (sixel_color_t)((i + j + k) % color_count);
                break;
            }
            case 2: {
                SIXEL_MAS(image->image, image->size_x, image->size_y, i, j) = (sixel_color_t)((i - j + k) % color_count);
                break;
            }

            case 3: {
                SIXEL_MAS(image->image, image->size_x, image->size_y, i, j) = (sixel_color_t)((i + k) % color_count);
                break;
            }
            case 4: {
                SIXEL_MAS(image->image, image->size_x, image->size_y, i, j) = (sixel_color_t)((j + k) % color_count);
                break;
            }

            case 5: {
                SIXEL_MAS(image->image, image->size_x, image->size_y, i, j) = (sixel_color_t)(color_x);
                break;
            }
            case 6: {
                SIXEL_MAS(image->image, image->size_x, image->size_y, i, j) = (sixel_color_t)(color_y);
                break;
            }
            default:
                break;
            }
        }
    }
}
