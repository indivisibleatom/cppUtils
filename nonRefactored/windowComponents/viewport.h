#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include "ViewportHost.h"
#include "viewInterface.h"
#include "SceneGraph.h"
#include "Mesh.h"

class Viewport : public IViewport
{
public:
  Viewport( Viewport&& other ) : m_rect( other.m_rect ), m_pMeshes( std::move( other.m_pMeshes ) ) , m_pViewportHost( nullptr ), 
                                 m_selectedMeshIndex(-1), m_fLighting( true ), m_listener( other.m_listener ) 
  {
    m_pSceneGraphRoot.reset(new TransformationNode());
    std::for_each( m_pMeshes.begin(), m_pMeshes.end(), [ this ] ( IInteractableMesh* pMesh ) {
      std::unique_ptr<GeometryNode> pGeometryNode(new GeometryNode(pMesh));
      m_pSceneGraphRoot->addChild(std::move(pGeometryNode));
    } );
  }

  Viewport( const Rect<int>& rect, INotifiable& listenter );
  void setHost( const IViewportHost* pViewportHost ) throw() { m_pViewportHost = pViewportHost; }

  ISceneNode* addSceneNode(std::unique_ptr<ISceneNode> pSceneNode) throw()
  {
    return m_pSceneGraphRoot->addChild(std::move(pSceneNode));
  }
  void setCameraNode(CameraNode* pCameraNode) throw()
  {
    m_pCameraNode = pCameraNode;
  }
  ISceneNode* addDrawable(IDrawable* pDrawable) throw()
  {
    std::unique_ptr<GeometryNode> pGeometryNode(new GeometryNode(pDrawable));
    return addSceneNode(std::move(pGeometryNode));
  }
  void removeSceneNode(ISceneNode* pSceneNode) throw() { m_pSceneGraphRoot->removeChild(pSceneNode); }

  void registerMesh(IInteractableMesh* pMesh, const Matrix& worldTransformationMatrix);
  void unregisterMesh(IInteractableMesh* pMesh);

  void draw() override;
  void onSelected() {}
  void onDeselected() {}

  template <class U> void lookAt( const Point<U>& lookAtPoint )
  {
    m_pCameraNode->setLookAt( lookAtPoint );
  }

  void setActiveViewport();

  //IViewport
  int onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;
  int onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;
  int onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState) override;
  int onKeyReleased( int ch ) override;
  int onKeyPressed( int ch ) override;
  int onCommand( const std::string& command ) override;
  void init() override;
  void onIdle() override {}
  const Rect<int>& getRect() const throw() override { return m_rect; }
  void setRect( const Rect<int>& rect ) throw() { m_rect = rect; }

  bool containsPoint(int x, int y) const throw();

protected:
  IInteractableMesh* getSelectedMesh() throw();
  void setFrame() throw(); //sets the frame from the modelview matrix

  std::vector< IInteractableMesh* > m_pMeshes;
  int m_selectedMeshIndex;
  bool m_fLighting;
  INotifiable& m_listener;

  Rect<int> m_rect;
  const IViewportHost* m_pViewportHost;

  std::unique_ptr<ISceneNode> m_pSceneGraphRoot;
  CameraNode* m_pCameraNode;
};

#endif//_VIEWPORT_H_