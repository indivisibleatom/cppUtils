#include "plane.h"

Plane::Plane( double a, double b, double c, double d ) : m_normal( a, b, c ), m_distance( d )
{
  float norm = m_normal.norm();
  m_normal.normalize();
  m_distance /= norm;
}

void Plane::draw()
{
  glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
  std::vector<Point<float>> boundPoints;
  boundPoints.push_back(Point<float>(500, 500, 0)); boundPoints.push_back(Point<float>(-500, 500, 0));
  boundPoints.push_back(Point<float>(-500, -500, 0)); boundPoints.push_back(Point<float>(500, -500, 0));
  glBegin( GL_QUADS );
    for (const auto& point : boundPoints)
    {
      Point<float> worldPoint = toWorldCoordinates(point);
      glVertex3d(worldPoint.x(), worldPoint.y(), worldPoint.z());
    }
  glEnd();
}

int Plane::onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState) { return 0; }
int Plane::onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState) { return 0; }
int Plane::onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState) { return 0; }
int Plane::onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) { return 0; }
int Plane::onClick(int deltaX, int deltaY, const IKeyboardState& keyboardState) { return 0; }
int Plane::onKeyReleased( int ch ) { return 0; }
int Plane::onKeyPressed( int key ) { return 0; }

std::vector<Vector<float>> Plane::getAxesFromNormal(const Vector<float>& normal)
{
  std::vector<Vector<float>> retVal(3);
  GetOrthonormalBasisFromNormal(normal, retVal[0], retVal[1], retVal[2]);
  return retVal;
}

Point<float> Plane::toWorldCoordinates(const Point<float>& point)
{
  std::vector<Vector<float>> frame = getAxesFromNormal(m_normal);
  return Point<float>(frame[0].x() * point.x() + frame[0].y() * point.y() + frame[0].z() * point.z(),
                      frame[1].x() * point.x() + frame[1].y() * point.y() + frame[1].z() * point.z(),
                      frame[2].x() * point.x() + frame[2].y() * point.y() + frame[2].z() * point.z());
}

bool planeLineIntersection( const Plane& plane, const Point<float>& point1, const Point<float> & point2, Point<float>* retPoint )
{
  Vector<float> op1 = Vector<float>( Point<float>(0,0,0), point1 );
  Vector<float> p1p2 = Vector<float>( point1, point2 );
  float numerator = plane.normal().dot( op1 ) - plane.distance();
  float denominator = plane.normal().dot( p1p2 );
  if ( denominator == 0 )
  {
    retPoint = nullptr;
    return false;
  }
  float s = -numerator / denominator;
  *retPoint = Point<float>( point1, s, p1p2 );
  return true;
}