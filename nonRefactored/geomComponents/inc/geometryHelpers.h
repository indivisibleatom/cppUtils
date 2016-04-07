#ifndef _GEOMETRY_HELPERS_H_
#define _GEOMETRY_HELPERS_H_

#include "vector.h"
#include "mesh.h"
#include <vector>

// Miscellaneous geometry functionality goes here. May be factored into
// subfiles as need be
namespace GeomHelpers {
template <typename T>
class PointNormalPair {
 private:
  Point<T> m_point;
  Vector<T> m_normal;

 public:
  PointNormalPair() {}
  PointNormalPair(const Point<T>& point, const Vector<T>& normal)
      : m_point(point), m_normal(normal) {}
  Point<T> point() const { return m_point; }
  Vector<T> normal() const { return m_normal; }
};

// Does this in opengl coordinate system (0 at bottom of screen)
template <typename T>
Point<T> projectPoint(const Point<T>& point);

// Does this in non-opengl coordinate system (window coordinates)
template <typename T>
Point<T> unprojectPoint(const Point<T>& point);

// Given a normal, return an 3D orthonormal basis for the same. The returned
// basis has the normal as the third component
template <typename T>
void GetOrthonormalBasisFromNormal(const Vector<T>& normal,
                                   Vector<T>& u, Vector<T>& v,
                                   Vector<T>& w);

// Given a vector, return a matrix with the 3D orthonormal basis as column
// vectors (appropriately ordered)
template <typename T>
Matrix RotationMatrixFromNormal(const Vector<float>& normal);
} // namespace GeomHelpers

#endif  //_GEOMETRY_HELPERS_H_
