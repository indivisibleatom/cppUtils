// Some operations performed on vectors in vector spaces
#ifndef _VECTORSPACE_H_
#define _VECTORSPACE_H_

#include <typeinfo>
#include <iterator>
#include <boost/iterator/zip_iterator.hpp>

#include "utils/collectionHelpers.h"

// Return f_1 * V + f_2 * U.
template <typename RetType, typename InputType1, typename InputType2,
          typename FieldType>
static RetType bilinearCombination(const InputType1& val1,
                                   const InputType2& val2,
                                   const FieldType& fieldCoeffs1,
                                   const FieldType& fieldCoeffs2) {
  static_assert(is_iterable<InputType1>::value &
                    is_iterable<InputType2>::value &
                    is_iterable<RetType>::value,
                "Both the input types and return type must be iterable");
  RetType retValue;
  auto iter = retValue.begin();
  std::for_each(
      boost::make_zip_iterator(boost::make_tuple(val1.cbegin(), val2.cbegin())),
      boost::make_zip_iterator(boost::make_tuple(val1.cend(), val2.cend())),
      [&iter, &fieldCoeffs1, &fieldCoeffs2](const boost::tuple<
          typename std::iterator_traits<InputType1>::value_type,
          typename std::iterator_traits<InputType2>::value_type>& tuple) {
        *iter = fieldCoeffs1* std::get<0>(tuple) +
                fieldCoeffs2 * std::get<1>(tuple);
        ++iter;
      });
  return retValue;
}

// Result sum(f_i * V_i)
template <typename RetType, typename InputType, typename FieldType>
static RetType multilinearCombination(const InputType& vals,
                                      const FieldType& fieldCoeffs) {
  static_assert(is_iterable<InputType>::value & is_iterable<RetType>::value &
                    is_iterable<FieldType>::value,
                "InputType, ReturnType and FieldType must be iterable");
  RetType retValue;
  std::for_each(
      boost::make_zip_iterator(
          boost::make_tuple(vals.cbegin(), fieldCoeffs.cbegin())),
      [&retValue](const boost::tuple<
          typename std::iterator_traits<InputType>::value_type,
          typename std::iterator_traits<FieldType>::value_type>& tuple) {
        retValue = bilinearCombination(retValue, std::get<0>(tuple), 1,
                                    std::get<2>(tuple));
      });
  return retValue;
}

//------------Specialize for fixed size containers for speed------------------
// Return f_1 * V + f_2 * U.
template <typename RetType, typename InputType1, typename InputType2,
          typename FieldType, size_t N>
static RetType bilinearCombination(const InputType1& val1,
                                   const InputType2& val2,
                                   const FieldType& fieldCoeffs1,
                                   const FieldType& fieldCoeffs2) {
  static_assert(is_iterable<InputType1>::value &
                    is_iterable<InputType2>::value &
                    is_iterable<RetType>::value,
                "Both the input types and return type must be iterable");
  RetType retValue;
  for (size_t i = 0; i < N; ++i) {
    retValue[i] = fieldCoeffs1 * val1[i] + fieldCoeffs2 * val2[i];
  }
  return retValue;
}

// Result sum(f_i * V_i)
template <typename RetType, typename InputType, typename FieldType, size_t N>
static RetType multilinearCombination(const InputType& vals,
                                      const FieldType& fieldCoeffs) {
  static_assert(is_iterable<InputType>::value & is_iterable<RetType>::value &
                    is_iterable<FieldType>::value,
                "InputType, ReturnType and FieldType must be iterable");
  RetType retValue;
  std::for_each(
      boost::make_zip_iterator(
          boost::make_tuple(vals.cbegin(), fieldCoeffs.cbegin())),
      [&retValue](const boost::tuple<
          typename std::iterator_traits<InputType>::value_type,
          typename std::iterator_traits<FieldType>::value_type>& tuple) {
        retValue = bilinearCombination<RetType, RetType, InputType, FieldType, N>(
            retValue, std::get<0>(tuple), 1, std::get<2>(tuple));
      });
  return retValue;
}

#endif  //_VECTORSPACE_H_
