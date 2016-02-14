#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GL/glew.h>
#include <FL/Fl.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <chrono>
#include "Rect.h"
#include "ViewportHost.h"

enum MouseGestures {
  MOUSEGESTURE_None,
  MOUSEGESTURE_Click,
};

class MouseGestureDetector
{
public:
  MouseGestureDetector() : m_currentGesture( MOUSEGESTURE_None ) {}
  void onEvent( int event );
  MouseGestures consumeCurrent();

private:
  MouseGestures m_currentGesture;
  std::chrono::high_resolution_clock::time_point m_currentTime;
};

class Window : public Fl_Gl_Window, public IViewportHost
{
private:
  IViewport* m_pViewport;

public:
  Window( IViewport* pViewport, const std::string& name );

  bool isKeyPressed( int key ) const throw() override;
  void onIdle(void *pData);

  void draw();
  int handle( int );

private:
  MouseGestureDetector m_gestureDetector;
};

#endif//_WINDOW_H_