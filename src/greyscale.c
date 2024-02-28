#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

void greyscale_filter(Bitmap *bmp) {
    Pixel *pixel = malloc(sizeof(Pixel));
            if (pixel == NULL) {
                perror("Failed to allocate memory for Pixel");
            }

    Pixel *pixel2 = malloc(sizeof(Pixel));
            if (pixel2 == NULL) {
                perror("Failed to allocate memory for Pixel");
            }
    
    // Begin iterating through the image pixels of the bitmap file
    for (int i = 0; i < bmp->height; i++){
        for (int j = 0; j < bmp->width; j++){
            // Read pixel data
            fread(pixel2, sizeof(Pixel), 1, stdin);

            // Modify RGB values of the pixel using greyscale calculation
            pixel->blue = (pixel2->blue + pixel2->green + pixel2->red) / 3;
            pixel->green = (pixel2->blue + pixel2->green + pixel2->red) / 3;
            pixel->red = (pixel2->blue + pixel2->green + pixel2->red) / 3;

            // Write the modified pixel data to stdout
            fwrite(pixel, sizeof(Pixel), 1, stdout);    
        }
    }
    free(pixel);
}

int main() {
    // Run the filter program with copy_filter to process the pixels.
    // You shouldn't need to change this implementation.
    run_filter(greyscale_filter, 1);
    return 0;
}