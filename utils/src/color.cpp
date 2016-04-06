#include "color.h"

Color::Color(unsigned int color) : m_color(color) {}
unsigned int Color::R() { return ((m_color >> 24) & 0xff); }
unsigned int Color::G() { return (((m_color) >> 16) & 0xff); }
unsigned int Color::B() { return (((m_color) >> 8) & 0xff); }
unsigned int Color::A() { return (m_color & 0xff); }
