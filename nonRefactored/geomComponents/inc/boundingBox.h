#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

template <class U>
class BoundingBox
{
private:
  Point<U> m_low;
  Point<U> m_high;

public:
  BoundingBox() {}
  BoundingBox( const Point<U>& low, const Point<U>& high ) : m_low( low ), m_high( high ) {}
  const Point<U>& low() const throw() { return m_low; }
  const Point<U>& high() const throw() { return m_high; }
};

#endif//_BOUNDINGBOX_H_