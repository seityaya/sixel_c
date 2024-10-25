#ifndef TEST_3_H
#define TEST_3_H

#include "sixel.h"

#include "assets/nyancat/1.h"
#include "assets/nyancat/2.h"
#include "assets/nyancat/3.h"
#include "assets/nyancat/4.h"
#include "assets/nyancat/5.h"
#include "assets/nyancat/6.h"
#include "assets/nyancat/7.h"
#include "assets/nyancat/8.h"
#include "assets/nyancat/9.h"
#include "assets/nyancat/10.h"
#include "assets/nyancat/11.h"
#include "assets/nyancat/12.h"


#define NYANCAT_IMAGE_X 400
#define NYANCAT_IMAGE_Y 400

#define NYANCAT_IMAGE_COUNT (sizeof(nyancat_gif)/sizeof(nyancat_gif[0]))

static image_t nyancat_gif[] = {
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_1)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_2)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_3)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_4)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_5)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_6)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_7)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_8)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_9)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_10)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_11)},
    {.size_x = NYANCAT_IMAGE_X, .size_y = NYANCAT_IMAGE_Y, .image = (color_t*)(nyancat_image_12)},
};



#include "assets/color216/1.h"
#include "assets/color216/2.h"


#define RAINBOW216_IMAGE_H_X 864
#define RAINBOW216_IMAGE_H_Y 20

#define RAINBOW216_IMAGE_COUNT_H 1

static image_t rainbow_image_h[] = {
    {
     .size_x = RAINBOW216_IMAGE_H_X,
     .size_y = RAINBOW216_IMAGE_H_Y,
     .image = (color_t*)(rainbow_image_h_1)
    }
};


#define RAINBOW216_IMAGE_V_X 20
#define RAINBOW216_IMAGE_V_Y 864

#define RAINBOW216_IMAGE_COUNT_V 1

static image_t rainbow_image_v[] = {
    {
     .size_x = RAINBOW216_IMAGE_V_X,
     .size_y = RAINBOW216_IMAGE_V_Y,
     .image = (color_t*)(rainbow_image_v_1)
    }
};

#endif /*TEST_3_H*/
