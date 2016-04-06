#include "precomp.h"
#include "geometryHelpers.h"
#include "rect.h"
#include <FL/fl_draw.H>

#define RADPERDEG 0.0174533
#define WINGS 0.10

namespace GeomHelpers {
void GetOrthonormalBasisFromNormal(const Vector<float>& normal,
                                   Vector<float>& u, Vector<float>& v,
                                   Vector<float>& w) {
  // u-v-w - Coordinate system - w is along normal
  // determine major direction:
  w = normal;
  int axis = 0;
  float mag = fabs(w.x());
  if (fabs(w.y()) > mag) {
    axis = 1;
    mag = fabs(w.y());
  }
  if (fabs(w.z()) > mag) {
    axis = 2;
    mag = fabs(w.z());
  }

  w.normalize();

  if (axis != 0) {
    v = w.cross(Vector<float>(1, 0, 0));
    v.normalize();
    u = v.cross(w);
  } else if (axis != 1) {
    v = w.cross(Vector<float>(0, 1, 0));
    v.normalize();
    u = v.cross(w);
  } else {
    v = w.cross(Vector<float>(0, 0, 1));
    v.normalize();
    u = v.cross(w);
  }
}

Matrix RotationMatrixFromNormal(const Vector<float>& normal) {
  // u-v-w - Coordinate system:
  Vector<float> u, v, w;
  GetOrthonormalBasisFromNormal(normal, u, v, w);
  return Matrix(u, v, w);
}

Point<float> unprojectPoint(int pointX, int pointY) {
  int viewport[4];
  double proj[16];
  double model[16];
  pointY = (float)viewport[3] -
           pointY;  // Invert Y, as OpenGL coordinates start at bottom
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  float depth;
  glReadPixels(pointX, pointY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

  float depth1[200 * 200];
  glReadPixels(200, 200, 200, 200, GL_DEPTH_COMPONENT, GL_FLOAT, depth1);

  double posArr[4];
  gluUnProject((double)pointX, (double)pointY, (double)depth, model, proj,
               viewport, posArr, posArr + 1, posArr + 2);
  return Point<float>((float)posArr[0], (float)posArr[1], (float)posArr[2]);
}

Point<float> projectPoint(const Point<float>& point) {
  int viewport[4];
  double proj[16];
  double model[16];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  glGetDoublev(GL_MODELVIEW_MATRIX, model);

  double posArr[4];
  gluProject(point.x(), point.y(), point.z(), model, proj, viewport, posArr,
             posArr + 1, posArr + 2);
  return Point<float>((float)posArr[0], (float)posArr[1], (float)posArr[2]);
}
} //namespace GeomHelpers
