#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    for(int i = 0; i < im.c; i ++) {
        float sum = 0;
        for(int j = 0; j < im.h; j ++) {
            for(int k = 0; k < im.w; k ++) {
                sum += get_pixel(im, k, j, i);
            }
        }

        for(int j = 0; j < im.h; j ++) {
            for(int k = 0; k < im.w; k ++) {
                set_pixel(im, k, j, i, get_pixel(im, k, j, i) / sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    image filter = make_image(w, w, 1);
    float num_of_pixels = (float)w * (float)w;
    for(int i = 0; i < w; i ++) {
        for(int j = 0; j < w; j ++) {
            set_pixel(filter, j, i, 0, 1.0 / num_of_pixels);
        }
    }
    return filter;
}

void convolve_channel(image im, int im_c, image filter, int filter_c, image new, int new_c) {
    // convolve within a single channel
    // write the data to new image
    for(int i = 0; i < new.h; i ++) {
        for(int j = 0; j < new.w; j ++) {
            // calculate the value of pixel (i, j)
            // index of center of filter is the same as the half value
            int filter_x_half = (filter.w - 1) / 2;
            int filter_y_half = (filter.h - 1) / 2;
            float value = 0;

            for(int y = i - filter_y_half; y <= i + filter_y_half; y ++) {
                for(int x = j - filter_x_half; x <= j + filter_x_half; x ++) {
                    value += get_pixel(im, x, y, im_c) * get_pixel(filter, filter_x_half + x - j, filter_y_half + y - i, filter_c);
                }
            }
            set_pixel(new, j, i, new_c, value);
        }
    }
}

image sum_channel(image im) {
    image new = make_image(im.w, im.h, 1);
    for(int i = 0; i < im.h; i ++) {
        for(int j = 0; j < im.w; j ++) {
            float value = 0;
            for(int c = 0; c < im.c; c ++) {
                value += get_pixel(im, j, i, c);
            }
            set_pixel(new, j, i, 0, value);
        }
    }
    return new;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == 1 || filter.c == im.c);
    image new = make_image(im.w, im.h, im.c);
    if(filter.c == im.c) {
        for(int c = 0; c < im.c; c ++) {
            convolve_channel(im, c, filter, c, new, c);
        }
        if(preserve == 1) return new;
        else {
            return sum_channel(new);
        }
    }
    if(filter.c == 1 && im.c > 1) {
        for(int c = 0; c < im.c; c ++) {
            convolve_channel(im, c, filter, 0, new, c);
        }
        if(preserve == 1) return new;
        else {
            return sum_channel(new);
        }
    }
}

image make_highpass_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 1, 1, 0, 4);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);

    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 1, 1, 0, 5);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);

    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, -2);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 1, 1, 0, 1);
    set_pixel(filter, 1, 2, 0, 1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 2, 1, 0, 1);
    set_pixel(filter, 2, 2, 0, 2);

    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    int edge = 6 * sigma;
    if (edge % 2 == 0) edge += 1;

    image filter = make_image(edge, edge, 1);
    int center = (edge - 1) / 2;
    for(int i = 0; i < edge; i ++) {
        for(int j = 0; j < edge; j ++) {
            float x = j - center;
            float y = i - center;
            float g = (1.0 /(TWOPI * powf(sigma, 2))) * expf((-1) * (powf(x, 2) + powf(y, 2)) / (2 * powf(sigma, 2)));
            set_pixel(filter, j, i, 0, g);
        }
    }
    return filter;
}

image add_image(image a, image b)
{
    // image new = make_image(a.w, a.h, a.c);

    // for(int i = 0; i < a.c; i ++) {
    //     for(int j= 0; j < a.h; j ++) {
    //         for(int k = 0; j < a.w; k ++) {
    //             set_pixel(new, k, j, i, get_pixel(a, k, j, i) + get_pixel(b, k, j, i));
    //         }
    //     }
    // }
    return make_image(1,1,1);
}

image sub_image(image a, image b)
{
    // image new = make_image(a.w, a.h, a.c);

    // for(int i = 0; i < a.c; i ++) {
    //     for(int j = 0; j < a.h; j ++) {
    //         for(int k = 0; j < a.w; k ++) {
    //             set_pixel(new, k, j, i, get_pixel(a, k, j, i) - get_pixel(b, k, j, i));
    //         }
    //     }
    // }
    return make_image(1,1,1);
}

image make_gx_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, -2);
    set_pixel(filter, 0, 2, 0, -1);
    set_pixel(filter, 1, 0, 0, 0);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 1, 2, 0, 0);
    set_pixel(filter, 2, 0, 0, 1);
    set_pixel(filter, 2, 1, 0, 2);
    set_pixel(filter, 2, 2, 0, 1);

    return filter;
}

image make_gy_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, 0);
    set_pixel(filter, 0, 2, 0, 1);
    set_pixel(filter, 1, 0, 0, -2);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 1, 2, 0, 2);
    set_pixel(filter, 2, 0, 0, -1);
    set_pixel(filter, 2, 1, 0, 0);
    set_pixel(filter, 2, 2, 0, 1);

    return filter;
}

void feature_normalize(image im)
{
    int max = 0, min = 0;
    for(int i = 0; i < im.c; i ++) {
        for(int j = 0; j < im.h; j ++) {
            for(int k = 0; k < im.w; k ++) {
                float value = get_pixel(im, k, j, i);
                if(value > max) max = value;
                if(value < min) min = value;
            }
        }
    }

    float range = max - min;

    for(int i = 0; i < im.c; i ++) {
        for(int j = 0; j < im.h; j ++) {
            for(int k = 0; k < im.w; k ++) {
                if(range == 0) set_pixel(im, k, j, i, 0);
                else set_pixel(im, k, j, i, (get_pixel(im, k, j, i) - min) / range);
            }
        }
    }
}

image *sobel_image(image im)
{
    void *images = calloc(2, sizeof(image));
    image gx = convolve_image(im, make_gx_filter(), 0);
    image gy = convolve_image(im, make_gy_filter(), 0);
    image magnitude = make_image(im.w, im.h, 1);
    image direction = make_image(im.w, im.h, 1);

    for(int i = 0; i < im.h; i ++) {
        for(int j = 0; j < im.w; j ++) {
            float gx_value = get_pixel(gx, j, i, 0);
            float gy_value = get_pixel(gy, j, i, 0);            
            set_pixel(magnitude, j, i, 0, sqrtf(powf(gx_value, 2) + powf(gy_value, 2)));

            if(gx_value == 0) set_pixel(direction, j, i, 0, 0);
            else set_pixel(direction, j, i, 0, atanf(gy_value / gx_value));
        }
    }
    feature_normalize(magnitude);
    feature_normalize(direction);

    memcpy(images, &magnitude, sizeof(image));
    memcpy(images + sizeof(image), &direction, sizeof(image));

    return images;
}

image colorize_sobel(image im)
{
    image new = make_image(im.w, im.h, im.c);
    image gx = convolve_image(im, make_gx_filter(), 1);
    image gy = convolve_image(im, make_gy_filter(), 1);


    for(int i = 0; i < im.c; i ++) {
        for(int j = 0; j < im.h; j ++) {
            for(int k = 0; k < im.w; k ++) {
            float gx_value = get_pixel(gx, j, i, 0);
            float gy_value = get_pixel(gy, j, i, 0);    
                set_pixel(new, k, j, i, sqrtf(powf(gx_value, 2) + powf(gy_value, 2)) + atanf(gy_value / gx_value) + 1);
            }
        }
    }

    new = convolve_image(new, make_gaussian_filter(2), 1);

    return new;
}
