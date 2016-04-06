#ifndef _NUM_HELPERS_H_
#define _NUM_HELPERS_H_

// Quantizes a value that is guaranteed to lie between minValue and maxValue to
// use the desired number of bits. This is a uniform quantization
template <class T, class U>
T quantizeValue(U value, U minValue, U maxValue, int numBits);

// Quantize assuming -maxValue to maxValue. Again uniform quantization
template <class T, class U>
T quantizeValue(U value, U maxValue, int numBits);

// Delta encoding of two values
template <class T>
T deltaEncode(T value, T prevValue,
              std::function<T(const T&, const T&)> diffingFunc) {
  return diffingFunc(value, prevValue);
}

// Clamp an orderable value to lie between the min and max values
template <class T>
T clamp(T value, T minValue, T maxValue) {
  if (value < minValue)
    return minValue;
  if (value > maxValue)
    return maxValue;
  return value;
}

// Tranpose a matrix given as a plain array.
// TODO msati3: Move this to matrix class
template <class T>
void transposeMatrix(T* pMatrix, int dim) {
  for (int i = 1; i < dim; i++) {
    for (int j = 0; j < i; j++) {
      std::swap(pMatrix[i * dim + j], pMatrix[j * dim + i]);
    }
  }
}

#endif  // _NUM_HELPERS_H_
