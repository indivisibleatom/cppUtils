#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>

template <class T>
class Point;

template <class T>
class Vector
{
private:
  T m_x;
  T m_y;
  T m_z;

public:
  Vector() { m_x = 0; m_y = 0; m_z = 0;}
  Vector( const Point<T>& p1, const Point<T>& p2 ) throw() : m_x( p2.x() - p1.x() ), m_y( p2.y() - p1.y() ), m_z( p2.z() - p1.z() ) {} //Vector p1p2
  Vector( const Vector& other ) throw() : m_x ( other.m_x ), m_y( other.m_y ), m_z( other.m_z ) {}
  Vector( T x, T y, T z ) throw() : m_x( x ), m_y( y ), m_z( z ) {}
  Vector( float a, const Vector& vec ) throw() : m_x( a * vec.m_x ), m_y( a * vec.m_y ), m_z( a * vec.m_z ) {}
  Vector( const Vector& vec, float a, const Vector& I, const Vector& J) // Rotated vec by 'a' parallel to plane (I,J)
  {
    T x= vec.dot(I);
    T y= vec.dot(J);
    float c=cos(a);
    float s=sin(a);
    *this = vec.operator+( Vector( x*c-x-y*s, I ).operator+( Vector( x*s+y*c-y, J ) ) );
  }

  Vector operator+( const Vector& other ) const throw()
  {
    Vector retVal( *this );
    retVal.add( other );
    return retVal;
  }

  T x() const throw() { return m_x; }
  T y() const throw() { return m_y; }
  T z() const throw() { return m_z; }

  Vector& set( T x, T y, T z ) throw() 
  {
    m_x = x;
    m_y = y;
    m_z = z;
    return *this;
  }

  T dot( const Vector& other ) const throw()
  {
    return m_x * other.m_x + m_y * other.m_y + m_z * other.m_z;
  }

  Vector<T> cross( const Vector& other ) const throw()
  {
    return Vector(m_y * other.m_z - m_z * other.m_y, other.m_x * m_z - m_x * other.m_z, m_x * other.m_y - m_y * other.m_x);
  }

  Vector& set( const Vector& other ) throw() 
  { 
    m_x = other.m_x;
    m_y = other.m_y;
    m_z = other.m_z;
    return *this;
  }
  Vector& set( int index, T value ) throw() 
  {
    switch ( index )
    {
    case 0: m_x = value;
      break;
    case 1: m_y = value;
      break;
    case 2: m_z = value;
      break;
    }
    return *this;
  }

  T get(int index) const throw()
  {
    switch (index)
    {
    case 0: return m_x;
      break;
    case 1: return m_y;
      break;
    case 2: return m_z;
      break;
    case 3: return 0;
      break;
    }
    assert(false);
    return m_x;
  }

  Vector& add( const Vector& other ) throw() 
  {
    m_x += other.m_x;
    m_y += other.m_y;
    m_z += other.m_z;
    return *this;
  }
  
  Vector& add( float scale, const Vector& other ) throw() {
    m_x += scale*other.m_x;
    m_y += scale*other.m_y;
    m_z += scale*other.m_z; 
    return *this;
  }

  Vector& sub( const Vector& other ) throw() 
  {
    m_x-=other.m_x;
    m_y-=other.m_y;
    m_z-=other.m_z;
    return *this;
  }

  Vector& mul( T f ) throw()
  {
    m_x*=f;
    m_y*=f;
    m_z*=f;
    return *this;
  }

  Vector& div( T f )
  {
    float recip = 1/f;
    return mul( recip );
  }

  Vector& rev() throw()
  {
    m_x=-m_x;
    m_y=-m_y;
    m_z=-m_z;
    return *this;
  }

  T sqnorm() throw()
  {
    return(m_x*m_x + m_y*m_y + m_z*m_z);
  }

  T norm() throw()
  {
    return( sqrt( m_x*m_x + m_y*m_y + m_z*m_z ) );
  }

  Vector& normalize() throw()
  {
    float n=norm();
    if ( n>0.000001 )
    {
      div( n );
    }
    return *this;
  }
};

template<typename T>
Vector<T> operator*( T scale, const Vector<T>& vector )
{
  return Vector<T>( scale * vector.x(), scale * vector.y(), scale * vector.z() );
}

#endif//_VECTOR_H_