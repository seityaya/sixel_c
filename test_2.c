#include "image.h"
#include "sixel.h"


#define IMG_SIZE_X  (864)
#define IMG_SIZE_Y  (80)


static uint_fast8_t color_count = 0;
static sixel_color_model_t color_map[SIXEL_COLOR_MAX_COUNT] = {0};


static sixel_color_t image_buff[IMG_SIZE_X * IMG_SIZE_Y] = {0};
static sixel_image_t image = {
    .size_x = IMG_SIZE_X,
    .size_y = IMG_SIZE_Y,
    .image  = image_buff
};


static sixel_t *sixel = NULL;


int test_2_free(void) {
    if (!sixel_free(&sixel)) {
        return -1;
    }

    return 0;
}

int test_2_init(void) {
    if (!sixel_build_color_map_palete(SIXEL_COLOR_PALETE_RGB216, &color_count, color_map)) {
        goto end;
    }

    if (!sixel_init(&sixel, IMG_SIZE_X, IMG_SIZE_Y)) {
        goto end;
    }

    if (!sixel_cmap_init(sixel, SIXEL_COLOR_MODEL_RGB, color_count, color_map)) {
        goto end;
    }

    if (!sixel_frame_init(sixel)) {
        goto end;
    }

    return 0;
end:
    test_2_free();
    return -1;
}

int test_2_loop(uint32_t type) {
    build_image_map(&image, color_count, type);

    if (!sixel_frame_draw(sixel, &image)) {
        goto end;
    }

    return 0;
end:
    test_2_free();
    return -1;
}
