#include "precomp.h"
#include "SceneGraph.h"
#include "SceneGraphVisitor.h"

const float k_MouseSensitivityMagicNum = 500;

ISceneNode* ISceneNode::addChild(std::unique_ptr<ISceneNode> pChild)
{
  pChild->m_pParent = this;
  m_pChildren.push_back(std::move(pChild));
  return m_pChildren[m_pChildren.size() - 1].get();
}

void ISceneNode::removeChild(ISceneNode* pChild)
{
  m_pChildren.erase(std::remove_if(m_pChildren.begin(), m_pChildren.end(), [&pChild](const std::unique_ptr<ISceneNode>& child) {
    return pChild == child.get();
  }), m_pChildren.end());
}

void ISceneNode::accept(ISceneNodeVisitor* pVisitor)
{
  for (auto& child : m_pChildren)
  {
    child->accept(pVisitor);
  }
}

Matrix ISceneNode::getLocalTransformationMatrix()
{
  Matrix retval;
  retval.setIdentity();
  return retval;
}

Matrix ISceneNode::getWorldTransformationMatrix()
{
  Matrix transformationMatrix;
  transformationMatrix.setIdentity();

  ISceneNode* currentPtr = this;
  while (currentPtr != nullptr)
  {
    transformationMatrix = currentPtr->getLocalTransformationMatrix() * transformationMatrix;
    currentPtr = currentPtr->m_pParent;
  }
  return transformationMatrix;
}

int ISceneNode::onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState)
{
  int retVal = 0;
  for (auto& child : m_pChildren)
  {
    retVal = child->onMouseDragged(deltaX, deltaY, keyboardState);
    if (retVal)
      return retVal;
  }
  return retVal;
}

TransformationNode::TransformationNode()
{
  m_matrix.setIdentity();
}

TransformationNode::TransformationNode( const Matrix& matrix )
{
  m_matrix = matrix;
}

Matrix TransformationNode::getLocalTransformationMatrix()
{
  return m_matrix;
}

void TransformationNode::accept(ISceneNodeVisitor* pVisitor)
{
  pVisitor->visitBegin(this);
  ISceneNode::accept(pVisitor);
  pVisitor->visitEnd(this);
}

int InteractableTransformationNode::onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState)
{
  m_matrix.rotate(m_axis, -(PI * float(deltaX)) / k_MouseSensitivityMagicNum);
  return 1;
}

void InteractableTransformationNode::accept(ISceneNodeVisitor* pVisitor)
{
  pVisitor->visitBegin(this);
  ISceneNode::accept(pVisitor);
  pVisitor->visitEnd(this);
}

ClippingNode::ClippingNode()
{
  std::fill(m_planeEquation, m_planeEquation + 4, 0);
  m_planeEquation[2] = 1;
}

ClippingNode::ClippingNode(const GLdouble planeEquation[4])
{
  std::copy(planeEquation, planeEquation + 4, stdext::checked_array_iterator<GLdouble*>(&m_planeEquation[0], 4));
}

void ClippingNode::setClipPlane(float a, float b, float c, float d)
{
  m_planeEquation[0] = a;
  m_planeEquation[1] = b;
  m_planeEquation[2] = c;
  m_planeEquation[3] = d;
}

void ClippingNode::accept(ISceneNodeVisitor* pVisitor)
{
  pVisitor->visitBegin(this);
  ISceneNode::accept(pVisitor);
  pVisitor->visitEnd(this);
}

CameraNode::CameraNode() : m_up(0.0f, 1.0f, 0.0f), m_eye(0.0f, 0.0f, 1000.0f), m_lookAt(0.0f, 0.0f, 0.0f)
{
  for (int i = 0; i < 3; i++)
  {
    Vector<float> vector(0.0f, 0.0f, 0.0f);
    vector.set(i, 1.0f);
    m_axes.push_back(vector);
  }
}

int CameraNode::onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState)
{
  int retVal = 0;
  if (keyboardState.isKeyPressed('z'))
  {
    m_eye = Point<float>(m_eye, -float(deltaY), m_axes[2]);
  }
  else if (keyboardState.isKeyPressed('x'))
  {
    m_eye = Point<float>(m_eye, -float(deltaY), m_axes[2]);
    m_up = Vector<float>(m_up, -(PI*float(deltaX)) / k_MouseSensitivityMagicNum, m_axes[0], m_axes[1]);
  }
  else
  {
    retVal = ISceneNode::onMouseDragged(deltaX, deltaY, keyboardState);
    if (retVal == 0)
    {
      onMouseFreeDragged(deltaX, deltaY, keyboardState);
    }
  }
  return 1;
}

void CameraNode::accept(ISceneNodeVisitor* pVisitor)
{
  pVisitor->visitBegin(this);
  ISceneNode::accept(pVisitor);
  pVisitor->visitEnd(this);
}

void CameraNode::beforeRender( bool fLighting )
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  Point<float> eye = getWorldTransformationMatrix().transformPoint(m_eye);
  Point<float> lookAt = getWorldTransformationMatrix().transformPoint(m_lookAt);
  Vector<float> up = getWorldTransformationMatrix().transformVector(m_up);
  gluLookAt(eye.x(), eye.y(), eye.z(), lookAt.x(), lookAt.y(), lookAt.z(), up.x(), up.y(), up.z());

  if (fLighting)
  {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_position[] = { eye.x(), eye.y(), eye.z(), 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  }
}

int CameraNode::onMouseFreeDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState)
{
  m_eye = Point<float>(m_eye, -(PI*float(deltaX)) / k_MouseSensitivityMagicNum, m_axes[0], m_axes[2], m_lookAt);
  m_eye = Point<float>(m_eye, (-PI*float(deltaY)) / k_MouseSensitivityMagicNum, m_axes[1], m_axes[2], m_lookAt);
  return 1;
}

GeometryNode::GeometryNode(IDrawable* pDrawable) : m_pOwnedDrawable(nullptr), m_pDrawable(pDrawable)
{
}

GeometryNode::GeometryNode(std::unique_ptr<IDrawable> pDrawable) : m_pOwnedDrawable(std::move(pDrawable)), m_pDrawable(m_pOwnedDrawable.get())
{
}

void GeometryNode::accept(ISceneNodeVisitor* pVisitor)
{
  pVisitor->visitBegin(this);
  ISceneNode::accept(pVisitor);
  pVisitor->visitEnd(this);
}
