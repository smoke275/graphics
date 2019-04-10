/*
 * COMP 597 Computer Graphics
 * Assignment 1
 * Shashwata Mandal
 * sum675
 */
#include <bits/stdc++.h>
#define SUCCESS 0
//#define FACTOR 1
/*
 * FACTOR is used to control the size of the boxes in the pattern. This is to answer the add-on
 * question on what we can do to control the size of the pattern. When
 *
 * FACTOR = 1 the size of the boxes is the regular size
 *
 * FACTOR > 1 the size of the boxes increases by the value of the factor.
 */
using namespace std;

struct RGB{
    float r;
    float g;
    float b;
};

int main(int argc, char **argv) {

    int w = atoi(argv[1]); // take the input width from command line and convert it to int
    int h = atoi(argv[2]); // take the input height from command line and convert it to int

    RGB **raster =  new RGB*[h];
    for(int i = 0; i < h; i++){
        raster[i] = new RGB[w];
    }

    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            //int col = (((i/FACTOR) & 0x08)==0) ^ (((j/FACTOR) & 0x08) == 0);
            int col = ((i & 0x08)==0) ^ ((j & 0x08) == 0);
            /*
             * The color computation of the raster is done by checking if the 4th bit of the current value
             * of i and j for the pixel (i,j) is 0 or not. If the 4th bit is 0 for either i or j then the value of col
             * becomes 1. However if both of them contain a 0 in the 4th bit or neither of them contain a 0 in the 4th
             * bit, then the value of col becomes 0.
             *
             * If col is 1 then r becomes 1.0, g becomes 0.0 and b becomes 1.0.
             * In terms of pixel value when max intensity is 255, (r,g,b) becomes (255,0,255) which is magenta
             * If col is 0 then r becomes 0.0, g becomes 0.0 and b becomes 0.0.
             * In terms of pixel value when max intensity is 255, (r,g,b) becomes (0,0,0) which is magenta
             *
             */

            raster[i][j].r = static_cast<float>(col);
            raster[i][j].g = static_cast<float>(col & 0x00);
            raster[i][j].b = static_cast<float>(col & 0x11);
        }
    }

    FILE *fp = fopen(argv[3], "wb"); /* The filename mentioned in the commandline */

    fprintf(fp, "P3\n%d %d\n255\n", w, h);// Set the magic number, width, height and the max intensity

    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            static unsigned char color[3];
            color[0] = (char)(raster[i][j].r*255);  /* red */
            color[1] = (char)(raster[i][j].g*255);  /* green */
            color[2] = (char)(raster[i][j].b*255);  /* blue */
            fprintf(fp, "%d %d %d ", color[0], color[1], color[2]);
        }

        fprintf(fp, "\n");
    }
    fclose(fp);


    return SUCCESS;
}

