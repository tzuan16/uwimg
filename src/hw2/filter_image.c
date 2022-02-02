#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    for (int c = 0; c < im.c; c++) {
        float sum = 0;
        for (int i = 0; i < im.h * im.w; i++) {
            sum += im.data[c * im.h * im.w + i];
        }
        for (int i = 0; i < im.h * im.w; i++) {
            im.data[c * im.h * im.w + i] /= sum;
        }
    }
}

image make_box_filter(int w)
{
    image filter = make_image(w, w, 1);
    for (int i = 0; i < w * w; i++) {
        filter.data[i] = 1;
    }
    l1_normalize(filter);
    return filter;
}

float get_convolved_value(image im, image filter, int preserve, int x, int y, int c) {
    float value = 0;
    for (int m = 0; m < filter.h; m++) {
        for (int n = 0; n < filter.w; n++) {
            if (preserve == 1) {
                value += get_pixel(im, x - (filter.w-1)/2 + n, y - (filter.h-1)/2 + m, c) * get_pixel(filter, n, m, filter.c == 1 ? 0 : c);
            } else {
                for (int k = 0; k < im.c; k++) {
                    value += get_pixel(im, x - (filter.w-1)/2 + n, y - (filter.h-1)/2 + m, k) * get_pixel(filter, n, m, filter.c == 1 ? 0 : k);
                }
            }
        }
    }
    return value;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == 1 || filter.c == im.c);
    image convolved = make_image(im.w, im.h, preserve == 1 ? im.c : 1);
    for (int i = 0; i < im.h; i++) {
        for (int j = 0; j < im.w; j++) {
            if (preserve == 1) {
                for (int c = 0; c < im.c; c++) {
                    set_pixel(convolved, j, i, c, get_convolved_value(im, filter, preserve, j, i, c));
                }
            } else {
                set_pixel(convolved, j, i, 0, get_convolved_value(im, filter, preserve, j, i, 0));
            }
        }
    }
    return convolved;
}

image make_highpass_filter()
{
    image filter = make_image(3, 3, 1);

    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 4);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);

    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3, 3, 1);

    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 5);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);

    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3, 3, 1);

    set_pixel(filter, 0, 0, 0, -2);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 1);
    set_pixel(filter, 2, 1, 0, 1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, 1);
    set_pixel(filter, 2, 2, 0, 2);

    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: Use preserve for sharpen and emboss, use without for highpass.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Yes, we have to call clamp_image() since values might go above 1 after the convolution, causing overflow when storing to disk.

image make_gaussian_filter(float sigma)
{
    int tmp = ceil(sigma * 6);
    int w = (tmp % 2) ? tmp : tmp + 1;
    image filter = make_image(w, w, 1);
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < w; y++) {
            float G = exp(-1 * ((x-w/2)*(x-w/2) + (y-w/2)*(y-w/2)) / (2*sigma*sigma)) / (2*M_PI*sigma*sigma);
            set_pixel(filter, x, y, 0, G);
        }
    }
    l1_normalize(filter);
    return filter;
}

image add_image(image a, image b)
{
    assert(a.h == b.h && a.w == b.w && a.c == b.c);
    image img = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.w * a.h * a.c; i++) {
        img.data[i] = a.data[i] + b.data[i];
    }
    return img;
}

image sub_image(image a, image b)
{
    assert(a.h == b.h && a.w == b.w && a.c == b.c);
    image img = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.w * a.h * a.c; i++) {
        img.data[i] = a.data[i] - b.data[i];
    }
    return img;
}

image make_gx_filter()
{
    image filter = make_image(3, 3, 1);

    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 1, 0, 0, 0);
    set_pixel(filter, 2, 0, 0, 1);
    set_pixel(filter, 0, 1, 0, -2);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 2, 1, 0, 2);
    set_pixel(filter, 0, 2, 0, -1);
    set_pixel(filter, 1, 2, 0, 0);
    set_pixel(filter, 2, 2, 0, 1);

    return filter;
}

image make_gy_filter()
{
    image filter = make_image(3, 3, 1);

    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 1, 0, 0, -2);
    set_pixel(filter, 2, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, 0);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 2, 1, 0, 0);
    set_pixel(filter, 0, 2, 0, 1);
    set_pixel(filter, 1, 2, 0, 2);
    set_pixel(filter, 2, 2, 0, 1);

    return filter;
}

void feature_normalize(image im)
{
    for (int c = 0; c < im.c; c++) {
        float min = MAXFLOAT, max = 0;
        for (int i = 0; i < im.h * im.w; i++) {
            min = fmin(im.data[c * im.h * im.w + i], min);
            max = fmax(im.data[c * im.h * im.w + i], max);
        }
        for (int i = 0; i < im.h * im.w; i++) {
            im.data[c*im.h*im.w + i] = (max - min == 0) ? 0 : (im.data[c*im.h*im.w + i]-min) / (max-min);
        }
    }
}

image *sobel_image(image im)
{
    image gx = convolve_image(im, make_gx_filter(), 0), gy = convolve_image(im, make_gy_filter(), 0);
    image *imgs = calloc(2, sizeof(image));
    for (int i = 0; i < 2; i++) {
        imgs[i].w = im.w;
        imgs[i].h = im.h;
        imgs[i].c = 1;
        imgs[i].data = calloc(im.w * im.h, sizeof(float));
    }
    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            float G = sqrt(pow(get_pixel(gx, x, y, 0), 2) + pow(get_pixel(gy, x, y, 0), 2));
            set_pixel(imgs[0], x, y, 0, G);

            float Theta = atan2(get_pixel(gy, x, y, 0), get_pixel(gx, x, y, 0));
            set_pixel(imgs[1], x, y, 0, Theta);
        }
    }
    return imgs;
}

image colorize_sobel(image im)
{
    image *imgs = sobel_image(im);
    image colorized = make_image(im.w, im.h, 3);
    for (int k = 0; k < 3; k++) {
        for (int x = 0; x < im.w; x++) {
            for (int y = 0; y < im.h; y++) {
                set_pixel(colorized, x, y, 0, get_pixel(imgs[1], x, y, 0));
                set_pixel(colorized, x, y, 1, get_pixel(imgs[0], x, y, 0));
                set_pixel(colorized, x, y, 2, get_pixel(imgs[0], x, y, 0));
            }
        }
    }
    feature_normalize(colorized);
    hsv_to_rgb(colorized);
    return colorized;
}
