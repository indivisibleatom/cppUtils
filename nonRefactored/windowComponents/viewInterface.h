#ifndef _VIEWINTERFACE_H_
#define _VIEWINTERFACE_H_

#include <string>

#include "component.h"
#include "IInputState.h"

class IInteractableComponent : public IComponent {
 public:
  virtual int onMousePressed(int mouseX, int mouseY,
                             const IKeyboardState& keyboardState) = 0;
  virtual int onMouseReleased(int mouseX, int mouseY,
                              const IKeyboardState& keyboardState) = 0;
  virtual int onMouseDragged(int deltaX, int deltaY,
                             const IKeyboardState& keyboardState) = 0;
  virtual int onMouseMoved(int deltaX, int deltaY,
                           const IKeyboardState& keyboardState) = 0;
  virtual int onClick(int mouseX, int mouseY,
                      const IKeyboardState& keyboardState) = 0;
  virtual int onKeyReleased(int key, const IKeyboardState& keyboardState) = 0;
  virtual int onKeyPressed(int key, const IKeyboardState& keyboardState) = 0;
};

class ICommandableComponent : public IComponent {
 public:
  virtual int onCommand(const std::string& command) = 0;
};

// Capable of receiving sync notifications
class INotifiableComponent : public IComponent {
 public:
  virtual void onNotify(int message) = 0;
  // virtual void Notify(const string& message, const NotificationParameters&
  // notificationParameters);
};

#endif  //_VIEWINTERFACE_H_
