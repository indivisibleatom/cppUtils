// A generic n-dimensional Point class and its specializations
#ifndef _POINT_H_
#define _POINT_H_

#include <cmath>
#include <ostream>
#include <algorithm>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

#include "mathUtils/vectorSpace.h"

template <typename T, size_t N>
class Vector;

template <class T, size_t N>
class Point {
 protected:
  std::array<T, N> m_x;

 private:
  void swap(Point& other) { std::swap(m_x, other.m_x); }

 public:
  Point() : m_x() {}
  Point(Point&& other) : m_x(std::move(other.m_x)) {}
  Point(const std::array<T, N>& x) : m_x(x) {}
  Point(const Point& other) : m_x(other.m_x) {}

  Point& operator=(Point other) {
    swap(other);
    return *this;
  }

  Point(const Point& p1, const Point& p2) {
    *this = bilinearCombination<Point, Point, Point, N>(p1, p2, 0.5, 0.5);
  }

  Point(const std::vector<Point<T, N>>& points) {
    T denominator = 1.0 / points.size();
    std::array<T, N> denominators;
    std::fill_n(denominators.begin(), denominators.size(), denominator);
    *this =
        multilinearCombination<Point, std::vector<Point>, std::array<T, N>, N>(
            points, denominators);
  }

  Point(const Point& point, T s, const Vector<T>& vector) {
    swap(bilinearCombination(point, vector, 1, s));
  }

  // Point + s ( other - point)
  Point(const Point& point, float s, const Point& other) throw() {
    Vector<T> vector(point, other);
    Point temp(point, s, vector);
    swap(temp);
  }

  // Point + sum(s[i] * v[i])
  /*Point(const Point& point, std::vector<T> s, const std::vector<Vector<T>>& v)
  {
    Vector<T> resVector = multilinearCombination<T>(v, s);
    swap(bilinearCombination(point, resVector, 1, 1));
  }*/

  // Iterator interface for point dimensions
  template <typename BaseIterType>
  class dimension_iterator
      : public boost::iterator_adaptor<dimension_iterator<BaseIterType>,
                                       BaseIterType> {
   public:
    dimension_iterator() : dimension_iterator::iterator_adaptor_() {}

    explicit dimension_iterator(const BaseIterType& iter)
        : dimension_iterator::iterator_adaptor_(iter) {}
  };

  // Advertise self as container
  typedef dimension_iterator<typename std::array<T, N>::iterator> iterator;
  typedef dimension_iterator<typename std::array<T, N>::const_iterator>
      const_iterator;
  typedef T value_type;

  iterator begin() { return iterator(m_x.begin()); }
  iterator end() { return iterator(m_x.end()); }
  const_iterator cbegin() const { return const_iterator(m_x.cbegin()); }
  const_iterator cend() const { return const_iterator(m_x.cend()); }
  const T& operator[](size_t xi) const { return m_x[xi]; }
  T& operator[](size_t xi) { return m_x[xi]; }

  Point& set(std::array<T,N>& x) {
    m_x = x;
    return *this;
  }

  Point& set(const Point& other) {
    m_x = other.m_x;
    return *this;
  }

  T distance2(const Point& other) const {
    auto iterBegin =
        boost::make_zip_iterator(boost::make_tuple(cbegin(), other.cbegin()));
    auto iterEnd =
        boost::make_zip_iterator(boost::make_tuple(cend(), other.cend()));
    T dist2 = std::accumulate(
        iterBegin, iterEnd, static_cast<T>(0),
        [](const boost::tuple<
            typename std::iterator_traits<const_iterator>::value_type,
            typename std::iterator_traits<const_iterator>::value_type>& tuple) {
          return (boost::get<0>(tuple) - boost::get<1>(tuple)) *
                 (boost::get<0>(tuple) - boost::get<1>(tuple));
        });
    return dist2;
  }

  T distance(const Point& other) const throw() {
    return sqrt(distance2(other));
  }

  /*template <typename V, size_t N>
  friend Vector<V,N> operator-(const Point<V>& first, const Point<V>& second);*/

  template <class V, size_t M>
  friend std::ostream& operator<<(std::ostream& os, const Point<V, M>& point);
};

/*template <class T>
Vector<T> operator-(const Point<T>& first, const Point<T>& second) {
  return first.diff(second);
}*/

template <class T, size_t N>
std::ostream& operator<<(std::ostream& os, const Point<T, N>& point) {
  for (auto x : point) {
    os << x << " ";
  }
  return os;
}

//----------------------------------Specializations--------------------------------

//----------------------------------Point3D----------------------------------------
/*template <class T>
class Point3D : public Point<T, 3> {
 public:
  // Rotated point by 'a' around other in plane (I,J)
  Point3D(const Point3D& point, float a, const Vector<T>& I, const Vector<T>& J,
          const Point3D& other) {
    float x = Vector<T>(other, point).dot(I);
    float y = Vector<T>(other, point).dot(J);
    float c = cos(a);
    float s = sin(a);
    *this = Point(point, x * c - x - y * s, I, x * s + y * c - y, J);
  }

  T x() const throw() { return m_x[0]; }
  T y() const throw() { return m_x[1]; }
  T z() const throw() { return m_x[2]; }
};*/

#endif  //_POINT_H_
