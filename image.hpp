#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <stdint.h>

#include <vector>

#include "color.hpp"

using ImageArray = std::vector<std::vector<Color>>;
using ImageData = uint8_t *;

#endif
