#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdint.h>

#include <iostream>
#include <queue>
#include <vector>

#include "stb_image.h"
#include "stb_image_write.h"
#include "types.hpp"
#include "util.hpp"

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

ImageData toImageData(const ImageArray &imageArray) {
    const int height = imageArray.size(), width = imageArray[0].size();
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

/**
 * kMeans accepts a vector of colors and finds a suitable representation using at most k colors
 */
std::vector<Color> kMeans(const std::vector<Color> &colors, int k, int iterations) {
    const int n = colors.size();
    std::vector<int> clusterId(n, 0);               // id of ith color
    std::vector<Color> clusterColor = {{0, 0, 0}};  // color of kth cluster

    for (int it = 0; it < iterations; it++) {
        printf("it: %d colors: %d\n", it, (int)clusterColor.size());

        // choose closest cluster
        std::vector<std::vector<int>> sum(clusterColor.size(), std::vector<int>(3));  // rgb sum
        std::vector<int> count(clusterColor.size());

        // find the worst-represented cell
        int worst = -1e9;
        int worstI = -1;

        for (int i = 0; i < n; i++) {
            int id = -1;
            int dist = 1e9;

            for (int c = 0; c < clusterColor.size(); c++) {
                int curDist = colors[i].dist(clusterColor[c]);
                if (curDist < dist) {
                    dist = curDist;
                    id = c;
                }
            }
            assert(id != -1);

            // dist is the closest dist
            if (dist > worst) {
                worst = dist;
                worstI = i;
            }

            clusterId[i] = id;
            sum[id][0] += colors[i].r;
            sum[id][1] += colors[i].g;
            sum[id][2] += colors[i].b;
            count[id]++;
        }

        // update cluster
        // iterating backwards - delete count[c] = 0 as we go
        for (int c = clusterColor.size() - 1; c >= 0; c--) {
            if (count[c] == 0) {
                clusterColor.erase(clusterColor.begin() + c);
            } else {
                clusterColor[c].r = round(sum[c][0] / (double)count[c]);
                clusterColor[c].g = round(sum[c][1] / (double)count[c]);
                clusterColor[c].b = round(sum[c][2] / (double)count[c]);
            }
        }

        // If there are not enough cluster colors, add the currently worst represented color
        if (clusterColor.size() < k) {
            clusterColor.push_back(colors[worstI]);
        }

        // At random, delete the least saturated color
        if (rand() % 10 == 0 && clusterColor.size() > 1 && it != iterations - 1) {
            int lowestInd = std::min_element(clusterColor.begin(), clusterColor.end(),
                                             [](const auto &a, const auto &b) {
                                                 auto ha = getHSL(a.r, a.g, a.b);
                                                 auto hb = getHSL(b.r, b.g, b.b);
                                                 return ha[1] < hb[1];
                                             }) -
                            clusterColor.begin();
            clusterColor.erase(clusterColor.begin() + lowestInd);
        }
    }

    std::vector<Color> result(n);
    for (int i = 0; i < n; i++) {
        result[i] = clusterColor[clusterId[i]];
    }
    return result;
}

ImageArray voronoi(const ImageArray &imageArray, int k) {
    const int height = imageArray.size(), width = imageArray[0].size();

    // generate points
    std::vector<std::vector<int>> points(k);
    for (int i = 0; i < k; i++) {
        points[i] = {rand() % height, rand() % width};
    }

    // multi-source bfs
    // keeps track of the parent of each pixel
    std::vector<std::vector<int>> parent(height, std::vector<int>(width, -1));
    std::queue<std::pair<int, int>> Q;
    for (int p = 0; p < points.size(); p++) {
        const auto &point = points[p];
        Q.push({point[0], point[1]});
        parent[point[0]][point[1]] = p;
    }

    const std::vector<int> di = {0, 0, 1, -1}, dj = {-1, 1, 0, 0};
    while (Q.size()) {
        const auto at = Q.front();
        Q.pop();

        for (int d = 0; d < 4; d++) {
            const int ii = at.first + di[d], jj = at.second + dj[d];
            if (ii < 0 || ii >= height || jj < 0 || jj >= width) continue;
            if (parent[ii][jj] != -1) continue;
            parent[ii][jj] = parent[at.first][at.second];
            Q.push({ii, jj});
        }
    }

    std::vector<Color> sourceColors;
    for (int p = 0; p < points.size(); p++) {
        sourceColors.push_back(imageArray[points[p][0]][points[p][1]]);
    }

    const auto kMeansColors = kMeans(sourceColors, 20, 1000);
    auto result = imageArray;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            result[i][j] = kMeansColors[parent[i][j]];
        }
    }
    return result;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Invalid arguments. Usage: ./ImageProcessing [path/to/input] [path/to/output]\n");
        return 1;
    }

    const char *sourcePath = argv[1];
    const char *targetPath = argv[2];

    srand(time(NULL));

    // Reading image
    printf("Reading image...\n");
    int width, height, channels;
    ImageData inputImageData = stbi_load(sourcePath, &width, &height, &channels, CHANNELS);
    if (!inputImageData) {
        throw std::invalid_argument("Error loading image");
    }
    assert(channels == CHANNELS);
    auto inputImageArray = toImageArray(inputImageData, width, height);
    stbi_image_free(inputImageData);

    printf("Processing image...\n");
    int numPoints = 10000;
    auto outputImageArray = voronoi(inputImageArray, numPoints);

    // Writing image
    printf("Writing image...\n");
    auto outputImageData = toImageData(outputImageArray);
    stbi_write_png(targetPath, width, height, CHANNELS, outputImageData, width * CHANNELS);
    free(outputImageData);
    return 0;
}