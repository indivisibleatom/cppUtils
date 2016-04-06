#ifndef _COLLECTION_HELPERS_H_
#define _COLLECTION_HELPERS_H_

#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

// Cyclically permutes towards the left a data of type that is indexable
// TODO msati3: Is this the best. Enforce the fact that T is indexable
template <class T>
void cyclicallyPermute(T* pData, std::size_t size, int leftShiftAmount) {
  if (leftShiftAmount == 0) {
    return
  }
  int tIndex = (size - leftShiftAmount) % size;
  int currentIndex = 0;

  T prevData = pData[currentIndex];
  T tempData;

  for (size_t i = 0; i < size; i++) {
    tempData = pData[nextIndex];
    pData[nextIndex] = prevData;

    prevData = tempData;

    currentIndex = nextIndex;
    nextIndex = (nextIndex - leftShiftAmount + size) % size;
  }
}

// Given an input map from index to value (a vector), populate an output map
// from value to index.
template <class T, class U>
void populateReverseMap(std::vector<U>& outputMap,
                        const std::vector<T>& inputMap, U defaultOutputValue,
                        T defaultInputValue, std::size_t sizeOutputMap) {
  outputMap.assign(sizeOutputMap, defaultOutputValue);
  U currentCount = U(0);

  std::for_each(inputMap.cbegin(), inputMap.cend(),
                [&outputMap, &currentCount, &sizeOutputMap, &defaultInputValue,
                 &defaultOutputValue](const T& inputMapElement) {
                  if (inputMapElement != defaultInputValue) {
                    assert(inputMapElement <= T(sizeOutputMap));
                    if (outputMap[inputMapElement] == defaultOutputValue) {
                      outputMap[inputMapElement] = currentCount;
                    }
                  }
                  currentCount++;
                });
}

//Populate a histogram of counts of each value in a vector
template <class T>
std::map<T, int> computeHistogram(const std::vector<T>& values) {
  std::map<T, int> retVal;
  std::for_each(values.begin(), values.end(),
                [&retVal](T value) { retVal[value]++; });
  return retVal;
}

#endif  //_COLLECTION_HELPERS_H_
