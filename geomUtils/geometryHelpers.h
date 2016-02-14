#ifndef _GEOMETRY_HELPERS_H_
#define _GEOMETRY_HELPERS_H_

#include "Vector.h"
#include "Mesh.h"
#include <vector>

void Arrow2D(const Point<float>& tail, const Point<float>& head);

template <class U>
void DrawArrow2D(const Point<U>& pt, const Vector<U>& vec, float length)
{
  Point<U> tail = pt;
  Point<U> head = Point<U>(pt, length, vec);
  Arrow2D( tail, head );
}
void DrawAxes();
void DrawCylinder(float bottomRadius, float topRadius, float height);
void DrawMessage(const std::string& text, const Point<float>& point);

void GetOrthonormalBasisFromNormal(const Vector<float>& normal, Vector<float>& u, Vector<float>& v, Vector<float>& w);
Matrix RotationMatrixFromNormal(const Vector<float>& normal);

class AxesDrawable : public IDrawable
{
  void draw() override;
};

#endif//_GEOMETRY_HELPERS_H_