#ifndef IMAGE_H
#define IMAGE_H

#include "sixel.h"

void image_clear(sixel_image_t *image);
void image_print(sixel_image_t *image);

void build_image_map(sixel_image_t *image, uint32_t color_count, uint32_t type);

#endif /*IMAGE_H*/
