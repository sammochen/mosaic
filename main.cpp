#include <stdint.h>

#include <iostream>
#include <queue>
#include <vector>

#include "color.hpp"
#include "image.hpp"
#include "io.hpp"

const std::vector<int> di = {0, 0, 1, -1}, dj = {-1, 1, 0, 0};

/**
 * Accepts a vector of colors and finds a suitable representation using at most numColors colors
 */
std::vector<Color> kMeans(const std::vector<Color> &colors, int numColors) {
    const int maxIterations = 1000;

    const int n = colors.size();
    std::vector<int> clusterId(n, 0);               // id of ith color
    std::vector<Color> clusterColor = {{0, 0, 0}};  // color of kth cluster

    for (int it = 0; it < maxIterations; it++) {
        const std::vector<Color> prevColor = clusterColor;

        // Tracks the average color of each cluster
        std::vector<std::vector<int>> sum(clusterColor.size(), std::vector<int>(3));  // rgb sum
        std::vector<int> count(clusterColor.size());

        // Tracks the worst-represented cell
        int worstDist = -1e9;
        int worstInd = -1;

        for (int i = 0; i < n; i++) {
            // Assign each cell to the closest cluster
            int closestInd = -1;
            int closestDist = 1e9;

            for (int c = 0; c < clusterColor.size(); c++) {
                int dist = colors[i].dist(clusterColor[c]);
                if (dist < closestDist) {
                    closestDist = dist;
                    closestInd = c;
                }
            }

            clusterId[i] = closestInd;
            sum[closestInd][0] += colors[i].r;
            sum[closestInd][1] += colors[i].g;
            sum[closestInd][2] += colors[i].b;
            count[closestInd]++;

            // Update the worst cell
            if (closestDist > worstDist) {
                worstDist = closestDist;
                worstInd = i;
            }
        }

        // Update cluster colors
        for (int c = clusterColor.size() - 1; c >= 0; c--) {
            if (count[c] * 1000 <= n) {
                // Delete colors that are <0.1%
                clusterColor.erase(clusterColor.begin() + c);
            } else {
                // Update cluster color to be the average of the cells in the cluster
                clusterColor[c].r = round(sum[c][0] / (double)count[c]);
                clusterColor[c].g = round(sum[c][1] / (double)count[c]);
                clusterColor[c].b = round(sum[c][2] / (double)count[c]);
            }
        }

        // Add the worst represented color
        if (clusterColor.size() < numColors) {
            clusterColor.push_back(colors[worstInd]);
        }

        // Convergence
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

/**
 * Given the width and height, returns a 2D parent matrix of which "space" each cell is in
 */
std::vector<std::vector<int>> voronoi(const int width, const int height,
                                      const std::vector<std::vector<int>> &points) {
    // Multi-source BFS
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

/**
 * Given an image array, splits the matrix into numSpaces cells and then performs k-means on the
 * colors of the cell.
 * Returns the resulting mosaic image.
 */
ImageArray mosaic(const ImageArray &imageArray, const int numSpaces, const int numColors) {
    const int height = imageArray.size(), width = imageArray[0].size();

    // Generate random points
    std::vector<std::vector<int>> points(numSpaces);
    for (int i = 0; i < numSpaces; i++) {
        points[i] = {rand() % height, rand() % width};
    }

    const auto parent = voronoi(width, height, points);

    // Retrieve the original colors of the spaces
    std::vector<Color> sourceColors;
    for (int p = 0; p < points.size(); p++) {
        sourceColors.push_back(imageArray[points[p][0]][points[p][1]]);
    }

    // Find the new colors
    const auto kMeansColors = kMeans(sourceColors, numColors);

    // Create the new image
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
