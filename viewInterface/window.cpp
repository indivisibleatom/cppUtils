#include "precomp.h"
#include "ViewportHost.h"
#include "window.h"
#include <mutex>

void createViewportHost( std::unique_ptr< IViewportHost >& pViewportHost, IViewport* pViewport )
{
  std::unique_ptr< Window > pWindow( new Window( pViewport, "MeshViewer" ) );
  pWindow->show();
  pViewportHost = std::move( pWindow );
}

namespace
{
  std::function<void(void*)> onIdleCallback;
  void staticIdleCallback(void* pData)
  {
    onIdleCallback(pData);
    Fl::repeat_timeout(2.0, staticIdleCallback);
  }
}

void MouseGestureDetector::onEvent(int event)
{
  switch(event) 
  {
  case FL_PUSH: m_currentTime = std::chrono::high_resolution_clock::now();
    break;
  case FL_RELEASE:
    if ( std::chrono::duration_cast < std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_currentTime).count() < 250 )
    {
      m_currentGesture = MOUSEGESTURE_Click;
    }
    break;
  default: break;
  }
}

MouseGestures MouseGestureDetector::consumeCurrent()
{
  int retVal = m_currentGesture;
  m_currentGesture = MOUSEGESTURE_None;
  return static_cast<MouseGestures>(retVal);
}

Window::Window( IViewport* pViewport, const std::string& name ) : Fl_Gl_Window( pViewport->getRect().x(), pViewport->getRect().y(), 
                                                                                pViewport->getRect().width(), pViewport->getRect().height(), name.c_str() ),
                                                                  m_pViewport( pViewport )
{
  onIdleCallback = std::bind(&Window::onIdle, this, std::placeholders::_1);
  Fl::add_timeout(0.1, staticIdleCallback);
}

bool Window::isKeyPressed( int key ) const throw()
{
  return Fl::event_key( key );
}

void Window::onIdle(void* pData)
{
  m_pViewport->onIdle();
}

void Window::draw()
{
  if ( !valid() )
  {
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      LOG( glewGetErrorString( err ), DEBUG_LEVELS::LOW );
      return ;
    }
    glEnable( GL_DEPTH_TEST );
    glDepthMask( 1 );
    glDepthFunc( GL_LEQUAL ); 
    glFrontFace( GL_CW );

    mode( FL_MULTISAMPLE );

    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60.0f , 1.0f, 0.1f, 10000.0f );
    m_pViewport->init();
  }
  m_pViewport->draw();
}

int Window::handle( int evnt )
{
  switch ( evnt )
  {
  case FL_PUSH: m_pViewport->onMousePressed( Fl::event_x(), Fl::event_y(), *this);
    m_gestureDetector.onEvent( FL_PUSH );
    break;
  case FL_MOVE: m_pViewport->onMouseMoved(Fl::event_x(), Fl::event_y(), *this);
    m_gestureDetector.onEvent( FL_MOVE );
    break;
  case FL_DRAG: m_pViewport->onMouseDragged(Fl::event_x(), Fl::event_y(), *this);
    m_gestureDetector.onEvent( FL_DRAG );
    redraw();
    break;
  case FL_RELEASE: m_pViewport->onMouseReleased(Fl::event_x(), Fl::event_y(), *this);
    m_gestureDetector.onEvent( FL_RELEASE );
    if ( m_gestureDetector.consumeCurrent() == MOUSEGESTURE_Click )
    {
      m_pViewport->onClick(Fl::event_x(), Fl::event_y(), *this);
    }
    redraw();
    break;
  case FL_FOCUS:
  case FL_UNFOCUS: 
    break;
  case FL_KEYDOWN: m_pViewport->onKeyReleased( Fl::event_key() );
    redraw();
    break;
  case FL_KEYUP: m_pViewport->onKeyPressed( Fl::event_key() );
    redraw();
    break;
  default:
    return Fl_Gl_Window::handle( evnt );
  }
  return 1;
}
