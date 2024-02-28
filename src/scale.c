#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

void scale_filter(Bitmap *bmp) {
    // Reading current dimensions
    int original_width = bmp->width;
    int original_height = bmp->height;

    // Calculate new dimensions
    int new_width = original_width * bmp->scale;
    int new_height = original_height * bmp->scale;

    // Update bitmap header
    bmp->width = new_width;
    bmp->height = new_height;

    // Allocate memory for original image
    Pixel *unscaled_rows[original_height];
    for (int i = 0; i < original_height; i++) {
        unscaled_rows[i] = (Pixel *)malloc(sizeof(Pixel) * original_width);
        if (unscaled_rows[i] == NULL) {
            fprintf(stderr, "Error: Unable to allocate memory for original image.\n");
            exit(1);
        }
        // Store original pixel data into buffer
        fread(unscaled_rows[i], sizeof(Pixel), original_width, stdin);
    }

    // Allocate memory for new image
    Pixel *scaled_rows[new_height];
    for (int j = 0; j < new_height; j++) {
        scaled_rows[j] = (Pixel *)malloc(sizeof(Pixel) * new_width);
        if (scaled_rows[j] == NULL) {
            fprintf(stderr, "Error: Unable to allocate memory for new image.\n");
            exit(1);
        }
    }

    // Scale the image
    for (int y = 0; y < new_height; y++) {
        int corresponding_y = y / bmp->scale;
        for (int x = 0; x < new_width; x++) {
            int corresponding_x = x / bmp->scale;
            Pixel *pixel = unscaled_rows[corresponding_y] + (corresponding_x);
            fwrite(pixel, sizeof(Pixel), 1, stdout);
        }
    }
    
    for (int i = 0; i < original_height; i++) {
        free(unscaled_rows[i]);
    }

    for (int j = 0; j < new_height; j++) {
        free(scaled_rows[j]);
    }
}

int main(int argc, char *argv[]) {
    // Run the filter program with copy_filter to process the pixels.
    // You shouldn't need to change this implementation.
    int scale = strtol(argv[1], NULL, 10);

    run_filter(scale_filter, scale);
    return 0;
}