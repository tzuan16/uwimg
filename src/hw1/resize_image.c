#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{
    image resize = make_image(w, h, im.c);
    for (int k = 0; k < im.c; k++) {
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                set_pixel(resize, j, i, k, nn_interpolate(im, (j+0.5) * im.w / w - 0.5, (i+0.5) * im.h / h - 0.5, k));
            }
        }
    }
    return resize;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    float x1 = x - floor(x), x2 = ceil(x) - x, y1 = y - floor(y), y2 = ceil(y) - y;
    return get_pixel(im, floor(x), floor(y), c) * x2 * y2 +
           get_pixel(im, ceil(x), floor(y), c) * x1 * y2 +
           get_pixel(im, floor(x), ceil(y), c) * x2 * y1 +
           get_pixel(im, ceil(x), ceil(y), c) * x1 * y1;
}

image bilinear_resize(image im, int w, int h)
{
    image resize = make_image(w, h, im.c);
    for (int k = 0; k < im.c; k++) {
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                set_pixel(resize, j, i, k, bilinear_interpolate(im, (j+0.5) * im.w / w - 0.5, (i+0.5) * im.h / h - 0.5, k));
            }
        }
    }
    return resize;
}
