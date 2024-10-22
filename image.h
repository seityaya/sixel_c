#ifndef IMAGE_H
#define IMAGE_H

#include "sixel.h"

void image_clear(int x, int y, sixel_color_t *image);
void image_print(int x, int y, sixel_color_t *image);

void build_image_map(int x, int y, sixel_color_t *image, int color_cnt, sixel_color_t colormap[color_cnt]);
void build_image_mod(int x, int y, sixel_color_t *image);
void build_image_cat(int x, int y, sixel_color_t *image);

#endif // IMAGE_H
