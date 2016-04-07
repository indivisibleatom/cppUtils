#include <chrono>
#include <FL/gl.h> // TODO msati3: remove dependency on FL??

#include "mouseGestureComponent.h"

void MouseGestureDetector::onEvent(int event) {
  switch (event) {
    case FL_PUSH:
      m_currentTime = std::chrono::high_resolution_clock::now();
      break;
    case FL_RELEASE:
      if (std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::high_resolution_clock::now() - m_currentTime)
              .count() < 250) {
        m_currentGesture = MOUSEGESTURE_Click;
      }
      break;
    default:
      break;
  }
}

MouseGestures MouseGestureDetector::consumeCurrent() {
  int retVal = m_currentGesture;
  m_currentGesture = MOUSEGESTURE_None;
  return static_cast<MouseGestures>(retVal);
}
