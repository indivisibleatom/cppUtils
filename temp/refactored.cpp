void Arrow2D(const Point<float>& tail, const Point<float>& head) {
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

  Vector<float> point1(head.x() + d * (u.x() - w.x()),
                       head.y() + d * (u.y() - w.y()),
                       head.z() + d * (u.z() - w.z()));
  glBegin(GL_LINE_STRIP);
  glVertex3f(head.x(), head.y(), head.z());
  glVertex3f(point1.x(), point1.y(), point1.z());
  glEnd();

  Vector<float> point2(head.x() + d * (-u.x() - w.x()),
                       head.y() + d * (-u.y() - w.y()),
                       head.z() + d * (-u.z() - w.z()));
  glBegin(GL_LINE_STRIP);
  glVertex3f(head.x(), head.y(), head.z());
  glVertex3f(point2.x(), point2.y(), point2.z());
  glEnd();
}

void DrawAxes() {
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

void DrawCylinder(float bottomRadius, float topRadius, float height) {
  QuadricWrapper quadObj;
  gluCylinder(quadObj.getRaw(), bottomRadius, topRadius, height, 10, 10);
}

void DrawMessage(const std::string& text, const Point<float>& point) {
  fl_draw(text.c_str(), point.x(), point.y());
  /*glRasterPos3f(point.x(), point.y(), point.z());

  for (int i = 0; i < text.length(); i++){
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text.data()[i]);
  }*/
}

void AxesDrawable::draw() { DrawAxes(); }

void drawSphere(const Point<float>& center, float radius, COLORS color,
                QuadricWrapper* pQuadricWrapper) {
  glColor4f(Colors::R(color), Colors::G(color), Colors::B(color),
            Colors::A(color));
  glTranslatef(center.x(), center.y(), center.z());
  gluSphere(pQuadricWrapper->getRaw(), radius, 10, 10);
  glTranslatef(-center.x(), -center.y(), -center.z());
}
