#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdint.h>

#include <iostream>
#include <vector>

#include "stb_image.h"
#include "stb_image_write.h"

using VI8 = std::vector<uint8_t>;
using VVI8 = std::vector<VI8>;
using VVVI8 = std::vector<VVI8>;
using ImageArray = VVVI8;
using ImageData = uint8_t *;

VVVI8 toImageArray(ImageData imageData, int width, int height, int channels) {
    VVVI8 imageArray(height, VVI8(width, VI8(channels)));
    int ind = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < channels; k++) {
                imageArray[i][j][k] = imageData[ind++];
            }
        }
    }
    return imageArray;
}

ImageData toImageData(ImageArray &imageArray) {
    const int height = imageArray.size(), width = imageArray[0].size(),
              channels = imageArray[0][0].size();

    uint8_t *imageData = (uint8_t *)malloc(height * width * channels * sizeof(uint8_t));

    int ind = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < channels; k++) {
                imageData[ind++] = imageArray[i][j][k];
            }
        }
    }
    return imageData;
}

int main() {
    // Reading image
    printf("Reading image...\n");
    int width, height, channels;
    ImageData inputImageData = stbi_load("img/ball.jpg", &width, &height, &channels, 3);
    auto imageArray = toImageArray(inputImageData, width, height, channels);
    stbi_image_free(inputImageData);

    // process image
    int ind = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < channels; k++) {
                if (k == 1) imageArray[i][j][k] = 100;
            }
        }
    }

    // Writing image
    printf("Writing image...\n");
    auto outputImageData = toImageData(imageArray);
    stbi_write_png("output.jpg", width, height, 3, outputImageData, width * 3);
    free(outputImageData);
    return 0;
}