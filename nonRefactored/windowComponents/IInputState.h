#ifndef _IINPUT_STATE_H_
#define _IINPUT_STATE_H_

class IKeyboardState
{
public:
  virtual ~IKeyboardState() = 0 {};
  virtual bool isKeyPressed(int key) const = 0;
};

#endif//_IINPUT_STATE_H_