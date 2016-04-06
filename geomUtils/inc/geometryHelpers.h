#ifndef _GEOMETRY_HELPERS_H_
#define _GEOMETRY_HELPERS_H_

#include "vector.h"
#include "mesh.h"
#include <vector>


Point<float> projectPoint( const Point<float>& point );  // Does this in opengl coordinate system (0 at bottom of screen)
Point<float> unprojectPoint( int pointX, int pointY );  // Does this in non-opengl coordinate system (window coordinates)
void drawSphere( const Point<float>& center, float radius, COLORS color, QuadricWrapper* pQuadricWrapper );

class PointNormalPair
{
private:
  Point<float> m_point;
  Vector<float> m_normal;
public:
  PointNormalPair() {}
  PointNormalPair( const Point<float>& point, const Vector<float>& normal ) : m_point( point ), m_normal( normal ) {}
  Point<float> point() const { return m_point; }
  Vector<float> normal() const { return m_normal; }
};

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
