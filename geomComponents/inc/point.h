#ifndef _POINT_H_
#define _POINT_H_

#include <cmath>
#include <ostream>

template <class T>
class Vector;

template <class T>
class Point {
 private:
  T m_x;
  T m_y;
  T m_z;

  void swap(Point& other) {
    std::swap(m_x, other.m_x);
    std::swap(m_y, other.m_y);
    std::swap(m_z, other.m_z);
  }

 public:
  Point() {}

  Point(T x, T y, T z) throw() : m_x(x), m_y(y), m_z(z) {}

  Point(const Point& other) throw()
      : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {}

  Point& operator=(Point other) {
    swap(other);
    return *this;
  }

  Point(const Point& point, float a, const Vector<T>& I, const Vector<T>& J,
        const Point& other)  // Rotated point by 'a' around other in plane (I,J)
  {
    float x = Vector<T>(other, point).dot(I);
    float y = Vector<T>(other, point).dot(J);
    float c = cos(a);
    float s = sin(a);
    *this = Point(point, x * c - x - y * s, I, x * s + y * c - y, J);
  }

  Point(const Point& p1, const Point& p2)
      : m_x((p1.m_x + p2.m_x) / 2),
        m_y((p1.m_y + p2.m_y) / 2),
        m_z((p1.m_z + p2.m_z) / 2) {}

  Point(const std::vector<Point<T> >& points) throw() : m_x(0), m_y(0), m_z(0) {
    std::for_each(points.begin(), points.end(), [this](const Point& point) {
      m_x += point.m_x;
      m_y += point.m_y;
      m_z += point.m_z;
    });
    T denominator = ((T)1) / points.size();
    m_x *= denominator;
    m_y *= denominator;
    m_z *= denominator;
  }

  Point(const Point& point, float s, const Vector<T>& vector) throw()
      : m_x(point.x() + s * vector.x()),
        m_y(point.y() + s * vector.y()),
        m_z(point.z() + s * vector.z()) {}

  Point(const Point& point, float s,
        const Point& other) throw()  // point + s ( other - point )
  {
    Vector<T> vector(point, other);
    Point temp(point, s, vector);
    m_x = temp.m_x;
    m_y = temp.m_y;
    m_z = temp.m_z;
  }

  Point(const Point& point, float s, const Vector<T>& v1, float t,
        const Vector<T>& v2) throw()
      : m_x(point.x() + s * v1.x() + t * v2.x()),
        m_y(point.y() + s * v1.y() + t * v2.y()),
        m_z(point.z() + s * v1.z() + t * v2.z()) {}

  T x() const throw() { return m_x; }
  T y() const throw() { return m_y; }
  T z() const throw() { return m_z; }

  Point diff(const Point& other) {
    Point retVal;
    retVal.m_x = m_x - other.m_x;
    retVal.m_y = m_y - other.m_y;
    retVal.m_z = m_z - other.m_z;
    return retVal;
  }

  Point& set(T x, T y, T z) throw() {
    m_x = x;
    m_y = y;
    m_z = z;
    return *this;
  }

  Point& set(const Point& other) throw() {
    m_x = other.m_x;
    m_y = other.m_y;
    m_z = other.m_z;
    return *this;
  }

  Point& set(int index, T value) throw() {
    switch (index) {
      case 0:
        m_x = value;
        break;
      case 1:
        m_y = value;
        break;
      case 2:
        m_z = value;
        break;
    }
    return *this;
  }

  T get(int index) const throw() {
    switch (index) {
      case 0:
        return m_x;
        break;
      case 1:
        return m_y;
        break;
      case 2:
        return m_z;
        break;
      case 3:
        return 1;
        break;
    }
    assert(false);
    return m_x;
  }

  template <class U>
  Point<U> quantizePoint(const Point<T>& minPoint, const Point<T>& maxPoint,
                         int numBits) const {
    Point<U> retVal;
    U x = quantizeValue<U, T>(m_x, minPoint.m_x, maxPoint.m_x, numBits);
    U y = quantizeValue<U, T>(m_y, minPoint.m_y, maxPoint.m_y, numBits);
    U z = quantizeValue<U, T>(m_z, minPoint.m_z, maxPoint.m_z, numBits);
    retVal.set(x, y, z);
    return retVal;
  }

  T distance2(const Point& other) const throw() {
    return ((m_x - other.m_x) * (m_x - other.m_x) +
            (m_y - other.m_y) * (m_y - other.m_y) +
            (m_z - other.m_z) * (m_z - other.m_z));
  }

  T distance(const Point& other) const throw() {
    return sqrt(distance2(other));
  }

  template <class V>
  friend Point<T> operator-(const Point<T>& first, const Point<T>& second);

  template <class V>
  friend std::ostream& operator<<(std::ostream& os, const Point<T>& point);
};

template <class T>
Point<T> operator-(const Point<T>& first, const Point<T>& second) {
  Point<T> retVal;
  retVal.set(first.m_x - second.m_x, first.m_y - second.m_y,
             first.m_z - second.m_z);
  return retVal;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const Point<T>& point) {
  os << point.m_x << " " << point.m_y << " " << point.m_z;
  return os;
}

/*template <class T>
void deltaEncode(const std::vector<Point<T> >& inputPoints,
                 std::vector<Point<T> >& outputPoints) {
  Point<T>& prevPoint;
  std::for_each(
      inputPoints.begin(), inputPoints.end(),
      [&inputPoints, &outputPoints, &prevPoint](const Point<T>& point) {
        if (!outputPoints.empty()) {
          std::function<Point<T>(const Point<T>&, const Point<T>&)> diffFunc =
          operator-;
          deltaEncode<Point<T> >(point, prevPoint, diffFunction);
        } else {
          outputPoints.emplace_back(point);
        }
        prevPoint = point;
      });
}*/

#endif  //_POINT_H_
