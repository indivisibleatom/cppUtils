#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "Matrix.h"
#include "Drawable.h"
#include "viewInterface.h"

class ISceneNodeVisitor;

// TODO msati: move out Interaction from SceneNode to IInteractableSceneNode, or get rid
class ISceneNode : public IInteractable
{
public:
  ISceneNode() : m_pParent(nullptr) {}
  virtual ~ISceneNode() {}
  ISceneNode* addChild(std::unique_ptr<ISceneNode> pChild);
  void removeChild(ISceneNode* pChild);
  std::vector<std::unique_ptr<ISceneNode>>::iterator childBegin() { return m_pChildren.begin(); }
  std::vector<std::unique_ptr<ISceneNode>>::iterator childEnd() { return m_pChildren.end(); }
  ISceneNode* parent() { return m_pParent; }
  
  virtual Matrix getWorldTransformationMatrix();
  virtual Matrix getLocalTransformationMatrix();

  virtual void accept(ISceneNodeVisitor* pVisitor);

  int onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState) override { return 0; }
  int onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState) override{ return 0; }
  int onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;
  int onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) override { return 0; }
  int onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState) override { return 0; }
  int onKeyReleased(int key) override { return 0; }
  int onKeyPressed(int key) override { return 0; }

private:
  std::vector<std::unique_ptr<ISceneNode>> m_pChildren;
  ISceneNode* m_pParent;
};

class TransformationNode : public ISceneNode
{
public:
  TransformationNode();
  TransformationNode( const Matrix& matrix );

  const float* getTransformationAsGLArray() { return m_matrix.getAsGLArray(); }
  Matrix getLocalTransformationMatrix() override; 

  void accept(ISceneNodeVisitor* pVisitor) override;

  void rotate(const Vector<float>& axis, float angle) { m_matrix.rotate(axis, angle); }
  void translate(const Vector<float>& translation) { m_matrix.translate(translation); }
  void setTransform(const Matrix& matrix) { m_matrix = matrix;  }

protected:
  Matrix m_matrix;
};

class InteractableTransformationNode : public TransformationNode
{
public:
  InteractableTransformationNode() : m_axis(0, 1, 0) {}
  int onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;

  void accept(ISceneNodeVisitor* pVisitor) override;

private:
  Vector<float> m_axis;
};

class ClippingNode : public ISceneNode
{
public:
  ClippingNode();
  ClippingNode(const GLdouble planeEquation[4]);
  const GLdouble* getPlaneEquation() const throw() { return m_planeEquation;  }
  void setClipPlane(float a, float b, float c, float d);

  void accept(ISceneNodeVisitor* pVisitor) override;

private:
  GLdouble m_planeEquation[4];
};

class CameraNode : public ISceneNode
{
public:
  CameraNode();
  int onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState) override;

  void accept(ISceneNodeVisitor* pVisitor) override;

  void setEye(const Point<float>& eye) throw() { m_eye = eye; }
  void setUp(const Vector<float>& up) throw() { m_up = up; }
  void setLookAt(const Point<float>& lookAt) throw() { m_lookAt = lookAt; }
  void beforeRender(bool fLighting);

private:
  int onMouseFreeDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState);

  //Camera parameters
  std::vector< Vector<float> > m_axes;
  
  Point<float> m_eye;
  Point<float> m_lookAt;
  Vector<float> m_up;
};

class LightNode : public ISceneNode
{
public:
  LightNode(const Point<float>& lightPosition) : m_lightPosition(lightPosition) {}
private:
  Point<float> m_lightPosition;
};

class GeometryNode : public ISceneNode
{
public:
  GeometryNode(IDrawable* pDrawable);
  GeometryNode(std::unique_ptr<IDrawable> pDrawable);
  IDrawable& getDrawable() { return *m_pDrawable; }

  void accept(ISceneNodeVisitor* pVisitor) override;

private:
  std::unique_ptr<IDrawable> m_pOwnedDrawable;
  IDrawable* m_pDrawable;
};

#endif // _SCENEGRAPH_H_