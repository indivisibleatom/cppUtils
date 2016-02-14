#include "precomp.h"
#include "helpers.h"
#include <cstdlib>
#include <GL\glew.h>

template<> float strtoT<float>( const char* str, char **endPtr )
{
  return (float)strtod( str, endPtr );
}

template<> double strtoT<double>( const char* str, char **endPtr )
{
  return strtod( str, endPtr );
}

template<> int strtoT<int>( const char* str, char **endPtr )
{
  return (int)strtol( str, endPtr, 10 );
}

template<> long strtoT<long>( const char* str, char **endPtr )
{
  return strtol( str, endPtr, 10 );
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

template<> int quantizeValue<int, float>( float value, float minValue, float maxValue, int numBits )
{
  float normalized = ( value - minValue ) / ( maxValue - minValue );
  int scaleNumber = 1<<numBits;
  int quantized = scaleNumber * normalized;
  return clamp( quantized, 0, 1<<numBits );
}

template<> int quantizeValue<int, float>( float value, float maxValue, int numBits )
{
  return quantizeValue<int, float>( value, -maxValue, maxValue, numBits );
}

Point<float> unprojectPoint( int pointX, int pointY )
{
  int viewport[4];
  double proj[16];
  double model[16];
  pointY = (float)viewport[3] - pointY; //Invert Y, as OpenGL coordinates start at bottom
  glGetIntegerv( GL_VIEWPORT, viewport );
  glGetDoublev( GL_PROJECTION_MATRIX, proj );
  glGetDoublev( GL_MODELVIEW_MATRIX, model );
  float depth;
  glReadPixels( pointX, pointY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

  float depth1[200*200];
  glReadPixels( 200, 200, 200, 200, GL_DEPTH_COMPONENT, GL_FLOAT, depth1 );

  double posArr[4];
  gluUnProject((double)pointX, (double)pointY, (double)depth, model, proj, viewport, posArr, posArr + 1, posArr + 2);
  return Point<float>((float)posArr[0], (float)posArr[1], (float)posArr[2]);
}

Point<float> projectPoint(const Point<float>& point)
{
  int viewport[4];
  double proj[16];
  double model[16];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  glGetDoublev(GL_MODELVIEW_MATRIX, model);

  double posArr[4];
  gluProject(point.x(), point.y(), point.z(), model, proj, viewport, posArr, posArr + 1, posArr + 2);
  return Point<float>((float)posArr[0], (float)posArr[1], (float)posArr[2]);
}

//Replace first occurance of from in str to to
bool replace(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find( from );
    if ( start_pos == std::string::npos )
        return false;
    str.replace( start_pos, from.length(), to );
    return true;
}

void drawSphere( const Point<float>& center, float radius, COLORS color, QuadricWrapper* pQuadricWrapper)
{
  glColor4f( Colors::R( color ), Colors::G( color ), Colors::B( color ), Colors::A( color ) );
  glTranslatef(center.x(), center.y(), center.z());
  gluSphere( pQuadricWrapper->getRaw(), radius, 10, 10 );
  glTranslatef(-center.x(), -center.y(), -center.z());
}

