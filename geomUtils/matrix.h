#ifndef _MATRIX_H_
#define _MATRIX_H_

class Matrix
{
public:
  Matrix() { setZero();  }
  Matrix(const float matrix[][4]);
  Matrix(const Matrix& other);
  Matrix(const Vector<float>& x, const Vector<float>& y, const Vector<float>& z); //Create rotation matrix with given basis

  void setMatrix(const float matrix[][4]);
  void setIdentity() throw();
  void setZero() throw();

  void rotate(const Vector<float>& axis, float angle);
  void translate(const Vector<float>& vector);
  void invert();

  Matrix operator*(const Matrix& matrix2) const throw();
  const float* getAsGLArray() throw();
  Point<float> transformPoint(const Point<float>& untransformed) const throw();
  Point<float> untransformPoint(const Point<float>& transformed) const throw();
  Vector<float> transformVector(const Vector<float>& untransformed) const throw();
  Vector<float> untransformVector(const Vector<float>& transformed) const throw();

private:
  float m_elements[4][4];
  float m_elementsTransposed[4][4];
};

#endif //_MATRIX_H_