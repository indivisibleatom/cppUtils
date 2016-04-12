// Some operations performed on vectors in vector spaces
#ifndef _VECTORSPACE_H_
#define _VECTORSPACE_H_

#include <typeinfo>
#include <iterator>
#include <boost/iterator/zip_iterator.hpp>

#include "utils/collectionHelpers.h"

// Return f_1 * V + f_2 * U.
template <typename RetType, typename InputType1, typename InputType2>
static RetType bilinearCombination(
    const InputType1& val1, const InputType2& val2,
    typename InputType1::value_type fieldCoeff1,
    typename InputType2::value_type fieldCoeff2) {
  static_assert(is_iterable<InputType1>::value &
                    is_iterable<InputType2>::value &
                    is_iterable<RetType>::value,
                "Both the input types and return type must be iterable");
  RetType retValue;
  auto iter = retValue.begin();
  std::for_each(
      boost::make_zip_iterator(boost::make_tuple(val1.cbegin(), val2.cbegin())),
      boost::make_zip_iterator(boost::make_tuple(val1.cend(), val2.cend())),
      [&iter, &fieldCoeff1, &fieldCoeff2](
          const boost::tuple<typename std::iterator_traits<
                                 typename InputType1::iterator>::value_type,
                             typename std::iterator_traits<
                                 typename InputType2::iterator>::value_type>&
              tuple) {
        *iter = fieldCoeff1* boost::get<0>(tuple) +
                fieldCoeff2 * boost::get<1>(tuple);
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
  static_assert(std::is_same<typename FieldType::value_type,
                             typename InputType::value_type::value_type>::value,
                "InputType and FieldType must contain the same types");
  RetType retValue;
  std::for_each(boost::make_zip_iterator(
                    boost::make_tuple(vals.cbegin(), fieldCoeffs.cbegin())),
                boost::make_zip_iterator(
                    boost::make_tuple(vals.cend(), fieldCoeffs.cend())),
                [&retValue](const boost::tuple<
                    typename std::iterator_traits<
                        typename InputType::iterator>::value_type,
                    typename std::iterator_traits<
                        typename FieldType::iterator>::value_type>& tuple) {
                  retValue = bilinearCombination(retValue, boost::get<0>(tuple),
                                                 1, boost::get<2>(tuple));
                });
  return retValue;
}

//------------Specialize for fixed size containers for speed------------------
// Return f_1 * V + f_2 * U.
template <typename RetType, typename InputType1, typename InputType2, size_t N>
static RetType bilinearCombination(
    const InputType1& val1, const InputType2& val2,
    typename InputType1::value_type fieldCoeff1,
    typename InputType1::value_type fieldCoeff2) {
  static_assert(is_iterable<InputType1>::value &
                    is_iterable<InputType2>::value &
                    is_iterable<RetType>::value,
                "Both the input types and return type must be iterable");
  RetType retValue;
  for (size_t i = 0; i < N; ++i) {
    retValue[i] = fieldCoeff1 * val1[i] + fieldCoeff2 * val2[i];
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
  static_assert(
      boost::is_same<typename FieldType::value_type,
                     typename InputType::value_type::value_type>::value,
      "InputType and FieldType must contain the same types");
  RetType retValue;
  std::for_each(
      boost::make_zip_iterator(
          boost::make_tuple(vals.cbegin(), fieldCoeffs.cbegin())),
      boost::make_zip_iterator(
          boost::make_tuple(vals.cend(), fieldCoeffs.cend())),
      [&retValue](const boost::tuple<
          typename std::iterator_traits<
              typename InputType::const_iterator>::value_type,
          typename std::iterator_traits<
              typename FieldType::const_iterator>::value_type>& tuple) {
        retValue = bilinearCombination<
            RetType, RetType,
            typename std::iterator_traits<
                typename InputType::const_iterator>::value_type,
            N>(retValue, boost::get<0>(tuple), 1, boost::get<1>(tuple));
      });
  return retValue;
}

#endif  //_VECTORSPACE_H_
