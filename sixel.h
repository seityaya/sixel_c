#ifndef SIXEL_H
#define SIXEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SIXEL_MAX_SIZE_X 1000
#define SIXEL_MAX_SIZE_Y 9999

#define SIXEL_RGB(r, g, b) (sixel_color_model_t){.PX = 0,         .Px = (r),       .Py = (g), .Pz = (b)}
#define SIXEL_HSL(h, l, s) (sixel_color_model_t){.PX = ((h)/255), .Px = ((h)%255), .Py = (l), .Pz = (s)}

typedef enum {
    SIXEL_COLOR_MODEL_HLS = 1,
    SIXEL_COLOR_MODEL_RGB = 2,
} sixel_color_model_e;

typedef struct __attribute__((packed)) {
    uint8_t PX;
    uint8_t Px;
    uint8_t Py;
    uint8_t Pz;
} sixel_color_model_t;

typedef struct {
    union {
        sixel_color_model_t model;
        uint32_t            color;
    };
} sixel_color_t;

typedef struct {
    unsigned int         out_buff_len;
    unsigned int         out_buff_shift;
    unsigned int         out_buff_shift_max;
    char                *out_buff;
    unsigned int         tmp_buff_len;
    unsigned int         tmp_buff_shift;
    unsigned int         tmp_buff_shift_max;
    char                *tmp_buff;

    sixel_color_model_e  color_mod;
    unsigned int         color_cnt;
    sixel_color_t       *color_map;
} sixel_t;

bool sixel_color(int *color_cnt, sixel_color_t *color_map);

bool sixel_init(sixel_t **sixel, unsigned int max_size_x, unsigned int max_size_y);

bool sixel_init_colormap(sixel_t *sixel, sixel_color_model_e color_mod, unsigned int color_cnt, sixel_color_t color_map[color_cnt]);

bool sixel_draw(sixel_t *sixel, int size_x, int size_y, sixel_color_t image[size_x * size_y]);

bool sixel_free(sixel_t **sixel);


#endif /*SIXEL_H*/


