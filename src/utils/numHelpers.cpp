#include <vector>
#include <sstream>
#include <cstdlib>
#include <algorithm>

#include "numHelpers.h"

template <>
int quantizeValue<int, float>(float value, float minValue, float maxValue,
                              int numBits) {
  float normalized = (value - minValue) / (maxValue - minValue);
  int scaleNumber = 1 << numBits;
  int quantized = scaleNumber * normalized;
  return clamp(quantized, 0, 1 << numBits);
}

template <>
int quantizeValue<int, float>(float value, float maxValue, int numBits) {
  return quantizeValue<int, float>(value, -maxValue, maxValue, numBits);
}
