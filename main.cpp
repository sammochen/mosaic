#include <stdint.h>

#include <iostream>
#include <queue>
#include <vector>

#include "color.hpp"
#include "image.hpp"
#include "io.hpp"

const std::vector<int> di = {0, 0, 1, -1}, dj = {-1, 1, 0, 0};

/**
 * kMeans accepts a vector of colors and finds a suitable representation using at most k colors
 */
std::vector<Color> kMeans(const std::vector<Color> &colors, int numColors, int iterations) {
    const int n = colors.size();
    std::vector<int> clusterId(n, 0);               // id of ith color
    std::vector<Color> clusterColor = {{0, 0, 0}};  // color of kth cluster

    for (int it = 0; it < iterations; it++) {
        std::vector<Color> prevColor = clusterColor;

        // choose closest cluster
        std::vector<std::vector<int>> sum(clusterColor.size(), std::vector<int>(3));  // rgb sum
        std::vector<int> count(clusterColor.size());

        // find the worst-represented cell
        int worstDist = -1e9;
        int worstInd = -1;

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
            if (dist > worstDist) {
                worstDist = dist;
                worstInd = i;
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
            // Delete colors that are <0.1%
            if (count[c] * 1000 <= n) {
                clusterColor.erase(clusterColor.begin() + c);
            } else {
                clusterColor[c].r = round(sum[c][0] / (double)count[c]);
                clusterColor[c].g = round(sum[c][1] / (double)count[c]);
                clusterColor[c].b = round(sum[c][2] / (double)count[c]);
            }
        }

        // if there are not enough cluster colors, add the worst represented color
        if (clusterColor.size() < numColors) {
            clusterColor.push_back(colors[worstInd]);
        }

        if (prevColor == clusterColor) {
            printf("Converged at iteration %d\n", it);
            break;
        }
    }

    std::vector<Color> result(n);
    for (int i = 0; i < n; i++) {
        result[i] = clusterColor[clusterId[i]];
    }
    return result;
}

std::vector<std::vector<int>> voronoi(const ImageArray &imageArray,
                                      const std::vector<std::vector<int>> &points) {
    const int height = imageArray.size(), width = imageArray[0].size();

    // multi-source bfs
    // keeps track of the parent of each pixel
    std::vector<std::vector<int>> parent(height, std::vector<int>(width, -1));
    std::queue<std::pair<int, int>> Q;
    for (int p = 0; p < points.size(); p++) {
        const auto &point = points[p];
        Q.push({point[0], point[1]});
        parent[point[0]][point[1]] = p;
    }

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
    return parent;
}

ImageArray mosaic(const ImageArray &imageArray, const int numSpaces, const int numColors) {
    const int height = imageArray.size(), width = imageArray[0].size();

    // generate points
    std::vector<std::vector<int>> points(numSpaces);
    for (int i = 0; i < numSpaces; i++) {
        points[i] = {rand() % height, rand() % width};
    }

    const auto parent = voronoi(imageArray, points);

    std::vector<Color> sourceColors;
    for (int p = 0; p < points.size(); p++) {
        sourceColors.push_back(imageArray[points[p][0]][points[p][1]]);
    }

    const auto kMeansColors = kMeans(sourceColors, numColors, 1000);
    auto result = std::vector<std::vector<Color>>(height, std::vector<Color>(width));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            result[i][j] = kMeansColors[parent[i][j]];
        }
    }

    return result;
}

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("Invalid arguments. Usage: ./mosaic input.xxx output.xxx numSpaces numColors\n");
        return 1;
    }

    const char *sourcePath = argv[1];
    const char *targetPath = argv[2];
    const int numSpaces = atoi(argv[3]);
    const int numColors = atoi(argv[4]);

    srand(100);  // srand(time(NULL)); for random

    printf("Reading image...\n");
    const auto inputImageArray = readImage(sourcePath);

    printf("Processing image...\n");
    auto outputImageArray = mosaic(inputImageArray, numSpaces, numColors);

    printf("Writing image...\n");
    writeImage(targetPath, outputImageArray);
    return 0;
}
