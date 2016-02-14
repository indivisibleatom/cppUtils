#ifndef _VIEWINTERFACE_H_
#define _VIEWINTERFACE_H_

#include <string>
#include "IInputState.h"

class IInteractable
{
public:
  virtual ~IInteractable() = 0 {}
  virtual int onMousePressed( int mouseX, int mouseY, const IKeyboardState& keyboardState) = 0;
  virtual int onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState) = 0;
  virtual int onMouseDragged(int deltaX, int deltaY, const IKeyboardState& keyboardState) = 0;
  virtual int onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) = 0;
  virtual int onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState) = 0;
  virtual int onKeyReleased( int key ) = 0;
  virtual int onKeyPressed( int key ) = 0;
};

class ICommandable
{
public:
  virtual ~ICommandable() = 0 {}
  virtual int onCommand( const std::string& command ) = 0;
};

// Capable of receiving sync notifications
class INotifiable
{
public:
  virtual void onNotify(int message) = 0;
  //virtual void Notify(const string& message, const NotificationParameters& notificationParameters);
};

#endif//_VIEWINTERFACE_H_