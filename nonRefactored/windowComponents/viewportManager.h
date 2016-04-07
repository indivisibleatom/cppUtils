#ifndef _VIEWPORTMANAGER_H_
#define _VIEWPORTMANAGER_H_

#include "ViewportHost.h"
#include "SceneGraph.h"
#include "Viewport.h"
#include "Mesh.h"
#include "Rect.h"

class ViewportManager : public IViewport
{
private:
  std::unique_ptr< IAppController > m_pAppController;
  std::unique_ptr< IViewportHost > m_pViewportHost;
  std::vector< std::unique_ptr< Viewport > > m_pViewports;
  int m_selectedViewport;
  bool m_fShowingFullScreen;
  Rect<int> m_rect;
  Point<int> m_previousMousePosition;

    //Interaction operations for 
  bool m_fKeyEntryMode;
  std::string m_command;

public:
  ViewportManager( const Rect<int>& rect );

  void setAppController( std::unique_ptr< IAppController > pAppController ) { m_pAppController = std::move( pAppController ); }
  void onIdle() override { m_pAppController->onIdle(); }

  void addViewport( std::unique_ptr< Viewport > viewport );
  void addViewport( INotifiable& listener ); //Handle sizing automatically

  Rect<int> getViewportRect() const throw();
  int getSelectedViewport() const throw();
  void selectViewport( int index ) throw();

  template< class T, class U > void registerMeshToViewport(Mesh<T, U>* pMesh, int viewportIndex, const Matrix& worldTransformationMatrix) throw()
  {
    m_pViewports[viewportIndex]->registerMesh(pMesh, worldTransformationMatrix);
  }

  template< class T, class U > void registerMeshToViewport(Mesh<T, U>* pMesh, const Matrix& worldTransformationMatrix) throw()
  {
    registerMeshToViewport(pMesh, getSelectedViewport(), worldTransformationMatrix);
  }

  template< class T, class U > void unregisterMeshFromViewport( Mesh<T,U>* pMesh, int viewportIndex ) throw()
  {
    m_pViewports[viewportIndex]->unregisterMesh(pMesh);
  }

  template< class T, class U > void unregisterMeshFromViewport( Mesh<T,U>* pMesh ) throw()
  {
    unregisterMeshFromViewport(pMesh, getSelectedViewport());
  }

  ISceneNode* addDrawable( IDrawable* pDrawable ) throw()
  {
    return m_pViewports[getSelectedViewport()]->addDrawable(pDrawable);
  }
  
  void removeSceneNode(ISceneNode* pSceneNode) throw()
  {
    m_pViewports[getSelectedViewport()]->removeSceneNode(pSceneNode);
  }

  //IViewport
  int onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;
  int onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;
  int onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onKeyReleased( int ch ) override;
  int onKeyPressed( int ch ) override;
  int onCommand( const std::string& command ) override;
  void draw() throw() override;
  void init() override;
  const Rect<int>& getRect() const throw() override { return m_rect; }

private:
  int getViewportForMouse( int mouseX, int mouseY ) const throw();
};

#endif//_VIEWPORTMANAGER_H_