#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include "helpers.h"

class DrawingState
{
public:
  DrawingState() : m_fOpaque(true) {}
  DrawingState(bool fOpaque) : m_fOpaque(fOpaque) {}

  bool fOpaque() { return m_fOpaque; }

private:
  bool m_fOpaque;
};

class IDrawable
{
public:
  virtual ~IDrawable() = 0 {}
  IDrawable(const DrawingState& drawingState = DrawingState()) : m_fDrawingState(drawingState){}
  virtual void draw() = 0;

  DrawingState& getDrawingState() { return m_fDrawingState;  }
  void setDrawingState(const DrawingState& drawingState) { m_fDrawingState = drawingState; }

protected:
  DrawingState m_fDrawingState;
};

#endif//_DRAWABLE_H_