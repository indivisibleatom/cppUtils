#include "precomp.h"
#include "curve.h"

const float k_RadiusBalls = 10.0f;

Curve::Curve(bool fClosed) : m_fClosed(fClosed), m_fDrawDiscs(true)
{
}

Curve::Curve(const Curve& other)
{
  m_points = other.m_points;
}

Curve& Curve::operator=(const Curve& other)
{
  Curve temp(other);
  temp.swap(*this);
  return *this;
}

void Curve::swap(Curve& other)
{
  std::swap(m_points, other.m_points);
}

int Curve::nv() const throw()
{
  return m_points.size();
}

void Curve::addPoint(const Point<float>& point)
{
  m_points.push_back(point);
}

void Curve::setPoint(int index, const Point<float>& point)
{
  assert(index < m_points.size() && index >= 0);
  m_points[index] = point;
}

void Curve::draw()
{
  if (m_fDrawDiscs)
  {
    for(const auto& point : m_points)
    {
      drawSphere(point, k_RadiusBalls, COLORS::BLACK, &QuadricWrapper());
    }
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_POLYGON);
  for (const auto& point : m_points)
  {
    glVertex3f(point.x(), point.y(), point.z());
  }
  
  if (m_fClosed) // Then close the curve
  {
    glVertex3f(m_points[0].x(), m_points[0].y(), m_points[0].z());
  }
  glEnd();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int Curve::next(int index)
{
  int retVal = index + 1;
  if (m_fClosed)
  {
    return retVal % m_points.size();
  }
  return std::min<int>(retVal, m_points.size() - 1);
}

int Curve::prev(int index)
{
  int retVal = index - 1;
  if (m_fClosed)
  {
    return (retVal + m_points.size()) % m_points.size();
  }
  return std::max<int>(retVal, 0);
}

void Curve::tuck(float amount)
{
  std::vector<Point<float>> tuckedPoints;
  for (int i = 0; i < m_points.size(); ++i)
  {
    Point<float> midPoint = Point<float>(m_points[prev(i)], m_points[next(i)]);
    Point<float> tucked = Point<float>(m_points[i], amount, Vector<float>(m_points[i], midPoint));
    tuckedPoints.push_back(tucked);
  }
  m_points = tuckedPoints;
}

void Curve::untuck(float amount)
{
  std::vector<Point<float>> untuckedPoints;
  for (int i = 0; i < m_points.size(); ++i)
  {
    Point<float> midPoint = Point<float>(m_points[prev(i)], m_points[next(i)]);
    Point<float> untucked = Point<float>(m_points[i], -amount, Vector<float>(m_points[i], midPoint));
    untuckedPoints.push_back(untucked);
  }
  m_points = untuckedPoints;
}

void Curve::refine()
{
  std::vector<Point<float>> refinedPoints;
  for (int i = 0; i < m_points.size(); ++i)
  {
    refinedPoints.push_back(m_points[i]);
    refinedPoints.push_back(Point<float>(m_points[i], m_points[next(i)]));
  }
  m_points = refinedPoints;
}

void Curve::dual()
{
  std::vector<Point<float>> dualPoints;
  for (int i = 0; i < m_points.size(); ++i)
  {
    dualPoints.push_back(Point<float>(m_points[i], m_points[next(i)]));
  }
  m_points = dualPoints;
}

int Curve::onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState) { return 0; }
int Curve::onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState){ return 0; }
int Curve::onMouseDragged(int mouseX, int mouseY, const IKeyboardState& keyboardState) { return 0; }
int Curve::onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) { return 0; }
int Curve::onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState) { return 0; }
int Curve::onKeyPressed(int ch) { return 0; }
int Curve::onKeyReleased(int ch) { return 0; }

