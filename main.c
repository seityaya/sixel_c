#include <stdio.h>

#include "mtime.h"
#include "sixel.h"

extern int test_1_init(void);
extern int test_1_loop(uint32_t type);
extern int test_1_free(void);

extern int test_2_init(void);
extern int test_2_loop(uint32_t type);
extern int test_2_free(void);

extern int test_3_init(void);
extern int test_3_loop(uint32_t type);
extern int test_3_free(void);

#define CONSOLE_HIDE_CURSOR "\e[?25l"
#define CONSOLE_VIEW_CURSOR "\e[?25h"

#define CONSOLE_CLEAR "\e[2J"
#define CONSOLE_SETCURSORBEG "\e[H"
#define CONCOLE_PADING "\n\n\n     "

int main(int argc, char *argv[]) {
    //*******************************************************
    uint32_t test_cycles = 20;
    uint64_t test_frame_fps_milisec = 1;

    uint32_t test_number = 3; // 1 - 3

    uint32_t test_1_type = 6; // 0 - 7
    uint32_t test_2_type = 5; // 0 - 5
    //*******************************************************

    realtime_t beg = {0};
    realtime_t cen = {0};
    realtime_t end = {0};
    long double average_loop_time = 0;
    long double average_frame_time = 0;

    int err = 0;

    {
        printf(CONSOLE_HIDE_CURSOR);

        switch (test_number) {
        case 1: {
            if ((err = test_1_init())) {
                goto end;
            }
            break;
        }
        case 2: {
            if ((err = test_2_init())) {
                goto end;
            }
            break;
        }
        case 3: {
            if ((err = test_3_init())) {
                goto end;
            }
            break;
        }
        default:
            break;
        }
    }

    for (int i = 1; i <= test_cycles; ++i) {
        printf(CONSOLE_SETCURSORBEG);

        beg = get_time_real();
        switch (test_number) {
        case 1: {
            if ((err = test_1_loop(test_1_type))) {
                goto end;
            }
            break;
        }
        case 2: {
            if ((err = test_2_loop(test_2_type))) {
                goto end;
            }
            break;
        }
        case 3: {
            if ((err = test_3_loop(0))) {
                goto end;
            }
            break;
        }
        default:
            break;
        }
        cen = get_time_real();
        time_sleep(time_dif(time_build(test_frame_fps_milisec / MILLI, (test_frame_fps_milisec % MILLI) * MICRO), time_dif(cen, beg)));
        end = get_time_real();

        average_loop_time += time_convert(time_dif(cen, beg));
        average_frame_time += time_convert(time_dif(end, beg));

        printf("[frame]\n");
        printf("elapsed time: %15.10Lf ticks, frame time: %15.10Lf\n", time_convert(time_dif(cen, beg)), time_convert(time_dif(end, beg)));
        printf("cycle count: %d ticks\n", i);
        fflush(stdout);
    }

    printf("[average]\n");
    printf("elapsed time: %15.10Lf ticks, frame time: %15.10Lf\n", average_loop_time / test_cycles, average_frame_time / test_cycles);

    {
        switch (test_number) {
        case 1: {
            if ((err = test_1_free())) {
                goto end;
            }
            break;
        }
        case 2: {
            if ((err = test_2_free())) {
                goto end;
            }
            break;
        }
        case 3: {
            if ((err = test_3_free())) {
                goto end;
            }
            break;
        }
        default:
            break;
        }

        printf(CONSOLE_VIEW_CURSOR);
    }

    return 0;

end:
    printf("error %d \n", err);
    return err;
}
