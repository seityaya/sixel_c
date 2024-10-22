#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#include "image.h"
#include "sixel.h"

#define IMG_SIZE_X  (250)
#define IMG_SIZE_Y  (250)

static sixel_color_t color_map[] = {
    {.model = SIXEL_RGB(00, 00, 00)}, /*  0 Black    */
    {.model = SIXEL_RGB(00, 00, 99)}, /*  1 Blue     */
    {.model = SIXEL_RGB(99, 00, 00)}, /*  2 Red      */
    {.model = SIXEL_RGB(00, 99, 00)}, /*  3 Green    */
    {.model = SIXEL_RGB(80, 20, 80)}, /*  4 Magenta  */
    {.model = SIXEL_RGB(20, 80, 80)}, /*  5 Cyan     */
    {.model = SIXEL_RGB(80, 80, 20)}, /*  6 Yellow   */
    {.model = SIXEL_RGB(53, 53, 53)}, /*  7 Gray 50% */
    {.model = SIXEL_RGB(26, 26, 26)}, /*  8 Gray 25% */
    {.model = SIXEL_RGB(33, 33, 60)}, /*  9 Blue*    */
    {.model = SIXEL_RGB(60, 26, 26)}, /* 10 Red*     */
    {.model = SIXEL_RGB(33, 60, 33)}, /* 11 Green*   */
    {.model = SIXEL_RGB(60, 33, 60)}, /* 12 Magenta* */
    {.model = SIXEL_RGB(33, 60, 60)}, /* 13 Cyan*    */
    {.model = SIXEL_RGB(60, 60, 33)}, /* 14 Yellow*  */
    {.model = SIXEL_RGB(80, 80, 80)}, /* 15 Gray 75% */
    {.model = SIXEL_RGB(99, 99, 99)}, /* 16 White    */
};

static int color_count = (sizeof(color_map) / sizeof(color_map[0]));

static sixel_t *sixel = NULL;

static sixel_color_t image[IMG_SIZE_X * IMG_SIZE_Y] = {0};

int test_1_free() {
    if (!sixel_free(&sixel)) {
        return -1;
    }
    return 0;
}

int test_1_init() {
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
    test_1_free();
    return -1;
}

int test_1_loop(void) {
    build_image_map(IMG_SIZE_X, IMG_SIZE_Y, image, color_count, color_map);

    if (!sixel_draw(sixel, IMG_SIZE_X, IMG_SIZE_Y, image)) {
        goto end;
    }

    return 0;
end:
    test_1_free();
    return -1;
}
