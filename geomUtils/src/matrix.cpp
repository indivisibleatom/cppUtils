#include "precomp.h"
#include "Matrix.h"

Matrix::Matrix(const float matrix[][4])
{
  setMatrix(matrix);
}

Matrix::Matrix(const Matrix& other)
{
  setMatrix(other.m_elements);
}

Matrix::Matrix(const Vector<float>& x, const Vector<float>& y, const Vector<float>& z)
{
  setIdentity();
  m_elements[0][0] = x.x();
  m_elements[0][1] = y.x();
  m_elements[0][2] = z.x();

  m_elements[1][0] = x.y();
  m_elements[1][1] = y.y();
  m_elements[1][2] = z.y();

  m_elements[2][0] = x.z();
  m_elements[2][1] = y.z();
  m_elements[2][2] = z.z();
}

void Matrix::setMatrix(const float matrix[][4])
{
  std::copy(&matrix[0][0], &matrix[0][0] + 16, stdext::checked_array_iterator<float*>(&m_elements[0][0], 16));
}

void Matrix::setZero()
{
  std::fill(&m_elements[0][0], &m_elements[0][0] + 16, 0);
}

void Matrix::setIdentity()
{
  setZero();
  for (int i = 0; i < 4; i++)
  {
    m_elements[i][i] = 1;
  }
}

Matrix Matrix::operator*(const Matrix& matrix2) const throw()
{
  Matrix result;
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      for (int k = 0; k < 4; k++)
      {
        result.m_elements[i][j] += m_elements[i][k] * matrix2.m_elements[k][j];
      }
    }
  }
  return result;
}

void Matrix::rotate(const Vector<float>& axis, float angle)
{
  double c = cos(angle);
  double s = sin(angle);
  Matrix rotationMatrix;
  float matrix[4][4] = { c + axis.x()*axis.x()*(1 - c), axis.x()*axis.y()*(1 - c) - axis.z() * s, axis.x() * axis.z()*(1 - c) + axis.y()*s, 0,
    axis.x() * axis.y() * (1 - c) + axis.z() * s, c + axis.y() * axis.y() * (1 - c), axis.y() * axis.z() * (1 - c) - axis.x() * s, 0,
    axis.x() * axis.z() * (1 - c) - axis.y() * s, axis.y() * axis.z() * (1 - c) + axis.x() * s, c + axis.z() * axis.z() * (1 - c), 0,
    0, 0, 0, 1
  };
  rotationMatrix.setMatrix(matrix);
  *this = rotationMatrix * (*this);
}

void Matrix::translate(const Vector<float>& vector)
{
  Matrix translationMatrix;
  float matrix[4][4] = { 0, 0, 0, vector.x(),
    0, 0, 0, vector.y(),
    0, 0, 0, vector.z(),
    0, 0, 0, 1 };
  translationMatrix.setMatrix(matrix);
  *this = translationMatrix * (*this);
}

// From local coordinates to world coordinates X' = AX
Point<float> Matrix::transformPoint(const Point<float>& untransformed) const throw()
{
  Point<float> returnValue;
  for (int i = 0; i < 4; i++)
  {
    float sum = 0;
    for (int j = 0; j < 4; j++)
    {
      sum += m_elements[i][j] * untransformed.get(j);
    }
    returnValue.set(i, sum);
  }
  return returnValue;
}

// From world to local coordinates X' = A-1X
Point<float> Matrix::untransformPoint(const Point<float>& transformed) const throw()
{
  Matrix inverted(*this);
  inverted.invert();
  Point<float> returnValue;
  for (int i = 0; i < 4; i++)
  {
    float sum = 0;
    for (int j = 0; j < 4; j++)
    {
      sum += inverted.m_elements[i][j] * transformed.get(j);
    }
    returnValue.set(i, sum);
  }
  return returnValue;
}

Vector<float> Matrix::transformVector(const Vector<float>& untransformed) const throw()
{
  Vector<float> returnValue;
  for (int i = 0; i < 4; i++)
  {
    float sum = 0;
    for (int j = 0; j < 4; j++)
    {
      sum += m_elements[i][j] * untransformed.get(j);
    }
    returnValue.set(i, sum);
  }
  return returnValue;
}

Vector<float> Matrix::untransformVector(const Vector<float>& transformed) const throw()
{
  Matrix inverted(*this);
  inverted.invert();
  Vector<float> returnValue;
  for (int i = 0; i < 4; i++)
  {
    float sum = 0;
    for (int j = 0; j < 4; j++)
    {
      sum += inverted.m_elements[i][j] * transformed.get(j);
    }
    returnValue.set(i, sum);
  }
  return returnValue;
}

const float* Matrix::getAsGLArray()
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      m_elementsTransposed[i][j] = m_elements[j][i];
    }
  }
  return &m_elementsTransposed[0][0];
}

void Matrix::invert()
{
  float inv[16], det;
  int i;
  float* m = &m_elements[0][0];

  inv[0] = m[5] * m[10] * m[15] -
    m[5] * m[11] * m[14] -
    m[9] * m[6] * m[15] +
    m[9] * m[7] * m[14] +
    m[13] * m[6] * m[11] -
    m[13] * m[7] * m[10];

  inv[4] = -m[4] * m[10] * m[15] +
    m[4] * m[11] * m[14] +
    m[8] * m[6] * m[15] -
    m[8] * m[7] * m[14] -
    m[12] * m[6] * m[11] +
    m[12] * m[7] * m[10];

  inv[8] = m[4] * m[9] * m[15] -
    m[4] * m[11] * m[13] -
    m[8] * m[5] * m[15] +
    m[8] * m[7] * m[13] +
    m[12] * m[5] * m[11] -
    m[12] * m[7] * m[9];

  inv[12] = -m[4] * m[9] * m[14] +
    m[4] * m[10] * m[13] +
    m[8] * m[5] * m[14] -
    m[8] * m[6] * m[13] -
    m[12] * m[5] * m[10] +
    m[12] * m[6] * m[9];

  inv[1] = -m[1] * m[10] * m[15] +
    m[1] * m[11] * m[14] +
    m[9] * m[2] * m[15] -
    m[9] * m[3] * m[14] -
    m[13] * m[2] * m[11] +
    m[13] * m[3] * m[10];

  inv[5] = m[0] * m[10] * m[15] -
    m[0] * m[11] * m[14] -
    m[8] * m[2] * m[15] +
    m[8] * m[3] * m[14] +
    m[12] * m[2] * m[11] -
    m[12] * m[3] * m[10];

  inv[9] = -m[0] * m[9] * m[15] +
    m[0] * m[11] * m[13] +
    m[8] * m[1] * m[15] -
    m[8] * m[3] * m[13] -
    m[12] * m[1] * m[11] +
    m[12] * m[3] * m[9];

  inv[13] = m[0] * m[9] * m[14] -
    m[0] * m[10] * m[13] -
    m[8] * m[1] * m[14] +
    m[8] * m[2] * m[13] +
    m[12] * m[1] * m[10] -
    m[12] * m[2] * m[9];

  inv[2] = m[1] * m[6] * m[15] -
    m[1] * m[7] * m[14] -
    m[5] * m[2] * m[15] +
    m[5] * m[3] * m[14] +
    m[13] * m[2] * m[7] -
    m[13] * m[3] * m[6];

  inv[6] = -m[0] * m[6] * m[15] +
    m[0] * m[7] * m[14] +
    m[4] * m[2] * m[15] -
    m[4] * m[3] * m[14] -
    m[12] * m[2] * m[7] +
    m[12] * m[3] * m[6];

  inv[10] = m[0] * m[5] * m[15] -
    m[0] * m[7] * m[13] -
    m[4] * m[1] * m[15] +
    m[4] * m[3] * m[13] +
    m[12] * m[1] * m[7] -
    m[12] * m[3] * m[5];

  inv[14] = -m[0] * m[5] * m[14] +
    m[0] * m[6] * m[13] +
    m[4] * m[1] * m[14] -
    m[4] * m[2] * m[13] -
    m[12] * m[1] * m[6] +
    m[12] * m[2] * m[5];

  inv[3] = -m[1] * m[6] * m[11] +
    m[1] * m[7] * m[10] +
    m[5] * m[2] * m[11] -
    m[5] * m[3] * m[10] -
    m[9] * m[2] * m[7] +
    m[9] * m[3] * m[6];

  inv[7] = m[0] * m[6] * m[11] -
    m[0] * m[7] * m[10] -
    m[4] * m[2] * m[11] +
    m[4] * m[3] * m[10] +
    m[8] * m[2] * m[7] -
    m[8] * m[3] * m[6];

  inv[11] = -m[0] * m[5] * m[11] +
    m[0] * m[7] * m[9] +
    m[4] * m[1] * m[11] -
    m[4] * m[3] * m[9] -
    m[8] * m[1] * m[7] +
    m[8] * m[3] * m[5];

  inv[15] = m[0] * m[5] * m[10] -
    m[0] * m[6] * m[9] -
    m[4] * m[1] * m[10] +
    m[4] * m[2] * m[9] +
    m[8] * m[1] * m[6] -
    m[8] * m[2] * m[5];

  det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  if (det == 0)
    return;

  det = 1.0 / det;

  for (i = 0; i < 16; i++)
    m[i] = inv[i] * det;
}