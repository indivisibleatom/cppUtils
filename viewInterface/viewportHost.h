#ifndef _VIEWPORTHOST_H_
#define _VIEWPORTHOST_H_

#include "Rect.h"
#include "viewInterface.h"
#include "SceneGraph.h"
#include "IInputState.h"

//ViewportHost interface
class IViewportHost : public IKeyboardState
{
public:
  virtual ~IViewportHost() = 0 {}
};

class IAppController : public ICommandable, public IInteractable, public INotifiable
{
public:
  virtual ~IAppController() = 0 {}
  virtual void init() = 0;
  virtual void onIdle() = 0;
};

//Viewport interface
class IViewport : public IInteractable, public ICommandable, public IDrawable
{
public:
  virtual ~IViewport() = 0 {}
  virtual const Rect<int>& getRect() const throw() = 0;
  virtual void init() = 0;
  virtual void onIdle() = 0 {}
};

void createViewportHost( std::unique_ptr< IViewportHost >& pViewportHost, IViewport* pViewport );

#endif//_VIEWPORTHOST_H_