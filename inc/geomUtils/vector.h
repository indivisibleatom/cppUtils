#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cmath>

#include "mathUtils/vectorSpace.h"

template <class T, size_t N>
class Point;

template <class T, size_t N>
class Vector {
 protected:
  std::array<T, N> m_x;

 private:
  void swap(Point& other) { std::swap(m_x, other.m_x); }

 public:
  Vector() : m_x() {}
  Vector(Vector&& other) : m_x(std::move(other.m_x)) {}
  Vector(const Vector& other) : m_x(other.m_x) {}
  Vector(const std::array<T, N>& x) throw() : m_x(x) {}
  Vector& operator=(Vector other) {
    swap(other);
    return *this;
  }

  Vector(const Point<T>& p1, const Point<T>& p2) {
    *this = bilinearCombination<Vector, Point, Point, N>(p1, p2, 1, -1);
  }

  Vector(T a, const Vector& vec) { *this = scale<Vector, Vector, N>(vec, a); }

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

  Vector& set(std::array<T, N>& x) {
    m_x = x;
    return *this;
  }

  Vector& set(const Vector& other) throw() {
    m_x = other.m_x;
    return *this;
  }

  Vector operator+(const Vector& other) const throw() {
    return bilinearCombination<Vector, Vector, Vector, N>(*this, other, 1, 1);
  }

  T dot(const Vector& other) const throw() {
    return innerProduct<Vector, Vector, N>(*this, other);
  }

  T get(int index) const throw() {}

  Vector& add(const Vector& other) throw() {
    m_x += other.m_x;
    m_y += other.m_y;
    m_z += other.m_z;
    return *this;
  }

  Vector& add(float scale, const Vector& other) throw() {
    m_x += scale * other.m_x;
    m_y += scale * other.m_y;
    m_z += scale * other.m_z;
    return *this;
  }

  Vector& sub(const Vector& other) throw() {
    m_x -= other.m_x;
    m_y -= other.m_y;
    m_z -= other.m_z;
    return *this;
  }

  Vector& mul(T f) throw() {
    m_x *= f;
    m_y *= f;
    m_z *= f;
    return *this;
  }

  Vector& div(T f) {
    float recip = 1 / f;
    return mul(recip);
  }

  Vector& rev() throw() {
    m_x = -m_x;
    m_y = -m_y;
    m_z = -m_z;
    return *this;
  }

  T sqnorm() throw() { return (m_x * m_x + m_y * m_y + m_z * m_z); }

  T norm() throw() { return (sqrt(m_x * m_x + m_y * m_y + m_z * m_z)); }

  Vector& normalize() throw() {
    float n = norm();
    if (n > 0.000001) {
      div(n);
    }
    return *this;
  }
};

template <typename T>
Vector<T> operator*(T scale, const Vector<T>& vector) {
  return Vector<T>(scale * vector.x(), scale * vector.y(), scale * vector.z());
}

/*Vector(const Vector& vec, float a, const Vector& I,
         const Vector& J)  // Rotated vec by 'a' parallel to plane (I,J)
  {
    T x = vec.dot(I);
    T y = vec.dot(J);
    float c = cos(a);
    float s = sin(a);
    *this = vec.operator+(
        Vector(x * c - x - y * s, I).operator+(Vector(x * s + y * c - y, J)));
  }*/
/*T x() const throw() { return m_x; }
T y() const throw() { return m_y; }
T z() const throw() { return m_z; }*/

/*Vector<T> cross(const Vector& other) const throw() {
  return Vector(m_y * other.m_z - m_z * other.m_y,
                other.m_x * m_z - m_x * other.m_z,
                m_x * other.m_y - m_y * other.m_x);
}*/
#endif  //_VECTOR_H_
