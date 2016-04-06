#ifndef _SHAPE_H_
#define _SHAPE_H_

#include "Point.h"

template <class T> class Sphere;

template <class T>
class Shape
{
public:
  virtual bool intersects( const Shape& other ) const = 0;
  virtual bool intersects( const Sphere<T>& other ) const = 0;
};

template <class T>
class Sphere : public Shape<T>, public IDrawable
{
private:
  Point<T> m_center;
  T m_radius;
  QuadricWrapper m_pSphereQuadric;

public:
  Sphere( const Point<T>& center, T radius ) : m_center( center ), m_radius( radius )
  {
  }

  void draw() override
  {
    drawSphere( m_center, m_radius, COLORS::GREEN, &m_pSphereQuadric );
  }

  float radius() const throw() { return m_radius; }
  Point<T> center() const throw() { return m_center; }

  bool intersects( const Shape& other ) const override
  {
    return other.intersects( *this );
  }

  bool intersects( const Sphere& other ) const override
  {
    return ( m_center.distance2( other.m_center ) < ( ( m_radius + other.m_radius ) * ( m_radius + other.m_radius ) ) );
  }
};

template <class T>
class PlaneTemplated : public Shape<T>, public IDrawable
{
private:
  Vector<T> m_normal;
  T m_distance;
  float m_drawingScale;
public:
  PlaneTemplated(const Vector<T>& normal, T distance) : m_normal(normal), m_distance(distance / 2.0), m_drawingScale(1000)
  {
    m_normal.normalize();
  }
  
  PlaneTemplated(T a, T b, T c, T distance) : m_normal(a, b, c), m_distance(distance / 2.0), m_drawingScale(1000)
  {
    m_normal.normalize();
  }

  void draw() override
  {
    glColor4f( 0.0f, 0.0f, 1.0f, 0.2f );

    //Find two mutually perp vectors in plane
    Vector<T> vec1( -m_normal.y(), m_normal.x(), 0 );
    vec1.normalize();
    if ( m_normal.z() == 0 )
    {
      vec1 = Vector<T>(0,0,1);
    }
    Vector<T> vec2 = vec1.cross( m_normal );
    vec2.normalize();

    Vector<T> v1PlusV2 = vec1; 
    v1PlusV2.add( vec2 );
    v1PlusV2.normalize();
    Vector<T> v1MinusV2 = vec1;
    v1MinusV2.sub( vec2 );
    v1MinusV2.normalize();

    Point<T> p1( Point<T>(0,0,0), m_drawingScale, v1PlusV2 );
    Point<T> p2( Point<T>(0,0,0), m_drawingScale, v1MinusV2 );
    Point<T> p3( Point<T>(0,0,0), -m_drawingScale, v1PlusV2 );
    Point<T> p4( Point<T>(0,0,0), -m_drawingScale, v1MinusV2 );

    glPushMatrix();
    glTranslatef( -m_distance * m_normal.x(), -m_distance * m_normal.y(), -m_distance * m_normal.z() );
      glBegin(GL_QUADS);
        glVertex3f( p1.x(), p1.y(), p1.z() );
        glVertex3f( p2.x(), p2.y(), p2.z() );
        glVertex3f( p3.x(), p3.y(), p3.z() );
        glVertex3f( p4.x(), p4.y(), p4.z() );
      glEnd();
    glPopMatrix();
  }

  bool intersects( const Shape& other ) const override
  {
    return other.intersects( *this );
  }

  bool intersects( const Sphere<T>& other ) const override
  {
    float distanceFromPlane = m_normal.dot( Vector<T>( Point<T>(0,0,0), other.center() ) ) + m_distance;
    return ( (float)fabs( distanceFromPlane ) < other.radius() );
  }

  float distanceFrom( const Sphere<T>& sphere ) const
  {
    return m_normal.dot( Vector<T>( Point<T>(0,0,0), sphere.center() ) ) + m_distance;
  }
};

template <class T>
class Frustrum : public Shape<T>, public IDrawable
{
private:
  std::vector< PlaneTemplated<T> > m_planes; //left, right, top, bottom, near, far

public:
  //Technique from http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
  Frustrum( const float* pModelViewMatrix )
  {
    m_planes.emplace_back( Plane<T>( pModelViewMatrix[12]+pModelViewMatrix[0], pModelViewMatrix[13]+pModelViewMatrix[1], pModelViewMatrix[14]+pModelViewMatrix[2], pModelViewMatrix[15]+pModelViewMatrix[3] ) );
    m_planes.emplace_back( Plane<T>( pModelViewMatrix[12]-pModelViewMatrix[0], pModelViewMatrix[13]-pModelViewMatrix[1], pModelViewMatrix[14]-pModelViewMatrix[2], pModelViewMatrix[15]-pModelViewMatrix[3] ) );
    m_planes.emplace_back( Plane<T>( pModelViewMatrix[12]-pModelViewMatrix[4], pModelViewMatrix[13]-pModelViewMatrix[5], pModelViewMatrix[14]-pModelViewMatrix[6], pModelViewMatrix[15]-pModelViewMatrix[7] ) );
    m_planes.emplace_back( Plane<T>( pModelViewMatrix[12]+pModelViewMatrix[4], pModelViewMatrix[13]+pModelViewMatrix[5], pModelViewMatrix[14]+pModelViewMatrix[6], pModelViewMatrix[15]+pModelViewMatrix[7] ) );
    //m_planes.emplace_back( Plane<T>( pModelViewMatrix[12]+pModelViewMatrix[8], pModelViewMatrix[13]+pModelViewMatrix[9], pModelViewMatrix[14]+pModelViewMatrix[10], pModelViewMatrix[15]+pModelViewMatrix[11] ) );
    //m_planes.emplace_back( Plane<T>( pModelViewMatrix[12]-pModelViewMatrix[8], pModelViewMatrix[13]-pModelViewMatrix[9], pModelViewMatrix[14]-pModelViewMatrix[10], pModelViewMatrix[15]-pModelViewMatrix[11] ) );
  }

  void draw() override
  {
    std::for_each( m_planes.begin(), m_planes.end(), [this] ( Plane<T>& plane ) {
      plane.draw();
    } );
  }

  bool intersects( const Shape& other ) const override
  {
    return other.intersects( *this );
  }

  bool intersects( const Sphere<T>& sphere ) const override
  {
    for ( auto iter = m_planes.begin(); iter != m_planes.end(); ++iter ) {
      float fDistance = iter->distanceFrom( sphere );
      if( fDistance < -sphere.radius() )
      {
         return false;
      }
    }
    return true;
  }
};

#endif//_SHAPE_H_