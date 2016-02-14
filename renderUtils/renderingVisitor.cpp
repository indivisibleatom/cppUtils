#include "precomp.h"
#include "SceneGraphVisitor.h"

bool RenderingVisitor::visitBegin(TransformationNode* pNode)
{
  glPushMatrix();
  glMultMatrixf(pNode->getTransformationAsGLArray());
  return true;
}

bool RenderingVisitor::visitEnd(TransformationNode* pNode)
{
  glPopMatrix();
  return true;
}

bool RenderingVisitor::visitBegin(InteractableTransformationNode* pNode)
{
  glPushMatrix();
  glMultMatrixf(pNode->getTransformationAsGLArray());
  return true;
}

bool RenderingVisitor::visitEnd(InteractableTransformationNode* pNode)
{
  glPopMatrix();
  return true;
}

bool RenderingVisitor::visitBegin(ClippingNode* pNode)
{
  glClipPlane(GL_CLIP_PLANE0, pNode->getPlaneEquation());
  glEnable(GL_CLIP_PLANE0);
  return true;
}

bool RenderingVisitor::visitEnd(ClippingNode* pNode)
{
  glDisable(GL_CLIP_PLANE0);
  return true;
}

bool RenderingVisitor::visitBegin(CameraNode* pNode)
{
  return true;
}

bool RenderingVisitor::visitEnd(CameraNode* pNode)
{
  return true;
}

bool RenderingVisitor::visitBegin(GeometryNode* pNode)
{
  bool render = ((m_renderFlags == RenderFlags::RENDER_ALL) || (m_renderFlags == RenderFlags::RENDER_OPAQUE && pNode->getDrawable().getDrawingState().fOpaque()) ||
    (m_renderFlags == RenderFlags::RENDER_TRANSPARENT && !pNode->getDrawable().getDrawingState().fOpaque()));
  if (render)
  {
    pNode->getDrawable().draw();
  }
  return true;
}

bool RenderingVisitor::visitEnd(GeometryNode* pNode)
{
  return true;
}