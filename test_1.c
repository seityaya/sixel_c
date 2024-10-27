#include "image.h"
#include "sixel.h"

#define IMG_SIZE_X  (17*50)
#define IMG_SIZE_Y  (17*10)

static sixel_color_model_t color_map[] = {
    SIXEL_RGB( 00,  00,  00), /* Black    */
    SIXEL_RGB( 25,  25,  25), /* Gray 25% */
    SIXEL_RGB( 50,  50,  50), /* Gray 50% */
    SIXEL_RGB( 75,  75,  75), /* Gray 75% */
    SIXEL_RGB(100, 100, 100), /* White    */

    SIXEL_RGB( 00,  00, 100), /* Blue     */
    SIXEL_RGB(100,  00,  00), /* Red      */
    SIXEL_RGB( 00, 100,  00), /* Green    */
    SIXEL_RGB(100,  00, 100), /* Magenta  */
    SIXEL_RGB( 00, 100, 100), /* Cyan     */
    SIXEL_RGB(100, 100,  00), /* Yellow   */

    SIXEL_RGB( 33,  33, 100), /* Blue*    */
    SIXEL_RGB(100,  33,  33), /* Red*     */
    SIXEL_RGB( 33, 100,  33), /* Green*   */
    SIXEL_RGB(100,  33, 100), /* Magenta* */
    SIXEL_RGB( 33, 100, 100), /* Cyan*    */
    SIXEL_RGB(100, 100,  33), /* Yellow*  */
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

    if (!sixel_frame_init(sixel)) {
        goto end;
    }

    return 0;
end:
    test_1_free();
    return -1;
}

int test_1_loop(uint32_t type) {
    build_image_map(&image, color_count, type);

    if (!sixel_frame_draw(sixel, &image)) {
        goto end;
    }

    return 0;
end:
    test_1_free();
    return -1;
}
