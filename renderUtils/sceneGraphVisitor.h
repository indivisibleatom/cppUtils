#ifndef _SCENEGRAPHVISITOR_H_
#define _SCENEGRAPHVISITOR_H_

#include "SceneGraph.h"

// TODO msati: Look into http://www.artima.com/cppsource/cooperative_visitor.html for cleaning this up?
class ISceneNodeVisitor
{
public:
  virtual bool visitBegin(InteractableTransformationNode* pNode) = 0;
  virtual bool visitBegin(TransformationNode* pNode) = 0;
  virtual bool visitBegin(ClippingNode* pNode) = 0;
  virtual bool visitBegin(CameraNode* pNode) = 0;
  virtual bool visitBegin(GeometryNode* pNode) = 0;

  virtual bool visitEnd(InteractableTransformationNode* pNode) = 0;
  virtual bool visitEnd(TransformationNode* pNode) = 0;
  virtual bool visitEnd(ClippingNode* pNode) = 0;
  virtual bool visitEnd(CameraNode* pNode) = 0;
  virtual bool visitEnd(GeometryNode* pNode) = 0;
};

class RenderingVisitor : public ISceneNodeVisitor
{
public:
  enum RenderFlags
  {
    RENDER_OPAQUE,
    RENDER_TRANSPARENT,
    RENDER_ALL
  };

  RenderingVisitor(RenderFlags renderFlags) : m_renderFlags(renderFlags) {}
  bool visitBegin(InteractableTransformationNode* pNode) override;
  bool visitBegin(TransformationNode* pNode) override;
  bool visitBegin(ClippingNode* pNode) override;
  bool visitBegin(CameraNode* pNode) override;
  bool visitBegin(GeometryNode* pNode) override;
  
  bool visitEnd(InteractableTransformationNode* pNode) override;
  bool visitEnd(TransformationNode* pNode) override;
  bool visitEnd(ClippingNode* pNode) override;
  bool visitEnd(CameraNode* pNode) override;
  bool visitEnd(GeometryNode* pNode) override;
private:
  RenderFlags m_renderFlags;
};

class TransformationVisitor : public ISceneNodeVisitor
{
public:
  bool visitBegin(InteractableTransformationNode* pNode) override;
  bool visitBegin(TransformationNode* pNode) override;
  bool visitBegin(ClippingNode* pNode) override;
  bool visitBegin(CameraNode* pNode) override;
  bool visitBegin(GeometryNode* pNode) override;

  bool visitEnd(TransformationNode* pNode) override;
  bool visitEnd(ClippingNode* pNode) override;
  bool visitEnd(CameraNode* pNode) override;
  bool visitEnd(GeometryNode* pNode) override;
};

#endif //_SCENEGRAPHVISITOR_H_