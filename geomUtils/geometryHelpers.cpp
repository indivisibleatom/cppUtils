#include "precomp.h"
#include "geometryHelpers.h"
#include "Rect.h"
#include <FL\fl_draw.H>

#define RADPERDEG 0.0174533
#define WINGS 0.10

void GetOrthonormalBasisFromNormal(const Vector<float>& normal, Vector<float>& u, Vector<float>& v, Vector<float>& w)
{
  // u-v-w - Coordinate system - w is along normal
  // determine major direction:
  w = normal;
  int axis = 0;
  float mag = fabs(w.x());
  if (fabs(w.y()) > mag)
  {
    axis = 1;
    mag = fabs(w.y());
  }
  if (fabs(w.z())  > mag)
  {
    axis = 2;
    mag = fabs(w.z());
  }

  w.normalize();

  if (axis != 0)
  {
    v = w.cross(Vector<float>(1, 0, 0));
    v.normalize();
    u = v.cross(w);
  }
  else if (axis != 1)
  {
    v = w.cross(Vector<float>(0, 1, 0));
    v.normalize();
    u = v.cross(w);
  }
  else
  {
    v = w.cross(Vector<float>(0, 0, 1));
    v.normalize();
    u = v.cross(w);
  }
}

Matrix RotationMatrixFromNormal(const Vector<float>& normal)
{
  // u-v-w - Arrow coordinate system:
  Vector<float> u, v, w;
  GetOrthonormalBasisFromNormal(normal, u, v, w);
  return Matrix(u, v, w);
}

void Arrow2D(const Point<float>& tail, const Point<float>& head)
{
  // u-v-w - Arrow coordinate system:
  Vector<float> u, v, w;	
  Vector<float> arrowLine(tail, head);
  GetOrthonormalBasisFromNormal(arrowLine, u, v, w);

  // set size of wings and turn w into a Unit vector:
  float d = WINGS * arrowLine.norm();

  // draw the shaft of the arrow:
  glBegin(GL_LINE_STRIP);
  glVertex3f(tail.x(), tail.y(), tail.z());
  glVertex3f(head.x(), head.y(), head.z());
  glEnd();

  Vector<float> point1(head.x() + d * (u.x() - w.x()), head.y() + d * (u.y() - w.y()), head.z() + d * (u.z() - w.z()));
  glBegin(GL_LINE_STRIP);
  glVertex3f(head.x(), head.y(), head.z());
  glVertex3f(point1.x(), point1.y(), point1.z());
  glEnd();

  Vector<float> point2(head.x() + d * (-u.x() - w.x()), head.y() + d * (-u.y() - w.y()), head.z() + d * (-u.z() - w.z()));    
  glBegin(GL_LINE_STRIP);
  glVertex3f(head.x(), head.y(), head.z());
  glVertex3f(point2.x(), point2.y(), point2.z());
  glEnd();
}

void DrawAxes()
{
  glDisable(GL_LIGHTING);
  glLineWidth(3.0);

  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(1000, 0, 0);
  
  glBegin(GL_LINES);
  glColor3f(0, 1, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 1000, 0);

  glColor3f(0, 0, 1);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 1000);
  glEnd();

  glEnable(GL_LIGHTING);
  glLineWidth(1.0);
}

void DrawCylinder(float bottomRadius, float topRadius, float height)
{
  QuadricWrapper quadObj;
  gluCylinder(quadObj.getRaw(), bottomRadius, topRadius, height, 10, 10);
}

void DrawMessage(const std::string& text, const Point<float>& point)
{
  fl_draw(text.c_str(), point.x(), point.y());
  /*glRasterPos3f(point.x(), point.y(), point.z());

  for (int i = 0; i < text.length(); i++){
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text.data()[i]);
  }*/
}

void AxesDrawable::draw()
{
  DrawAxes();
}


void drawSphere( const Point<float>& center, float radius, COLORS color, QuadricWrapper* pQuadricWrapper )
{
  glColor4f( Colors::R( color ), Colors::G( color ), Colors::B( color ), Colors::A( color ) );
  glTranslatef( center.x(), center.y(), center.z() );
  gluSphere( pQuadricWrapper->getRaw(), radius, 10, 10 );
  glTranslatef( -center.x(), -center.y(), -center.z() );
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
  glReadPixels( pointX, pointY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth );

  float depth1[200 * 200];
  glReadPixels( 200, 200, 200, 200, GL_DEPTH_COMPONENT, GL_FLOAT, depth1 );

  double posArr[4];
  gluUnProject( (double)pointX, (double)pointY, (double)depth, model, proj, viewport, posArr, posArr + 1, posArr + 2 );
  return Point<float>( (float)posArr[0], (float)posArr[1], (float)posArr[2] );
}

Point<float> projectPoint( const Point<float>& point )
{
  int viewport[4];
  double proj[16];
  double model[16];
  glGetIntegerv( GL_VIEWPORT, viewport );
  glGetDoublev( GL_PROJECTION_MATRIX, proj );
  glGetDoublev( GL_MODELVIEW_MATRIX, model );

  double posArr[4];
  gluProject( point.x(), point.y(), point.z(), model, proj, viewport, posArr, posArr + 1, posArr + 2 );
  return Point<float>( (float)posArr[0], (float)posArr[1], (float)posArr[2] );
}

