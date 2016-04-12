// Some operations performed on vectors in vector spaces
#ifndef _VECTORSPACE_H_
#define _VECTORSPACE_H_

#include <typeinfo>
#include <boost/iterator/zip_iterator.hpp>

#include "utils/collectionHelpers.h"

// Return s1V + s2U.
template <bool b>
struct bilinearCombination_Selector {
  template <typename RetType, typename InputType1, typename InputType2,
            typename FieldType>
  static RetType bilinearCombination(const InputType1& val1,
                                     const InputType2& val2,
                                     const FieldType& fieldCoeffs1,
                                     FieldType& fieldCoeffs2);
};

template <>
struct bilinearCombination_Selector<true> {
  template <typename RetType, typename InputType1, typename InputType2,
            typename FieldType>
  static RetType bilinearCombination(const InputType1& val1,
                                     const InputType2& val2,
                                     const FieldType& fieldCoeffs1,
                                     const FieldType& fieldCoeffs2) {
    RetType retValue;
    auto iter = retValue.begin();
    for (auto tuple : zip(val1, val2)) {
      *iter =
          fieldCoeffs1* std::get<0>(tuple) + fieldCoeffs2 * std::get<1>(tuple);
      ++iter;
    }
  }
};

// Return s1V1 + s2V2 + ...
template <bool b>
struct multilinearCombination_Selector {
  template <typename RetType, typename InputType1, typename InputType2,
            typename FieldType>
  static RetType multilinearCombination(const InputType1& val1,
                                        const InputType2& val2,
                                        const FieldType& fieldCoeffs1,
                                        FieldType& fieldCoeffs2);
};

template <>
struct multilinearCombination_Selector<true> {
  template <typename RetType, typename InputType1, typename InputType2,
            typename FieldType>
  static RetType multilinearCombination(const InputType1& vals,
                                        const FieldType& fieldCoeffs) {
    RetType retValue;
    auto iter = retValue.begin();
    for (auto tuple : zip(vals, fieldCoeffs)) {
      *iter = bilinearCombination(retValue, std::get<0>(tuple), 1,
                                  std::get<2>(tuple));
      ++iter;
    }
  }
};

template <typename RetType, typename InputType1, typename InputType2,
          typename FieldType>
RetType bilinearCombination(const InputType1& val1, const InputType2& val2,
                            const FieldType& fieldCoeffs1,
                            const FieldType& fieldCoeffs2) {
  bilinearCombination_Selector<
      is_iterable<InputType1>::value & is_iterable<InputType2>::value &
      is_iterable<RetType>::value>::bilinearCombination(val1, val2,
                                                        fieldCoeffs1,
                                                        fieldCoeffs2);
}

template <typename RetType, typename InputType, typename FieldType>
RetType multilinearCombination(const InputType& vals,
                               const FieldType& fieldCoeffs) {
  multilinearCombination_Selector<
      is_iterable<InputType>::value & is_iterable<RetType>::value &
      is_iterable<FieldType>::value>::multilinearCombination(vals, fieldCoeffs);
}

#endif  //_VECTORSPACE_H_
