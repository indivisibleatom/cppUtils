#ifndef _RESOURCEWRAPPERS_H_
#define _RESOURCEWRAPPERS_H_

#include <GL/glew.h>

class QuadricWrapper
{
private:
  GLUquadric* m_pQuadric;

public:
  QuadricWrapper()
  {
    m_pQuadric = gluNewQuadric();
  }

  ~QuadricWrapper()
  {
    gluDeleteQuadric( m_pQuadric );
  }

  QuadricWrapper( QuadricWrapper& other )
  {
    m_pQuadric = gluNewQuadric();
  }

  QuadricWrapper operator= ( QuadricWrapper& other )
  {
    m_pQuadric = gluNewQuadric();
  }

  QuadricWrapper( QuadricWrapper&& other )
  {
    m_pQuadric = nullptr;
    std::swap( m_pQuadric, other.m_pQuadric );
  }

  GLUquadric* getRaw() const
  {
    return m_pQuadric;
  }
};

#endif//_RESOURCEWRAPPERS_H_