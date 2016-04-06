// Contains implementations of the templated mesh class functions
#include "mesh.h"

template <typename T, typename U>
void Mesh<T, U>::populateAuxMembers() {
  m_vm.resize(m_nv);
  m_fVRemoved.resize(m_nv);
  m_tm.resize(m_nt);
  resetMarkers();
  setColorMap();
  populateNormals();
}

template <typename T, typename U>
void Mesh<T, U>::setGTable(VIndex index, const Point<U>& point) {
  m_GTable[index] = point;
}

template <typename T, typename U>
void Mesh<T, U>::setVTable(CIndex index, VIndex value) {
  m_VTable[index] = value;
}

template <typename T, typename U>
void Mesh<T, U>::setOpposites(CIndex index1, CIndex index2) {
  if (index1 != -1)
    m_OTable[index1] = index2;
  if (index2 != -1)
    m_OTable[index2] = index1;
}

template <typename T, typename U>
void Mesh<T, U>::setOTable(CIndex index, CIndex opposite) {
  m_OTable[index] = opposite;
}

template <typename T, typename U>
Mesh<T, U>::Mesh()
    : m_selectedCorner(-1),
      m_fShowEdges(false),
      m_fShowCorners(false),
      m_fShowVertices(false),
      m_fDrawPlane(false),
      m_fShowNormals(true) {}

template <typename T, typename U>
Mesh<T, U>::Mesh(const Mesh& other)
    : m_selectedCorner(other.m_selectedCorner),
      m_VTable(other.m_VTable),
      m_GTable(other.m_GTable),
      m_OTable(other.m_OTable),
      m_nv(other.m_nv),
      m_nt(other.m_nt),
      m_nc(other.m_nc),
      m_fShowEdges(other.m_fShowEdges),
      m_fShowCorners(other.m_fShowCorners),
      m_fShowVertices(other.m_fShowVertices),
      m_tm(other.m_tm),
      m_vm(other.m_vm),
      m_boxCenter(other.m_boxCenter),
      m_fVRemoved(other.m_fVRemoved) {}

template <typename T, typename U>
Mesh<T, U>& Mesh<T, U>::swap(Mesh& other) {
  std::swap(m_selectedCorner, other.m_selectedCorner);
  std::swap(m_VTable, other.m_VTable);
  std::swap(m_OTable, other.m_OTable);
  std::swap(m_GTable, other.m_GTable);
  std::swap(m_nv, other.m_nv);
  std::swap(m_nc, other.m_nc);
  std::swap(m_nt, other.m_nt);
  std::swap(m_tm, other.m_tm);
  std::swap(m_vm, other.m_vn);

  std::swap(m_boxCenter, other.m_boxCenter);
  std::swap(m_fShowCorners, other.m_fShowCorners);
  std::swap(m_fShowEdges, other.m_fShowEdges);
  std::swap(m_fShowVertices, other.m_fShowVertices);
}

template <typename T, typename U>
Mesh<T, U>& Mesh<T, U>::operator=(Mesh other) {
  swap(other);
  return *this;
}

template <typename T, typename U>
Point<U> Mesh<T, U>::centerBoundingBox() const throw() {
  return m_boxCenter;
}

template <typename T, typename U>
void Mesh<T, U>::init() {
  setColorMap();
  initVBO(0);
}

template <typename T, typename U>
void Mesh<T, U>::resetMarkers() {
  std::fill(m_vm.begin(), m_vm.end(), 0);
  std::fill(m_tm.begin(), m_tm.end(), 0);
  std::fill(m_fVRemoved.begin(), m_fVRemoved.end(), false);
}

template <typename T, typename U>
typename Mesh<T, U>::TIndex Mesh<T, U>::t(CIndex c) const throw() {
  return TIndex(c / 3);
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::n(CIndex c) const throw() {
  return CIndex(3 * t(c) + (c + 1) % 3);
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::p(CIndex c) const throw() {
  return CIndex(3 * t(c) + (c + 2) % 3);
}

template <typename T, typename U>
typename Mesh<T, U>::VIndex Mesh<T, U>::v(CIndex c) const throw() {
  return m_VTable[c];
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::o(CIndex c) const throw() {
  return m_OTable[c];
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::l(CIndex c) const throw() {
  return o(n(c));
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::r(CIndex c) const throw() {
  return o(p(c));
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::s(CIndex c) const throw() {
  return n(l(c));
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::u(CIndex c) const throw() {
  return p(r(c));
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::c(TIndex t) const throw() {
  return CIndex(t * 3);
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::offset(CIndex c) const throw() {
  return CIndex(c % 3);
}

template <typename T, typename U>
const Point<U>& Mesh<T, U>::g(CIndex c) const throw() {
  return m_GTable[v(c)];
}

template <typename T, typename U>
const Point<U>& Mesh<T, U>::geom(VIndex v) const throw() {
  return m_GTable[v];
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::c(TIndex tIndex, VIndex vIndex) const
    throw() {
  CIndex startCorner = c(tIndex);
  auto retCIndexIter = std::find_if(
      beginNextIterator(startCorner), endNextIterator(startCorner),
      [this, &vIndex](const CIndex& cIndex) { return v(cIndex) == vIndex; });
  return (retCIndexIter == endNextIterator ? CIndex(-1) : *retCIndexIter);
}

template <typename T, typename U>
Point<U> Mesh<T, U>::centroid(TIndex t) const throw() {
  CIndex corner = c(t);
  std::vector<Point<U>> points;
  std::for_each(cBeginNextIterator(corner), cEndNextIterator(corner),
                [this, &points](const CIndex& cIndex) {
                  points.emplace_back(g(cIndex));
                });
  return Point<U>(points);
}

template <typename T, typename U>
typename Mesh<T, U>::VIndex Mesh<T, U>::nv() const throw() {
  return m_nv;
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::nc() const throw() {
  return m_nc;
}

template <typename T, typename U>
typename Mesh<T, U>::TIndex Mesh<T, U>::nt() const throw() {
  return m_nt;
}

template <typename T, typename U>
typename Mesh<T, U>::CIndex Mesh<T, U>::selectedCorner() const throw() {
  return m_selectedCorner;
}

template <typename T, typename U>
void Mesh<T, U>::setSelectedCorner(CIndex cIndex) {
  // Reset the color for the prev selected corner
  if (m_selectedCorner != -1) {
    m_tm[t(m_selectedCorner)] = m_selectedCornerPrevTM;
  }

  CIndex oldCorner = m_selectedCorner;
  m_selectedCorner = cIndex;
  m_selectedCornerPrevTM = m_tm[t(m_selectedCorner)];
  m_tm[t(m_selectedCorner)] = m_pColorMap->getIndexForColor(COLORS::MAGENTA);
  updateColorsVBO();
  notifySelectedCornerChange(oldCorner, m_selectedCorner);
}

template <typename T, typename U>
Mesh<T, U>::Swing_iterator::Swing_iterator(const Mesh<T, U>* const pMesh,
                                           CIndex corner,
                                           bool fDoneAtleastOneSwing)
    : m_pMesh(pMesh),
      m_corner(corner),
      m_fDoneAtleastOneSwing(fDoneAtleastOneSwing) {}

template <typename T, typename U>
const typename Mesh<T, U>::CIndex& Mesh<T, U>::Swing_iterator::operator*() const
    throw() {
  return m_corner;
}

template <typename T, typename U>
const typename Mesh<T, U>::CIndex* Mesh<T, U>::Swing_iterator::operator->()
    const throw() {
  return &m_corner;
}

template <typename T, typename U>
typename Mesh<T, U>::Swing_iterator& Mesh<T, U>::Swing_iterator::operator++() {
  m_corner = m_pMesh->s(m_corner);
  m_fDoneAtleastOneSwing = true;
  return *this;
}
