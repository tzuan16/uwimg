#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= im.w) x = im.w - 1;
    if (y >= im.h) y = im.h - 1;
    return im.data[c * im.w * im.h + y * im.w + x];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (x < 0 || y < 0 || x >= im.w || y >= im.h) return;
    im.data[c * im.w * im.h + y * im.w + x] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    for (int i = 0; i < im.h * im.w * im.c; i++) {
        copy.data[i] = im.data[i];
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for (int i = 0; i < im.h * im.w; i++) {
        gray.data[i] = im.data[i] * 0.299 +
                       im.data[i + im.w * im.h] * 0.587 +
                       im.data[i + 2 * im.w * im.h] * 0.114;
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int i = 0; i < im.h; i++) {
        for (int j = 0; j < im.w; j++) {
            set_pixel(im, j, i, c, get_pixel(im, j, i, c) + v);
        }
    }
}

void clamp_image(image im)
{
    for (int i = 0; i < im.h * im.w * im.c; i++) {
        if (im.data[i] > 1) im.data[i] = 1;
        else if (im.data[i] < 0) im.data[i] = 0;
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    for (int i = 0; i < im.h * im.w; i++) {
        float R = im.data[i], G = im.data[i + im.h * im.w], B = im.data[i + 2 * im.h * im.w];
        float V = three_way_max(R, G, B);
        float m = three_way_min(R, G, B);
        float C = V - m;
        // Set V value at B slot
        im.data[i + 2 * im.h * im.w] = V;
        // Set S value at G slot
        im.data[i + im.h * im.w] = V == 0 ? 0 : C / V;
        // Set H value at R slot
        if (C == 0) {
            im.data[i] = 0;
        } else {
            float H_prime = (V == R ? (G - B) / C : (V == G ? (B - R) / C + 2 : (R - G) / C + 4));
            im.data[i] = H_prime < 0 ? H_prime / 6 + 1 : H_prime / 6;
        }
    }
}

void hsv_to_rgb(image im)
{
    for (int i = 0; i < im.h * im.w; i++) {
        float H = im.data[i], S = im.data[i + im.h * im.w], V = im.data[i + 2 * im.h * im.w];
        if (V == 0) {
            im.data[i] = 0;
            im.data[i + im.h * im.w] = 0;
            im.data[i + 2 * im.h * im.w] = 0;
            continue;
        }
        float C = V * S;
        float H_prime = H * 6;
        float X = C * (1 - fabs(fmod(H_prime, 2) - 1));
        float R, G, B;
        if (H_prime >= 0 && H_prime < 1) {
            R = C; G = X; B = 0;
        } else if (H_prime >= 1 && H_prime < 2) {
            R = X; G = C; B = 0;
        } else if (H_prime >= 2 && H_prime < 3) {
            R = 0; G = C; B = X;
        } else if (H_prime >= 3 && H_prime < 4) {
            R = 0; G = X; B = C;
        } else if (H_prime >= 4 && H_prime < 5) {
            R = X; G = 0; B = C;
        } else if (H_prime >= 5 && H_prime < 6) {
            R = C; G = 0; B = X;
        } else {
            R = 0; G = 0; B = 0;
        }
        float m = V - C;
        // Set B value at V slot
        im.data[i + 2 * im.h * im.w] = B + m;
        // Set G value at S slot
        im.data[i + im.h * im.w] = G + m;
        // Set R value at H slot
        im.data[i] = R + m;
    }
}

void scale_image(image im, int c, float v)
{
    for (int i = 0; i < im.h; i++) {
        for (int j = 0; j < im.w; j++) {
            set_pixel(im, j, i, c, get_pixel(im, j, i, c) * v);
        }
    }
}
