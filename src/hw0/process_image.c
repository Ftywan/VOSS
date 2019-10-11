#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO: debug
    if(x < 0) x = 0;
    if(x >= im.w) x = im.w - 1;
    if(y < 0) y = 0;
    if(y >= im.h) y = im.h - 1;

    int position = y * im.w + x + im.w * im.h * c;
    float pixel = im.data[position];
    return pixel;
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if(x >= 0 && x < im.w && y >= 0 && y < im.h && c >= 0 && c < im.c) {
        int position = x + y * im.w + im.w * im.h * c;
        im.data[position] = v; 
    } else {
        return;
    }

}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    int num_of_pixels = im.w * im.h * im.c;
    memcpy(copy.data, im.data, 4 * num_of_pixels);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for(int i = 0; i < im.w; i ++){
        for(int j = 0; j < im.h; j ++) {
            float value = 0.299 * get_pixel(im, i, j, 0) + 0.587 * get_pixel(im, i, j, 1) + 0.114 * get_pixel(im, i, j, 2);
            set_pixel(gray, i, j, 0, value);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for(int i = 0; i < im.w; i ++) {
        for (int j = 0; j < im.h; j ++) {
            set_pixel(im, i, j, c, get_pixel(im, i, j, c) + v);
        }
    }
}

void clamp_image(image im)
{
    for(int i = 0; i < im.w; i ++) {
        for(int j = 0; j < im.h; j ++) {
            for(int k = 0; k < im.c; k ++) {
                if(get_pixel(im, i, j, k) > 1) set_pixel(im, i, j, k, 1);
                if(get_pixel(im, i, j, k) < 0) set_pixel(im, i, j, k, 0);
            }
        }
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
    // TODO Fill this in
    for(int i = 0; i < im.w; i ++) {
        for(int j = 0; j < im.h; j ++) {
            // get RGB values
            float R = get_pixel(im, i, j, 0);
            float G = get_pixel(im, i, j, 1);
            float B = get_pixel(im, i, j, 2);

            // calculate the value of V
            float V = three_way_max(R, G, B);

            // calculate the value of S
            float m = three_way_min(R, G, B);
            float C = V - m;
            float S = 0;
            if(V != 0) S = C / V;

            // calculate the value of H'
            float H0 = 0;
            if(C != 0) {
                if(V == R) H0 = (G - B) / C;
                if(V == G) H0 = (B - R) / C + 2;
                if(V == B) H0 = (R - G) / C + 4;
            }

            // calculate the value of H
            float H;
            if(H0 < 0) H = H0 / 6 + 1;
            else H = H0 / 6.0;

            set_pixel(im, i, j, 0, H);
            set_pixel(im, i, j, 1, S);
            set_pixel(im, i, j, 2, V);
        }
    }
}
void hsv_to_rgb(image im)
{
    for(int i = 0; i < im.w; i ++) {
        for(int j = 0; j < im.h; j ++) {
            float H = 360 * get_pixel(im, i, j, 0);
            float S = get_pixel(im, i, j, 1);
            float V = get_pixel(im, i, j, 2);

            float C = V * S;
            // X = C × (1 - |(H / 60°) mod 2 - 1|)
            float X = C * (1 - fabs(fmod(H / 60, 2) - 1));
            float m = V - C;

            float R = 0, G = 0, B = 0;

            if (H >= 0 && H < 60){
                R = C;
                G = X;
                B = 0;
            } else if (H >= 60 && H < 120) {
                R = X;
                G = C;
                B = 0;
            } else if (H >= 120 && H < 180) {
                R = 0;
                G = C;
                B = X;
            } else if (H >= 180 && H < 240) {
                R = 0;
                G = X;
                B = C;
            } else if (H >= 240 && H < 300) {
                R = X;
                G = 0;
                B = C;
            } else if (H >= 300 && H < 360) {
                R = C;
                G = 0;
                B = X;
            }

            set_pixel(im, i, j, 0, R + m);
            set_pixel(im, i, j, 1, G + m);
            set_pixel(im, i, j, 2, B + m);
        }
    }
}
