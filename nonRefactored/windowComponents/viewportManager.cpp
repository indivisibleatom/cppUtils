#include "precomp.h"
#include "ViewportManager.h"

const int c_viewPortWidth = 1600 / 3;

ViewportManager::ViewportManager( const Rect<int>& rect ) : m_selectedViewport( -1 ), m_fShowingFullScreen( false ),
                                                            m_rect( rect ), m_previousMousePosition( -1, -1, 0 ), m_fKeyEntryMode( false )
{
  createViewportHost( m_pViewportHost, this );
}

void ViewportManager::init()
{
  std::for_each( m_pViewports.cbegin(), m_pViewports.cend(), std::mem_fn( &Viewport::init ) );
}

void ViewportManager::addViewport( std::unique_ptr< Viewport > pViewport )
{
  pViewport->setHost( m_pViewportHost.get() );
  m_pViewports.push_back( std::move( pViewport ) );
  if ( m_selectedViewport == -1 )
  {
    selectViewport( 0 );
  }
}

void ViewportManager::addViewport( INotifiable& listener )
{
  std::unique_ptr< Viewport > pViewport( new Viewport( Rect<int>( m_pViewports.size() * c_viewPortWidth, 0, c_viewPortWidth, 1000 ), listener ) );
  addViewport( std::move(pViewport) );
}

Rect<int> ViewportManager::getViewportRect() const throw()
{
  return Rect<int>( m_pViewports.size() * c_viewPortWidth, 0, c_viewPortWidth, 1000 );
}

int ViewportManager::getSelectedViewport() const throw()
{
  return m_selectedViewport;
}

void ViewportManager::selectViewport( int index ) throw()
{
  if ( index >= (int)m_pViewports.size() || index < 0)
  {
    LOG( "ViewportManager::selectViewport incorrect viewport index", DEBUG_LEVELS::HIGH );
    if ( m_pViewports.size() > 0 )
    {
      selectViewport( m_pViewports.size() - 1 );
    }
    else
    {
      LOG( "ViewportManager::selectViewPort no viewports exist in viewport list", DEBUG_LEVELS::HIGH );
      return;
    }
  }

  //Track the selected viewport
  if ( m_selectedViewport != -1 )
  {
    m_pViewports[m_selectedViewport]->onDeselected();
  }
  m_selectedViewport = index;  
  m_pViewports[m_selectedViewport]->onSelected();
}

void ViewportManager::draw() throw()
{
  glClearColor( 1.0, 1.0, 1.0, 0.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  if ( m_fShowingFullScreen )
  {
    if ( m_selectedViewport != -1 )
    {
      m_pViewports[m_selectedViewport]->draw();
    }
  }
  else
  {
    std::for_each( m_pViewports.cbegin(), m_pViewports.cend(), std::bind( &Viewport::draw, std::placeholders::_1 ) );
    if ( m_selectedViewport != -1 )
    {
      m_pViewports[m_selectedViewport]->setActiveViewport();
    }
  }
}

int ViewportManager::onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState)
{
  m_previousMousePosition.set( mouseX, mouseY, 0 );

  int viewport = getViewportForMouse( mouseX, mouseY );
  selectViewport( viewport );
  if (m_selectedViewport != -1)
  {
    m_pViewports[m_selectedViewport]->onMousePressed( mouseX, mouseY, keyboardState );
  }
  else
  {
    LOG("No viewport currently selected", DEBUG_LEVELS::VERBOSE );
  }
  return 1;
}

int ViewportManager::onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState)
{
  m_previousMousePosition.set( -1, -1, 0 );
  m_pViewports[m_selectedViewport]->onMouseReleased(mouseX, mouseY, keyboardState);
  return 1;
}

int ViewportManager::onMouseDragged(int mouseX, int mouseY, const IKeyboardState& keyboardState)
{
  if ( m_previousMousePosition.x() != -1 )
  {
    int deltaX = mouseX - m_previousMousePosition.x();
    int deltaY = mouseY - m_previousMousePosition.y();

    if (m_selectedViewport != -1)
    {
      m_pViewports[m_selectedViewport]->onMouseDragged(deltaX, deltaY, keyboardState);
    }
    else
    {
      LOG( "No viewport currently selected", DEBUG_LEVELS::VERBOSE );
    }
  }
  m_previousMousePosition.set( mouseX, mouseY, 0 );
  return 1;
}

int ViewportManager::onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState)
{
  if (m_selectedViewport != -1)
  {
    m_pViewports[m_selectedViewport]->onMouseMoved( deltaX, deltaY, keyboardState );
  }
  else
  {
    LOG( "No viewport currently selected", DEBUG_LEVELS::VERBOSE );
  }
  return 1;
}

int ViewportManager::onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState)
{
  if (m_selectedViewport != -1)
  {
    m_pViewports[m_selectedViewport]->onClick( mouseX, mouseY, keyboardState );
  }
  else
  {
    LOG( "No viewport currently selected", DEBUG_LEVELS::VERBOSE );
  }
  return 1;
}

int ViewportManager::onKeyReleased( int key )
{
  if (m_selectedViewport != -1)
  {
    m_pViewports[m_selectedViewport]->onKeyReleased( key );
  }
  else
  {
    LOG( "No viewport currently selected", DEBUG_LEVELS::VERBOSE );
  }
  return 1;
}

int ViewportManager::onKeyPressed( int key )
{
  if ( m_fKeyEntryMode )
  {
    if ( key == FL_Enter )
    {
      return onCommand( m_command );
    }
    else
    {
      m_command.push_back( key );
    }
  }
  else
  {
    switch ( key )
    {
    case '/': m_fKeyEntryMode = true;
      m_command.clear();
      break;
    case ',': m_fShowingFullScreen = !m_fShowingFullScreen;
      if (m_selectedViewport != -1)
      {
        if ( m_fShowingFullScreen )
        {
          m_pViewports[m_selectedViewport]->setRect( m_rect );
        }
        else
        {
          m_pViewports[m_selectedViewport]->setRect( Rect<int>( m_selectedViewport * c_viewPortWidth, 0, c_viewPortWidth, 1000 ) );
        }
      }
      break;
    default: if (m_selectedViewport != -1)
      {
        if ( !m_pViewports[m_selectedViewport]->onKeyPressed( key ) )
        {
          m_pAppController->onKeyPressed( key );
        }
      }
      break;
    }
  }
  return 1;
}

int ViewportManager::onCommand( const std::string& command )
{
  if ( !m_pViewports[m_selectedViewport]->onCommand( command ) )
  {
     m_pAppController->onCommand( command );
  }
  m_fKeyEntryMode = false;
  m_command.clear();
  return 1;
}

int ViewportManager::getViewportForMouse( int mouseX, int mouseY ) const throw()
{
  if ( m_fShowingFullScreen )
  {
    return m_selectedViewport;
  }

  for (size_t i = 0; i < m_pViewports.size(); i++)
  {
    if (m_pViewports[i]->containsPoint(mouseX, mouseY))
    {
      return i;
    }
  }
  LOG("Can't find viewport for mouse. Keeping same viewport!", DEBUG_LEVELS::VERBOSE );
  return m_selectedViewport;
}
