#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#include "image.h"
#include "sixel.h"

extern int test_1_init(void);
extern int test_1_loop(void);
extern int test_1_free(void);

extern int test_2_init(void);
extern int test_2_loop(void);
extern int test_2_free(void);

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

int main() {
    uintmax_t beg = 0;
    uintmax_t end = 0;

    int err = 0;

    printf(CONSOLE_HIDE_CURSOR);
    if ((err = test_1_init())) {
        return err;
    }

    for (int i = 0; i < 100; ++i) {
        printf(CONSOLE_SETCURSORBEG);

        beg = times(NULL);

        if ((err = test_1_loop())) {
            return err;
        }

        end = times(NULL);
        printf("elapsed time: %ju ticks\n", end - beg);
        printf("cycle count: %d ticks\n", i);
        fflush(stdout);
        // sleep_milisec(100);
    }

    if ((err = test_1_free())) {
        return err;
    }

    return 0;
}
