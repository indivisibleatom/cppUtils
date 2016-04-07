#include "precomp.h"
#include "Viewport.h"
#include <GL/glew.h>
#include <GL/GLU.h>
#include <boost/iterator/indirect_iterator.hpp>
#include "SceneGraphVisitor.h"

Viewport::Viewport( const Rect<int>& rect, INotifiable& listener ) : m_selectedMeshIndex(-1), m_rect( rect ), m_fLighting( true ), m_listener( listener )
{
  m_pSceneGraphRoot.reset(new TransformationNode());
}

IInteractableMesh* Viewport::getSelectedMesh() throw()
{
  if ( m_selectedMeshIndex == -1 )
  {
    return nullptr;
  }
  return m_pMeshes[ m_selectedMeshIndex ];
}

void Viewport::unregisterMesh( IInteractableMesh* pMesh ) throw()
{
  m_pMeshes.erase( std::remove( m_pMeshes.begin(), m_pMeshes.end(), pMesh ), m_pMeshes.end() );
}

void Viewport::registerMesh(IInteractableMesh* pMesh, const Matrix& worldTransformationMatrix) throw()
{
  m_pMeshes.push_back( pMesh );
  m_pCameraNode->setLookAt(pMesh->boundingBoxCenter());
  m_pCameraNode->setEye(Point<float>(pMesh->boundingBoxCenter().x(), pMesh->boundingBoxCenter().y(), pMesh->boundingBoxCenter().z() + pMesh->boundingBoxSize().z()));
  m_pCameraNode->setUp(Vector<float>(0, 1, 0));
  m_selectedMeshIndex = 0;
}

void Viewport::init()
{
  std::for_each( m_pMeshes.cbegin(), m_pMeshes.cend(), std::mem_fn( &IInteractableMesh::init ) );
}

void Viewport::draw() throw() 
{
  glViewport( m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height() );

  m_pCameraNode->beforeRender( m_fLighting );

  m_pSceneGraphRoot->accept(&RenderingVisitor(RenderingVisitor::RenderFlags::RENDER_OPAQUE));
  m_pSceneGraphRoot->accept(&RenderingVisitor(RenderingVisitor::RenderFlags::RENDER_TRANSPARENT));
}

int Viewport::onMouseDragged( int deltaX, int deltaY, const IKeyboardState& keyboardState )
{
  return m_pSceneGraphRoot->onMouseDragged(deltaX, deltaY, keyboardState);
}

int Viewport::onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState)
{
  if ( IInteractableMesh* pMesh = getSelectedMesh() )
  {
    return pMesh->onMouseReleased( mouseX, mouseY, keyboardState );
  }
  return 0;
}

int Viewport::onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState)
{
  if ( IInteractableMesh* pMesh = getSelectedMesh() )
  {
    return pMesh->onMousePressed( mouseX, mouseY, keyboardState );
  }
  return 0;
}

int Viewport::onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState)
{
  if ( IInteractableMesh* pMesh = getSelectedMesh() )
  {
    return pMesh->onMouseMoved( deltaX, deltaY, keyboardState );
  }
  return 0;
}

int Viewport::onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState)
{
  if ( IInteractableMesh* pMesh = getSelectedMesh() )
  {
    return pMesh->onClick( mouseX, mouseY, keyboardState );
  }
  return 0;
}

int Viewport::onKeyReleased( int ch )
{
  if ( IInteractableMesh* pMesh = getSelectedMesh() )
  {
    return pMesh->onKeyReleased( ch );
  }
  return 0;
}

int Viewport::onKeyPressed( int ch )
{
  if ( IInteractableMesh* pMesh = getSelectedMesh() )
  {
    switch ( ch )
    {
    case '-': lookAt<float>( pMesh->lookAtLocation() );
      return 1;
      break;
    default: return pMesh->onKeyPressed( ch );
      break;
    }
  }
  return 0;
}

int Viewport::onCommand( const std::string& command )
{
  if ( IInteractableMesh* pMesh = getSelectedMesh() )
  {
    return pMesh->onCommand( command );
  }
  return 0;
}

bool Viewport::containsPoint( int x, int y ) const throw()
{
  return m_rect.contains( x, y );
}

void Viewport::setActiveViewport()
{
  glViewport( m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height() );
  m_pCameraNode->beforeRender( m_fLighting );
}
