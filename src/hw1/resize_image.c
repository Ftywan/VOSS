#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // TODO Fill in
    int new_x = roundf(x);
    int new_y = roundf(y);
    float value = get_pixel(im, new_x, new_y, c);
    return value;
}

image nn_resize(image im, int w, int h)
{
    image new_image = make_image(w, h, im.c);

    for(int i = 0; i < im.c; i ++) {
        for(int j = 0; j < h; j ++) {
            for(int k = 0; k < w; k ++) {
                // map to the original coordinate
                // the beginning positon should be: -0.5 + a/2 (a is the ratio of original / new)
                float a = (float)im.w / w;

                float x = (-0.5) + a / 2.0 + (float)k * a;
                float y = (-0.5) + a / 2.0 + (float)j * a;
                
                set_pixel(new_image, k, j, i, nn_interpolate(im, x, y, i));
            }
        }
    }

    return new_image;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    int l, r, u, d;
    l = (int)x;
    r = l + 1;
    u = (int)y;
    d = u + 1;

    float V1, V2, V3, V4;
    V1 = get_pixel(im, l, u, c);
    V2 = get_pixel(im, r, u, c);
    V3 = get_pixel(im, l, d, c);
    V4 = get_pixel(im, r, d, c);

    float d1, d2, d3, d4;
    d1 = x - l;
    d2 = r - x;
    d3 = y - u;
    d4 = d - y;
    
    if(l < 0) {
        V1 = 0;
        V3 = 0;
        d1 = 0;
    }
    if(r > im.w - 1) {
        V2 = 0;
        V4 = 0;
        d2 = 0;
    }
    if(u < 0) {
        V1 = 0;
        V2 = 0;
        d3 = 0;
    }
    if(d > im.h - 1) {
        V3 = 0;
        V4 = 0;
        d4 = 0;
    }

    float q1, q2, q;
    q1 = V1 * d2 + V2 * d1;
    q2 = V3 * d2 + V4 * d1;
    q = q1 * d4 + q2 * d3;

    return q;
}

image bilinear_resize(image im, int w, int h)
{
    image new_image = make_image(w, h, im.c);

    for(int i = 0; i < im.c; i ++) {
        for(int j = 0; j < h; j ++) {
            for(int k = 0; k < w; k ++) {
                // map to the original coordinate
                // the beginning positon should be: -0.5 + a/2 (a is the ratio of original / new)
                float a = (float)im.w / w;

                float x = -0.5 + a / 2 + (float)k * a;
                float y = -0.5 + a / 2 + (float)j * a;
                
                set_pixel(new_image, k, j, i, bilinear_interpolate(im, x, y, i));
            }
        }
    }    
    return new_image;
}

