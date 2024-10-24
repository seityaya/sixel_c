#include "image.h"
#include "sixel.h"

#define IMG_SIZE_X  (17*50)
#define IMG_SIZE_Y  (17*10)

static sixel_color_model_t color_map[] = {
    SIXEL_RGB(00, 00, 00), /*  0 Black    */
    SIXEL_RGB(00, 00, 99), /*  1 Blue     */
    SIXEL_RGB(99, 00, 00), /*  2 Red      */
    SIXEL_RGB(00, 99, 00), /*  3 Green    */
    SIXEL_RGB(80, 20, 80), /*  4 Magenta  */
    SIXEL_RGB(20, 80, 80), /*  5 Cyan     */
    SIXEL_RGB(80, 80, 20), /*  6 Yellow   */
    SIXEL_RGB(53, 53, 53), /*  7 Gray 50% */
    SIXEL_RGB(26, 26, 26), /*  8 Gray 25% */
    SIXEL_RGB(33, 33, 60), /*  9 Blue*    */
    SIXEL_RGB(60, 26, 26), /* 10 Red*     */
    SIXEL_RGB(33, 60, 33), /* 11 Green*   */
    SIXEL_RGB(60, 33, 60), /* 12 Magenta* */
    SIXEL_RGB(33, 60, 60), /* 13 Cyan*    */
    SIXEL_RGB(60, 60, 33), /* 14 Yellow*  */
    SIXEL_RGB(80, 80, 80), /* 15 Gray 75% */
    SIXEL_RGB(99, 99, 99), /* 16 White    */  //TODO: Перепроверить цвета
};
static int color_count = (sizeof(color_map) / sizeof(color_map[0]));


static sixel_color_t image_buff[IMG_SIZE_X * IMG_SIZE_Y] = {0};
static sixel_image_t image = {
    .size_x = IMG_SIZE_X,
    .size_y = IMG_SIZE_Y,
    .image  = image_buff
};


static sixel_t *sixel = NULL;


int test_1_free(void) {
    if (!sixel_free(&sixel)) {
        return -1;
    }

    return 0;
}

int test_1_init(void) {
    if (!sixel_init(&sixel, IMG_SIZE_X, IMG_SIZE_Y)) {
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
    test_1_free();
    return -1;
}

int test_1_loop(uint32_t type) {
    build_image_map(&image, color_count, type);

    if (!sixel_draw(sixel, &image)) {
        goto end;
    }

    return 0;
end:
    test_1_free();
    return -1;
}
