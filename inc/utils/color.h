#ifndef _COLOR_H_
#define _COLOR_H_

#include "colors_gen.h"

class Color {
 public:
  Color(unsigned int color);
  unsigned int R();
  unsigned int G();
  unsigned int B();
  unsigned int A();

 private:
  unsigned int m_color;
};

#endif  //_COLOR_H_
