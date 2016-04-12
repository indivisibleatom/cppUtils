// A generic n-dimensional Point class and its specializations
#ifndef _POINT_H_
#define _POINT_H_

#include <cmath>
#include <ostream>
#include <algorithm>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

#include "mathUtils/vectorSpace.h"

// template <typename T>
// class Vector;

template <class T, size_t N>
class Point {
 protected:
  std::array<T, N> m_x;

  void swap(Point& other) { std::swap(m_x, other.m_x); }

 public:
  Point() {}

  Point(const std::array<T, N>& x) : m_x(x) {}

  Point(const Point& other) : m_x(other.m_x) {}

  Point& operator=(Point other) {
    swap(other);
    return *this;
  }

  Point(const Point& p1, const Point& p2) {
    *this = bilinearCombination<Point, Point, Point, T>(p1, p2, 0.5, 0.5);
  }

  Point(const std::vector<Point<T, N>>& points) {
    T denominator = 1.0 / points.size();
    *this = multilinearCombination<Point, Point, T>(
        points, std::array<T, N>{denominator});
  }

  /*Point(const Point& point, T s, const Vector<T>& vector) {
    swap(bilinearCombination(point, vector, 1, s));
  }*/

  // Point + s ( other - point)
  /*Point(const Point& point, float s, const Point& other) throw() {
    Vector<T> vector(point, other);
    Point temp(point, s, vector);
    swap(temp);
  }*/

  // Point + sum(s[i] * v[i])
  /*Point(const Point& point, std::vector<T> s, const std::vector<Vector<T>>& v)
  {
    Vector<T> resVector = multilinearCombination<T>(v, s);
    swap(bilinearCombination(point, resVector, 1, 1));
  }*/

  Point diff(const Point& other) {
    return (bilinearCombination<Point, Point, Point, T>(this, other, 1, -1));
  }

  Point& set(std::vector<T>& x) {
    m_x = x;
    return *this;
  }

  Point& set(const Point& other) throw() {
    m_x = other.m_x;
    return *this;
  }

  /*T distance2(const Point& other) const throw() {
    T value = 0;
    return ((m_x - other.m_x) * (m_x - other.m_x) +
            (m_y - other.m_y) * (m_y - other.m_y) +
            (m_z - other.m_z) * (m_z - other.m_z));
  }

  T distance(const Point& other) const throw() {
    return sqrt(distance2(other));
  }*/

  /*template <typename V, size_t N>
  friend Point<V> operator-(const Point<V>& first, const Point<V>& second);

  template <class V, size_t V>
  friend std::ostream& operator<<(std::ostream& os, const Point<T>& point);*/
};

/*template <class T>
Point<T> operator-(const Point<T>& first, const Point<T>& second) {
  return first.diff(second);
}

template <class T>
std::ostream& operator<<(std::ostream& os, const Point<T>& point) {
  for (auto x : point) {
    os << x << " ";
  }
  return os;
}*/

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
