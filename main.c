#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#include "image.h"
#include "sixel.h"

#define CONSOLE_HIDE_CURSOR    "\e[?25l"
#define CONSOLE_CLEAR          "\e[2J"
#define CONSOLE_SETCURSORBEG   "\e[H"
#define CONCOLE_PADING         "\n\n\n     "

void sleep_milisec(int milisec) {
    struct timespec ts;
    ts.tv_sec = (milisec / 1000);
    ts.tv_nsec = (milisec % 1000) * 1000000;
    nanosleep(&ts, &ts);
}

#define IMG_SIZE_X  (250)
#define IMG_SIZE_Y  (250)

int test_1(void) {
    uintmax_t beg = 0;
    uintmax_t end = 0;

    sixel_color_t color_map[] = {
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
    int color_count = (sizeof(color_map) / sizeof(color_map[0]));

    sixel_t *sixel = NULL;
    sixel_color_t image[IMG_SIZE_X * IMG_SIZE_Y] = {0};

    if (!sixel_init(&sixel, IMG_SIZE_X, IMG_SIZE_Y)) {
        goto end;
    }

    if (!sixel_cmap_init(sixel, SIXEL_COLOR_MODEL_RGB, color_count, color_map)) {
        goto end;
    }

    sixel_draw_init(sixel);

    for (;;) {
        printf(CONSOLE_SETCURSORBEG);

        beg = times(NULL);

        build_image_map(IMG_SIZE_X, IMG_SIZE_Y, image, color_count, color_map);

        if (!sixel_draw(sixel, IMG_SIZE_X, IMG_SIZE_Y, image)) {
            goto end;
        }

        end = times(NULL);
        printf("elapsed time: %ju ticks\n", end - beg);
        fflush(stdout);
    }

    if (!sixel_free(&sixel)) {
        goto end;
    }

    return 0;

end:
    if (!sixel_free(&sixel)) {
        return -1;
    }

    return -1;
}

int test_2(void) {
    sixel_t *sixel = NULL;
    sixel_color_t image[IMG_SIZE_X * IMG_SIZE_Y] = {0};

    build_image_mod(IMG_SIZE_X, IMG_SIZE_Y, image);

    sixel_color_t *color_map = 0;
    int color_count = 0;

    if (!sixel_color(&color_count, color_map)) {
        goto end;
    }

    if (!sixel_init(&sixel, IMG_SIZE_X, IMG_SIZE_Y)) {
        goto end;
    }

    if (!sixel_cmap_init(sixel, SIXEL_COLOR_MODEL_RGB, color_count, color_map)) {
        goto end;
    }

    sixel_draw_init(sixel);

    if (!sixel_draw(sixel, IMG_SIZE_X, IMG_SIZE_Y, image)) {
        goto end;
    }

    if (!sixel_free(&sixel)) {
        return -1;
    }

    return 0;

end:
    if (!sixel_free(&sixel)) {
        return -1;
    }

    return -1;
}

int main() {
    printf(CONSOLE_HIDE_CURSOR);

    int err = 0;

    for (int i = 0;; i++) {
        printf(CONSOLE_CLEAR);
        printf(CONSOLE_SETCURSORBEG);

        switch (1) {
        case 1: {
            if ((err = test_1())) {
                return err;
            }
            break;
        }
        case 2: {
            if ((err = test_2())) {
                return err;
            }
            break;
        }
        default:
            break;
        }

        // sleep_milisec(100);
        printf("cycle count: %d ticks\n", i);
    }
    return 0;
}
