#ifndef _MOUSE_GESTURE_COMPONENT_H_
#define _MOUSE_GESTURE_COMPONENT_H_

#include "component.h"

enum MouseGestures {
  MOUSEGESTURE_None,
  MOUSEGESTURE_Click,
};

class MouseGestureDetector : public IComponent
{
public:
  MouseGestureDetector() : m_currentGesture( MOUSEGESTURE_None ) {}
  void onEvent( int event );
  MouseGestures consumeCurrent();

private:
  MouseGestures m_currentGesture;
  std::chrono::high_resolution_clock::time_point m_currentTime;
};

#endif  //_MOUSE_GESTURE_COMPONENT_H_

