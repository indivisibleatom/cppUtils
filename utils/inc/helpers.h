#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <cassert>
#include <map>
#include <thread>
#include <chrono>
#include <algorithm>

const float PI = 3.14159265359f;

template <class T>
T strtoT(const char* str, char** endPgettr);

std::vector<std::string>& split(const std::string& s, char delim,
                                std::vector<std::string>& elems);

template <class T, class U>
T quantizeValue(U value, U minValue, U maxValue, int numBits);

template <class T, class U>
T quantizeValue(U value, U minValue, int numBits);

template <class T>
T deltaEncode(T value, T prevValue,
              std::function<T(const T&, const T&)> diffingFunc) {
  return diffingFunc(value, prevValue);
}

template <class T>
T clamp(T value, T minValue, T maxValue) {
  if (value < minValue)
    return minValue;
  if (value > maxValue)
    return maxValue;
  return value;
}

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

template <class T>
void cyclicallyPermute(T* pData, std::size_t size, int leftShiftAmount) {
  if (leftShiftAmount == 0) {
    return;
  }
  int nextIndex = (size - leftShiftAmount) % size;
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

template <class T>
std::map<T, int> computeHistogram(const std::vector<T>& values) {
  std::map<T, int> retVal;
  std::for_each(values.begin(), values.end(),
                [&retVal](T value) { retVal[value]++; });
  return retVal;
}

template <class T>
void transposeMatrix(T* pMatrix, int dim) {
  for (int i = 1; i < dim; i++) {
    for (int j = 0; j < i; j++) {
      std::swap(pMatrix[i * dim + j], pMatrix[j * dim + i]);
    }
  }
}

bool replace(std::string& str, const std::string& from, const std::string& to);

template <typename Function>
void callAfter(long millis, Function const& function) {
  std::thread([millis, function]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    function();
  }).detach();
}

#endif  //_HELPERS_H_
