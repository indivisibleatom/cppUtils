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

class WindowObject : public Fl_Gl_Window, public IEntity
{
private:
  IViewport* m_pViewport;

public:
  WindowObject( IViewport* pViewport, const std::string& name );

  bool isKeyPressed( int key ) const throw() override;
  void onIdle(void *pData);

  void draw();
  int handle( int );

private:
  IViewportHostComponent
  MouseGestureDetectorComponent m_gestureDetector;
};

#endif//_WINDOW_H_
