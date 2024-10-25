#ifndef IMAGE_H
#define IMAGE_H

#include "sixel.h"

void image_clear(sixel_image_t *image);
void image_print(sixel_image_t *image);

void build_image_map(sixel_image_t *image, uint_fast8_t color_count, uint_fast32_t type);

#endif /*IMAGE_H*/
