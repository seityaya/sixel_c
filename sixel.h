#ifndef SIXEL_H
#define SIXEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SIXEL_MAS(m, x_size, y_size, x, y) m[ (((x_size)*(y)) + (x)) ]

#define SIXEL_RGB(r, g, b) (sixel_color_model_t){.PX = 0        , .Px = (r)      , .Py = (g), .Pz = (b)}
#define SIXEL_HLS(h, l, s) (sixel_color_model_t){.PX = ((h)/255), .Px = ((h)%255), .Py = (l), .Pz = (s)}

typedef enum {
    SIXEL_PIXEL_IN_ONE_LINE = 6,
    SIXEL_SIZE_MAX_IMAGE_X  = 1000,
    SIXEL_SIZE_MAX_IMAGE_Y  = 9999,
    SIXEL_COLOR_MAX_COUNT   = 256,
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

typedef enum {
    SIXEL_COLOR_MODEL_RGB_MAX_R = 100,
    SIXEL_COLOR_MODEL_RGB_MAX_G = 100,
    SIXEL_COLOR_MODEL_RGB_MAX_B = 100,
} sixel_color_model_rgb_e;

typedef enum {
    SIXEL_COLOR_MODEL_HLS_MAX_H = 360,
    SIXEL_COLOR_MODEL_HLS_MAX_L = 100,
    SIXEL_COLOR_MODEL_HLS_MAX_S = 100,
} sixel_color_model_hls_e;


typedef struct __attribute__((packed)) {
    uint8_t PX;
    uint8_t Px;
    uint8_t Py;
    uint8_t Pz;
} sixel_color_model_t;

typedef uint8_t sixel_color_t;

typedef struct {
    uint_fast32_t  size_y;
    uint_fast32_t  size_x;
    sixel_color_t *image;
} sixel_image_t;

typedef struct {
    uint_fast32_t  out_buff_len;
    uint_fast32_t  out_buff_shift;
    uint_fast32_t  out_buff_shift_init;
    char          *out_buff;

    uint_fast32_t  max_size_x;
    uint_fast32_t  max_size_y;

    uint_fast8_t         color_cnt;
    sixel_color_model_e  color_mod;
    sixel_color_model_t *color_map;
} sixel_t;


bool sixel_init(sixel_t **sixel, uint_fast32_t max_size_x, uint_fast32_t max_size_y);
bool sixel_cmap_init(sixel_t *sixel, sixel_color_model_e color_mod, uint_fast8_t color_cnt, sixel_color_model_t *color_map);
bool sixel_frame_init(sixel_t *sixel);
bool sixel_frame_draw(sixel_t *sixel, sixel_image_t *image);
bool sixel_free(sixel_t **sixel);

//***********************************************************************************************************************

typedef enum {
    SIXEL_COLOR_PALETE_GREYSCALE  = 1,
    SIXEL_COLOR_PALETE_RGB216     = 2,
} sixel_color_palete_e;

typedef struct {
    union {
        struct __attribute__((packed)) {
            uint8_t A;
            uint8_t B;
            uint8_t G;
            uint8_t R;
        };
        uint32_t C;
    };
} color_t;

typedef struct {
    uint_fast32_t  size_x;
    uint_fast32_t  size_y;
    color_t       *image;
} image_t;

bool sixel_build_color_map_palete(sixel_color_palete_e model, uint_fast8_t* out_color_count, sixel_color_model_t *out_color_map);

bool sixel_frame_conv(sixel_color_palete_e model, image_t *in_image, sixel_image_t *out_image);

#endif /*SIXEL_H*/


