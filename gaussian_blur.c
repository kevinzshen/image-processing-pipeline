#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

void gaussianBlur_filter(Bitmap *bmp) {
    // Pixel *rows[3] = {malloc(sizeof(Pixel) * bmp->width), malloc(sizeof(Pixel) * bmp->width), malloc(sizeof(Pixel) * bmp->width)};
    Pixel *rows[3];
    int read = 1;

    for (int i = 0; i < 3; i++){
        rows[i] = (Pixel *)malloc(sizeof(Pixel) * bmp->width);
    }

    // Read the first two rows into the buffer
    fread(rows[0], sizeof(Pixel), bmp->width, stdin); // Bottom row
    fread(rows[1], sizeof(Pixel), bmp->width, stdin); // Middle row

    // Iterate through rows
    for (int y = 0; y < bmp->height; y++) {
        // If not currently on last row (top edge), read third row into buffer
        if (y < (bmp->height - 1) && read == 1) {
            fread(rows[2], sizeof(Pixel), bmp->width, stdin); // Top row
        }

        Pixel *top_row = rows[2];
        Pixel *middle_row = rows[1];
        Pixel *bottom_row = rows[0];

        // Begin iterating through pixels
        for (int x = 0; x < bmp->width; x++) {
            int x_left;

            // Set pointers to the first pixel in each of row of the 3x3 grid of interest
            if (x == 0) {
                x_left = 0;
            }
            // If current pixel is last pixel of a particular row
            else if (x == bmp->width - 1) {
                x_left = x - 2;
            }
            // Base case
            else {
                x_left = x - 1;
            }
            
            Pixel pixel = apply_gaussian_kernel(bottom_row + x_left , middle_row + x_left, top_row + x_left);
            
            fwrite(&pixel, sizeof(Pixel), 1, stdout);
        }

        // Shift each row up one level
        if (y != 0 && y != (bmp->height - 2) && y != (bmp->height - 1)) {
            Pixel *temp = rows[0];
            rows[0] = rows[1];
            rows[1] = rows[2];
            rows[2] = temp;
            read = 1;
        }
        else {
            read = 0;
        }
    }

    // Free the allocated rows
    for (int i = 0; i < 3; ++i) {
        free(rows[i]);
    }
}

int main() {
    // Run the filter program with copy_filter to process the pixels.
    // You shouldn't need to change this implementation.
    run_filter(gaussianBlur_filter, 1);
    return 0;
}