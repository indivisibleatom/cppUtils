#ifndef _PLANE_H_
#define _PLANE_H_

#include "Mesh.h"

// TODO msati3: Clean up to not have Mesh as a templated class
class Plane : public IDrawable, public IInteractable
{
public:
  Plane( double a, double b, double c, double d ); // The plane equation ax + by + cz = d

  void draw() override;
  int onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;
  int onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;
  int onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onKeyReleased( int key ) override;
  int onKeyPressed( int key ) override;

  Vector<float> normal() const { return m_normal; }
  double distance() const { return m_distance; }

private:
  std::vector<Vector<float>> getAxesFromNormal(const Vector<float>& normal);
  Point<float> toWorldCoordinates(const Point<float>& point);

  Vector<float> m_normal;
  double m_distance;
};

bool planeLineIntersection( const Plane& plane, const Point<float>& point1, const Point<float> & point2, Point<float>* retPoint );

#endif //_PLANE_H_
