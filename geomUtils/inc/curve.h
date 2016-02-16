#ifndef _CURVE_H_
#define _CURVE_H_

#include "viewInterface.h"
#include "Drawable.h"

class Curve : public IInteractable, public IDrawable
{
public:
  Curve(bool fClosed);
  Curve(const Curve& other);
  Curve& operator=(const Curve& other);
  void swap(Curve& other);

  int next(int index);
  int prev(int index);

  int nv() const throw();
  void addPoint(const Point<float>& point);
  void setPoint(int index, const Point<float>& point);
  void tuck(float amount);
  void untuck(float amount);
  void refine();
  void dual();
  void dual2() { dual();  dual(); }

  void draw() override;

#pragma region CONTROL
  int onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseDragged(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;
  int onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onKeyReleased(int ch) override;
  int onKeyPressed(int ch) override;
#pragma endregion CONTROL

private:
  std::vector<Point<float>> m_points;
  bool m_fDrawDiscs;
  bool m_fClosed;
};

#endif//_CURVE_H_