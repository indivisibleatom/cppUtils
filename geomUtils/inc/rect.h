#ifndef _RECT_H_
#define _RECT_H_

template <class T>
class Rect
{
private:
  T m_x;
  T m_y;
  T m_width;
  T m_height;
public:
  Rect( T x, T y, T width, T height ) throw() : m_x( x ), m_y( y ), m_width( width ), m_height( height ) {}
  Rect( const Rect& other ) throw() : m_x( other.m_x ), m_y( other.m_y ), m_width( other.m_width ), m_height( other.m_height ) {}
  T x() const throw() { return m_x; }
  T y() const throw() { return m_y; }
  T width() const throw() { return m_width; }
  T height() const throw() { return m_height; }
  bool contains( T x, T y ) const throw()
  {
    T xTrans = x - m_x;
    T yTrans = y - m_y;
    return ( xTrans >= 0 && xTrans < m_width && yTrans >= 0 && yTrans < m_height );
  }
};

#endif//_RECT_H_