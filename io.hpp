#ifndef IO_HPP

#define IO_HPP

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdint.h>

#include <cassert>
#include <stdexcept>
#include <vector>

#include "color.hpp"
#include "image.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

const int CHANNELS = 3;

ImageArray toImageArray(const ImageData &imageData, const int width, const int height) {
    std::vector<std::vector<Color>> imageArray(height, std::vector<Color>(width));
    int ind = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            imageArray[i][j].r = imageData[ind++];
            imageArray[i][j].g = imageData[ind++];
            imageArray[i][j].b = imageData[ind++];
        }
    }
    return imageArray;
}

ImageData toImageData(const ImageArray &imageArray, int &width, int &height) {
    height = imageArray.size();
    width = imageArray[0].size();
    uint8_t *imageData = (uint8_t *)malloc(height * width * CHANNELS * sizeof(uint8_t));

    int ind = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            imageData[ind++] = imageArray[i][j].r;
            imageData[ind++] = imageArray[i][j].g;
            imageData[ind++] = imageArray[i][j].b;
        }
    }
    return imageData;
}

ImageArray readImage(const char *filename) {
    int width, height, channels;
    ImageData imageData = stbi_load(filename, &width, &height, &channels, CHANNELS);
    if (!imageData) {
        throw std::invalid_argument("Error loading image");
    }
    assert(channels == CHANNELS);
    auto imageArray = toImageArray(imageData, width, height);
    stbi_image_free(imageData);

    return imageArray;
}

void writeImage(const char *filename, const ImageArray &imageArray) {
    int width = -1, height = -1;
    auto outputImageData = toImageData(imageArray, width, height);
    stbi_write_png(filename, width, height, CHANNELS, outputImageData, width * CHANNELS);
    free(outputImageData);
}

#endif
