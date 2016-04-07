

void Arrow2D(const Point<float>& tail, const Point<float>& head);

void drawSphere(const Point<float>& center, float radius, COLORS color,
                QuadricWrapper* pQuadricWrapper);

template <class U>
void DrawArrow2D(const Point<U>& pt, const Vector<U>& vec, float length) {
  Point<U> tail = pt;
  Point<U> head = Point<U>(pt, length, vec);
  Arrow2D(tail, head);
}

void DrawAxes();
void DrawCylinder(float bottomRadius, float topRadius, float height);
void DrawMessage(const std::string& text, const Point<float>& point);

class AxesDrawable : public IDrawable {
  void draw() override;
};
