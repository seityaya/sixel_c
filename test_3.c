#include "image.h"
#include "sixel.h"

#include "test_3.h"


#define IMAGE_MAX_SIZE_X 1000
#define IMAGE_MAX_SIZE_Y 1000


static uint_fast8_t color_count = 0;
static sixel_color_model_t color_map[SIXEL_COLOR_MAX_COUNT] = {0};


static sixel_color_t image_buff[NYANCAT_IMAGE_X * NYANCAT_IMAGE_Y] = {0};
static sixel_image_t image = {
    .size_x = NYANCAT_IMAGE_X,
    .size_y = NYANCAT_IMAGE_Y,
    .image = image_buff
};


static sixel_t *sixel = NULL;


int test_3_free(void) {
    if (!sixel_free(&sixel)) {
        return -1;
    }

    return 0;
}

int test_3_init(void) {
    if (!sixel_image_color_map_palete_build(SIXEL_COLOR_PALETE_RGB216, &color_count, color_map)) {
        goto end;
    }

    if (!sixel_init(&sixel, NYANCAT_IMAGE_X, NYANCAT_IMAGE_Y)) {
        goto end;
    }

    if (!sixel_cmap_init(sixel, SIXEL_COLOR_MODEL_RGB, color_count, color_map)) {
        goto end;
    }

    if (!sixel_draw_init(sixel)) {
        goto end;
    }

    return 0;
end:
    test_3_free();
    return -1;
}

int test_3_loop(uint32_t type) {
    static int i = 0;
    i++;

    if (!sixel_image_color_img_build(SIXEL_COLOR_PALETE_RGB216, &nyancat_gif[i % NYANCAT_IMAGE_COUNT], &image)) {
        goto end;
    }

    if (!sixel_draw(sixel, &image)) {
        goto end;
    }

    return 0;
end:
    test_3_free();
    return -1;
}
