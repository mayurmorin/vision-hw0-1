#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{   
    x = (x >= 0) ? ( (x < im.w) ? x : im.w-1 ) : 0;
    y = (y >= 0) ? ( (y < im.h) ? y : im.h-1 ) : 0;
    c = (c >= 0) ? ( (c < im.c) ? c : im.c-1 ) : 0;
    return im.data[x + y*im.w + c*im.w*im.h];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (x < 0 || x >= im.w || y < 0 || y >= im.h || c < 0 || c >= im.c) {
        return;
    }
    im.data[x + y*im.w + c*im.w*im.h] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, im.w*im.h*im.c*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    int w = im.w;
    int h = im.h;
    for(int y=0; y < h; y++){
        for(int x=0; x < w; x++){
            float gray_pixel = 0.0;
            float weighted_sum[3] = {0.299, 0.587, 0.114};
            for (int c=0; c < 3; c++) gray_pixel += get_pixel(im, x, y, c) * weighted_sum[c];
            set_pixel(gray, x, y, 0, gray_pixel);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    int h = im.h;
    int w = im.w;
    for(int x=0; x < w; x++){
        for(int y=0; y < h; y++){
            set_pixel(im, x, y, c, get_pixel(im, x, y, c) + v);
        }
    }
}

void clamp_image(image im)
{
    for(int x=0; x < im.w; x++){
        for(int y=0; y < im.h; y++){
            for(int c=0; c < im.c; c++){
                float v = get_pixel(im, x, y, c);
                v = (v>=0.0)?((v > 1.0)? 1:v):0.0;
                set_pixel(im, x, y, c, v);
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
    int w = im.w;
    int h = im.h;
    int x,y;
    float r, g, b;
    float hue, sat, val, m, C, hue_prime;
    for(x=0; x<w; x++){
        for(y=0; y<h; y++){
            r = get_pixel(im, x, y, 0); 
            g = get_pixel(im, x, y, 1); 
            b = get_pixel(im, x, y, 2);
            val = three_way_max(r, g, b);
            m = three_way_min(r, g, b);
            C = val - m;
            if(C != 0.0){
                sat = C / val;
                if(val==r){
                    hue_prime = (g - b)/C;
                }else if(val==g){
                    hue_prime = (b - r)/C + 2;
                }else{
                    hue_prime = (r - g)/C + 4;
                }
                hue = (hue_prime<0) ? (hue_prime/6)+1 : hue_prime/6;
            }else{
                sat = 0;
                hue = 0; 
            }
            set_pixel(im, x, y, 0, hue);
            set_pixel(im, x, y, 1, sat);
            set_pixel(im, x, y, 2, val);
        }
    }
}

void hsv_to_rgb(image im)
{  
    int x,y;
    float hue, sat, val;
    float r, g, b;
    float C, m, H; 
    for(x=0; x<im.w; x++){
        for(y=0; y<im.h; y++){
            hue = get_pixel(im, x, y, 0); 
            sat = get_pixel(im, x, y, 1); 
            val = get_pixel(im, x, y, 2);
            H = hue * 6;
            C = sat * val;
            m = val - C;
            if (sat == 0) {
                r = val;
                g = val;
                b = val;
            } else if (H >= 0 && H <= 1) {
                // R is max, G > B
                r = val;
                b = m;
                g = H*C+b;
            } else if (H > 1 && H <= 2) {
                // G is max, R > B
                g = val;
                b = m;
                r = b-C*(H-2);
            } else if (H > 2 && H <= 3) {
                // G is max, B > R
                g = val;
                r = m;
                b = C*(H-2)+r;
            } else if (H > 3 && H <= 4) {
                // B is max, G > R
                b = val;
                r = m;
                g = r - C*(H-4);
            } else if (H > 4 && H <= 5) {
                // B is max, R > G
                b = val;
                g = m;
                r = C*(H-4) + g;
            } else {
                // R is max, B > G
                H = H - 6;
                r = val;
                g = m;
                b = g - C*H;
            }
            set_pixel(im, x, y, 0, r);
            set_pixel(im, x, y, 1, g);
            set_pixel(im, x, y, 2, b);
        }
    }
}
