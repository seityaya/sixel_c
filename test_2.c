#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#include "image.h"
#include "sixel.h"

#define IMG_SIZE_X  (250)
#define IMG_SIZE_Y  (250)

static sixel_t *sixel = NULL;
static sixel_color_t image[IMG_SIZE_X * IMG_SIZE_Y] = {0};

static sixel_color_t *color_map = 0;
static int color_count = 0;

int test_2_free(void){
    if (!sixel_free(&sixel)) {
        return -1;
    }
    return 0;
}

int test_2_init(void){
    build_image_mod(IMG_SIZE_X, IMG_SIZE_Y, image);

    if (!sixel_color(&color_count, color_map)) {
        goto end;
    }

    if (!sixel_init(&sixel, IMG_SIZE_X, IMG_SIZE_Y)) {
        goto end;
    }

    if (!sixel_cmap_init(sixel, SIXEL_COLOR_MODEL_RGB, color_count, color_map)) {
        goto end;
    }

    if (!sixel_draw_init(sixel)){
        goto end;
    }
    return 0;
end:
    test_2_free();
    return -1;
}

int test_2_loop(void) {
    if (!sixel_draw(sixel, IMG_SIZE_X, IMG_SIZE_Y, image)) {
        goto end;
    }

    return 0;
end:
    test_2_free();
    return -1;
}
