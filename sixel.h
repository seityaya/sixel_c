#ifndef SIXEL_H
#define SIXEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SIXEL_RGB(r, g, b) (sixel_color_model_t){.PX = 0,         .Px = (r),       .Py = (g), .Pz = (b)}
#define SIXEL_HSL(h, l, s) (sixel_color_model_t){.PX = ((h)/255), .Px = ((h)%255), .Py = (l), .Pz = (s)}

typedef enum {
    SIXEL_MAX_SIZE_IMAGE_X = 1000,
    SIXEL_MAX_SIZE_IMAGE_Y = 9999,
    SIXEL_MAX_COLOR_COUNT  = 256,
} sixel_constant_e;

typedef enum {
    SIXEL_PIXEL_ASPERO_RATIO_DEF = 0,
    SIXEL_PIXEL_ASPERO_RATIO_2_1 = 0, // none, 1, 5, 6
    SIXEL_PIXEL_ASPERO_RATIO_5_1 = 2,
    SIXEL_PIXEL_ASPERO_RATIO_3_1 = 3, // 4
    SIXEL_PIXEL_ASPERO_RATIO_1_1 = 7, // 8, 9
} sixel_pixel_aspero_ratio_e;

typedef enum {
    SIXEL_PIXEL_BACKGRAUND_COLOR_DEF    = 0,
    SIXEL_PIXEL_BACKGRAUND_COLOR_CURENT = 0, // 2
    SIXEL_PIXEL_BACKGRAUND_COLOR_REMAIN = 1,
} sixel_pixel_backgraund_color_e;

typedef enum {
    SIXEL_CONTROL_FUNCTIONS_ESCAPE                 = '\e',
    SIXEL_CONTROL_FUNCTIONS_BEG_DATA_CHARACTERS    = '?' ,
    SIXEL_CONTROL_FUNCTIONS_BEG_PARAM_CHARACTERS   = 'P' ,
    SIXEL_CONTROL_FUNCTIONS_BEG_COMAND_CHARACTERS  = 'q' ,
    SIXEL_CONTROL_FUNCTIONS_END_DATA_CHARACTERS    = '~' ,
    SIXEL_CONTROL_FUNCTIONS_STRING_TERMINATOR      = '\\',
    SIXEL_CONTROL_FUNCTIONS_REPET                  = '!' ,
    SIXEL_CONTROL_FUNCTIONS_RASTER_ATTRIBUTES      = '\"',
    SIXEL_CONTROL_FUNCTIONS_COLOR_INTRODUCER       = '#' ,
    SIXEL_CONTROL_FUNCTIONS_CARRIAGE_RETURN        = '$' ,
    SIXEL_CONTROL_FUNCTIONS_NEW_LINE               = '-' ,
    SIXEL_CONTROL_FUNCTIONS_SEPARATOR              = ';' ,
} sixel_control_functions_e;

typedef enum {
    SIXEL_HORIZONTAL_GRID_SIZE_DEF = 0,
} sixel_horizontal_grid_size_e;

typedef enum {
    SIXEL_RASTER_ATTRIBUTES_PAN_DEF = 1,
    SIXEL_RASTER_ATTRIBUTES_PAD_DEF = 1,
} sixel_raster_attributes_e;

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
    int                  out_buff_len;
    int                  out_buff_shift;
    int                  out_buff_shift_init;
    char                *out_buff;

    unsigned int         max_size_x;
    unsigned int         max_size_y;

    sixel_color_model_e  color_mod;
    unsigned int         color_cnt;
    sixel_color_t       *color_map;
} sixel_t;

bool sixel_color(int *color_cnt, sixel_color_t *color_map);

bool sixel_init(sixel_t **sixel, unsigned int max_size_x, unsigned int max_size_y);

bool sixel_cmap_init(sixel_t *sixel, sixel_color_model_e color_mod, unsigned int color_cnt, sixel_color_t color_map[color_cnt]);

bool sixel_draw_init(sixel_t *sixel);
bool sixel_draw(sixel_t *sixel, int size_x, int size_y, sixel_color_t image[size_x * size_y]);

bool sixel_free(sixel_t **sixel);


#endif /*SIXEL_H*/


