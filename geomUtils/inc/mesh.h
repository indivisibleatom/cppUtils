#ifndef _MESH_H_
#define _MESH_H_

#include "precomp.h"
#include "Point.h"
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <fstream>
#include <GL/glew.h>
#include <FL/gl.h>
#include <FL/Fl.H> //TODO msati3: Remove dependency on FL
#include <boost/dynamic_bitset.hpp>
#include "viewInterface.h"
#include "SceneGraph.h"
#include "geometryHelpers.h"

#undef min
#undef max
#include <algorithm>
#include <limits>
#include <map>

const int MAX_VALENCE = 100;

class IInteractableMesh : public ICommandable, public IInteractable, public IDrawable
{
private:
public:
  virtual void init() = 0;
  virtual void draw() = 0;
  IInteractableMesh( IInteractableMesh&& other ) {}
  IInteractableMesh() {}
  virtual ~IInteractableMesh() = 0 {}

  virtual Point<float> boundingBoxCenter() const throw() = 0;
  virtual Point<float> boundingBoxSize() const throw() = 0;
  virtual Point<float> lookAtLocation() const throw() = 0;
};

const int c_numTriangleMarkers = 10;

class ColorMap
{
private:
  std::unique_ptr< std::vector< COLORS > > m_pColorMap;

public:
  ColorMap( std::unique_ptr< std::vector< COLORS > >& pColorMap ) throw() : m_pColorMap( std::move( pColorMap ) ) 
  {
    assert( c_numTriangleMarkers == m_pColorMap->size() );
  }

  const COLORS& operator[]( unsigned char index ) const { return (*m_pColorMap)[ index ]; }
  COLORS& operator[]( unsigned char index ) { return (*m_pColorMap)[ index ]; }
  unsigned int getIndexForColor( COLORS color )
  {
    auto iter = std::find( m_pColorMap->begin(), m_pColorMap->end(), color );
    return ( iter == m_pColorMap->end() ? addColor( color ) : std::distance( m_pColorMap->begin(), iter ) );
  }
  unsigned int addColor( COLORS color ) { m_pColorMap->push_back( color ); return m_pColorMap->size(); }
};

template <class T, class U>
class Mesh : public IInteractableMesh
{
public:
//#ifdef _DEBUG
  BOOST_STRONG_TYPEDEF(T,TIndex);
  BOOST_STRONG_TYPEDEF(T,CIndex);
  BOOST_STRONG_TYPEDEF(T,VIndex);
/*#else
  typedef T TIndex;
  typedef T CIndex;
  typedef T VIndex;
#endif //DEBUG*/

protected:
  std::unique_ptr< ColorMap > m_pColorMap;

  VIndex m_nv;
  TIndex m_nt;
  CIndex m_nc;

  std::vector<unsigned char> m_tm;
  std::vector<unsigned char> m_vm;
  std::vector<unsigned char> m_cm;
  CIndex m_selectedCorner;
  unsigned char m_selectedCornerPrevTM; //For the color of the selected corner before it was selected

  std::vector<VIndex> m_VTable;
  std::vector<CIndex> m_OTable;
  std::vector<Point<U>> m_GTable;
  std::vector<Vector<U>> m_normals;
  std::vector<bool> m_fVRemoved;

private:
  GLuint m_vertexVBO;
  GLuint m_colorVBO;
  GLuint m_edgeVBO;
  GLuint m_normalVBO;

  bool m_fDrawPlane;

  Point<U> m_boxCenter;
  BoundingBox<U> m_boundingBox;
  QuadricWrapper m_pCornerQuadric;
  QuadricWrapper m_pVertexQuadric;

  bool m_fShowEdges;
  bool m_fShowVertices;
  bool m_fShowCorners;
  bool m_fShowNormals;

  void setGTable( VIndex index, const Point<U>& point )
  {
    m_GTable[ index ] = point;
  }

  void setVTable( CIndex index, VIndex value )
  {
    m_VTable[ index ] = value;
  }

  void setOpposites( CIndex index1, CIndex index2 )
  {
    if ( index1 != -1 )
      m_OTable[ index1 ] = index2;
    if ( index2 != -1 )
      m_OTable[ index2 ] = index1;
  }

  void setOTable( CIndex index, CIndex opposite )
  {
    m_OTable[ index ] = opposite;
  }

protected:
  virtual void populateAuxMembers()
  {
    m_vm.resize( m_nv );
    m_fVRemoved.resize( m_nv );
    m_tm.resize( m_nt );
    resetMarkers();
    setColorMap();
    populateNormals();
  }

  virtual void setColorMap()
  {
    COLORS colorMap[] = { COLORS::RED, COLORS::GREEN, COLORS::BLUE, COLORS::CYAN, COLORS::MAGENTA, COLORS::YELLOW, COLORS::BLACK, COLORS::WHITE, COLORS::RED, COLORS::NONE };
    std::unique_ptr< std::vector<COLORS> > pVecColorMap( new std::vector<COLORS>() );
    for (int i = 0; i < sizeof( colorMap ) / sizeof( colorMap[0] ); i++)
    {
      pVecColorMap->push_back( colorMap[i] );
    }
    m_pColorMap.reset( new ColorMap( pVecColorMap ) );
  }

public:
  Mesh() : m_selectedCorner( -1 ), m_fShowEdges( false ), m_fShowCorners( false ), m_fShowVertices( false ), m_fDrawPlane( false ), m_fShowNormals( true )
  {
  }

  Mesh( const Mesh& other ) : m_selectedCorner( other.m_selectedCorner ), m_VTable( other.m_VTable ), m_GTable( other.m_GTable ), m_OTable( other.m_OTable ),
    m_nv( other.m_nv ), m_nt( other.m_nt ), m_nc( other.m_nc ), m_fShowEdges( other.m_fShowEdges ), m_fShowCorners( other.m_fShowCorners ), m_fShowVertices( other.m_fShowVertices ),
    m_tm( other.m_tm ), m_vm( other.m_vm ), m_boxCenter( other.m_boxCenter ), m_fVRemoved( other.m_fVRemoved )
  {
  }

  Mesh& swap( Mesh& other )
  {
    std::swap( m_selectedCorner, other.m_selectedCorner );
    std::swap( m_VTable, other.m_VTable );
    std::swap( m_OTable, other.m_OTable );
    std::swap( m_GTable, other.m_GTable );
    std::swap( m_nv, other.m_nv );
    std::swap( m_nc, other.m_nc );
    std::swap( m_nt, other.m_nt );
    std::swap( m_tm, other.m_tm );
    std::swap( m_vm, other.m_vn );

    std::swap( m_boxCenter, other.m_boxCenter );
    std::swap( m_fShowCorners, other.m_fShowCorners );
    std::swap( m_fShowEdges, other.m_fShowEdges );
    std::swap( m_fShowVertices, other.m_fShowVertices );
  }

  Mesh& operator=( Mesh other )
  {
    swap( other );
    return *this;
  }

#pragma region InitMesh
  Point<U> centerBoundingBox() const throw() { return m_boxCenter; }

  void init() override
  {
    setColorMap();
    initVBO( 0 );
  }

  void resetMarkers()
  {
    std::fill( m_vm.begin(), m_vm.end(), 0 );
    std::fill( m_tm.begin(), m_tm.end(), 0 );
    std::fill( m_fVRemoved.begin(), m_fVRemoved.end(), false );
  }

  //void reset()
#pragma endregion InitMesh
  
#pragma region CornerOperators
  TIndex t(CIndex c) const throw() { return TIndex(c/3); }
  CIndex n(CIndex c) const throw() { return CIndex(3*t(c) + (c+1)%3); }
  CIndex p(CIndex c) const throw() { return CIndex(3*t(c) + (c+2)%3); }
  VIndex v(CIndex c) const throw() { return m_VTable[c]; }
  CIndex o(CIndex c) const throw() { return m_OTable[c]; }
  CIndex l(CIndex c) const throw() { return o(n(c)); }
  CIndex r(CIndex c) const throw() { return o(p(c)); }
  CIndex s(CIndex c) const throw() { return n(l(c)); }
  CIndex u(CIndex c) const throw() { return p(r(c)); }
  CIndex c(TIndex t) const throw() { return CIndex(t*3); }
  CIndex offset(CIndex c) const throw() { return CIndex(c%3); }
  const Point<U>& g(CIndex c) const throw() { return m_GTable[v(c)]; }
  const Point<U>& geom(VIndex v) const throw() { return m_GTable[v]; }
  CIndex c(TIndex tIndex, VIndex vIndex) const throw()
  {
    CIndex startCorner = c( tIndex );
    auto retCIndexIter = std::find_if( beginNextIterator( startCorner ), endNextIterator( startCorner ), [ this, &vIndex ]( const CIndex& cIndex ) {
      return v( cIndex ) == vIndex;
    } );
    return ( retCIndexIter == endNextIterator ? CIndex(-1) : *retCIndexIter );
  }
#pragma endregion CornerOperators

#pragma region MiscHelpers
  Point<U> centroid(TIndex t) const throw() 
  {
    CIndex corner = c( t );
    std::vector< Point<U> > points;
    std::for_each( cBeginNextIterator( corner ), cEndNextIterator( corner ), [this, &points] ( const CIndex& cIndex ) {
      points.emplace_back( g( cIndex ) );
    } );
    return Point<U>( points );
  }

  VIndex nv() const throw() { return m_nv; }
  CIndex nc() const throw() { return m_nc; }
  TIndex nt() const throw() { return m_nt; }
  CIndex selectedCorner() const throw() { return m_selectedCorner; }
#pragma endregion MiscHelpers

#pragma region MarkingOperations
void setSelectedCorner( CIndex cIndex )
{
  //Reset the color for the prev selected corner
  if ( m_selectedCorner != -1 )
  {
    m_tm[ t( m_selectedCorner ) ] = m_selectedCornerPrevTM;
  }

  CIndex oldCorner = m_selectedCorner;
  m_selectedCorner = cIndex;
  m_selectedCornerPrevTM = m_tm[ t( m_selectedCorner ) ];
  m_tm[ t( m_selectedCorner ) ] = m_pColorMap->getIndexForColor( COLORS::MAGENTA );
  updateColorsVBO();
  notifySelectedCornerChange( oldCorner, m_selectedCorner );
}

#pragma endregion MarkingOperations

#pragma region SwingIterator
  class Swing_iterator : public std::iterator<std::input_iterator_tag, CIndex, ptrdiff_t, const CIndex*, const CIndex&> // Info about iterator
  {
  private:
    CIndex m_corner;
    bool m_fDoneAtleastOneSwing;
    const Mesh<T,U>* const m_pMesh;
  public:
    Swing_iterator( const Mesh<T,U>* const pMesh, CIndex corner, bool fDoneAtleastOneSwing = false) : m_pMesh( pMesh ), m_corner( corner ), 
                                                                                          m_fDoneAtleastOneSwing( fDoneAtleastOneSwing ) {}
    const CIndex& operator*() const throw() { return m_corner; }
    const CIndex* operator->() const throw() { return &m_corner; }

    Swing_iterator& operator++() 
    {
      m_corner = m_pMesh->s( m_corner ); 
      m_fDoneAtleastOneSwing = true; 
      return *this; 
    } 

    Swing_iterator& operator++(int) 
    { 
      m_corner = m_pMesh->s( m_corner ); 
      m_fDoneAtleastOneSwing = true; 
      return *this; 
    }

    Swing_iterator& operator--() 
    {
      m_corner = m_pMesh->u( m_corner ); 
      m_fDoneAtleastOneSwing = true; 
      return *this; 
    } 

    Swing_iterator& operator--(int) 
    { 
      m_corner = m_pMesh->u( m_corner ); 
      m_fDoneAtleastOneSwing = true; 
      return *this; 
    }

    bool operator!=( const Swing_iterator& other ) const throw() { return !equals( other ); }
    bool operator==( const Swing_iterator& other ) const throw() { return equals( other ); }

  private:
    bool equals( const Swing_iterator& other ) const throw() 
    { 
      return ( m_pMesh == other.m_pMesh && m_corner == other.m_corner && m_fDoneAtleastOneSwing == other.m_fDoneAtleastOneSwing );
    }
  };

  Swing_iterator beginSwingIterator(CIndex corner) { return Swing_iterator( this, corner ); }
  Swing_iterator endSwingIterator(CIndex corner) { return Swing_iterator( this, corner, true/*fDoneAtleastOneSwing*/ ); }
  Swing_iterator cBeginSwingIterator(CIndex corner) const { return Swing_iterator( this, corner ); }
  Swing_iterator cEndSwingIterator(CIndex corner) const { return Swing_iterator( this, corner, true/*fDoneAtleastOneNext*/ ); }
#pragma endregion SwingIterator

#pragma region NextIterator
  class Next_iterator : public std::iterator<std::input_iterator_tag, CIndex, ptrdiff_t, const CIndex*, const CIndex&> // Info about iterator
  {
  private:
    CIndex m_corner;
    bool m_fDoneAtleastOneNext;
    const Mesh<T,U>* m_pMesh;
  public:
    Next_iterator( const Mesh<T,U>* pMesh, CIndex corner, bool fDoneAtleastOneNext = false) : m_pMesh( pMesh ), m_corner( corner ), 
                                                                                             m_fDoneAtleastOneNext( fDoneAtleastOneNext ) {}
    const CIndex& operator*() const throw() { return m_corner; }
    const CIndex* operator->() const throw() { return &m_corner; }

    Next_iterator& operator++() 
    {
      m_corner = m_pMesh->n( m_corner ); 
      m_fDoneAtleastOneNext = true; 
      return *this; 
    } 

    Next_iterator& operator++(int) 
    { 
      m_corner = m_pMesh->n( m_corner ); 
      m_fDoneAtleastOneNext = true; 
      return *this; 
    }

    Next_iterator& operator--() 
    {
      m_corner = m_pMesh->p( m_corner ); 
      m_fDoneAtleastOneNext = true; 
      return *this; 
    } 
    
    Next_iterator& operator--(int) 
    { 
      m_corner = m_pMesh->p( m_corner ); 
      m_fDoneAtleastOneNext = true; 
      return *this; 
    }

    Next_iterator& operator=( const Next_iterator& other ) 
    { 
      m_corner = other.m_corner;
      m_fDoneAtleastOneNext = other.m_fDoneAtleastOneNext;
      m_pMesh = other.m_pMesh;
      return *this;
    }

    bool operator!=( const Next_iterator& other ) const throw() { return !equals( other ); }
    bool operator==( const Next_iterator& other ) const throw() { return equals( other ); }

  private:
    bool equals( const Next_iterator& other ) const throw() 
    { 
      return ( m_pMesh == other.m_pMesh && m_corner == other.m_corner && m_fDoneAtleastOneNext == other.m_fDoneAtleastOneNext );
    }
  };

  Next_iterator beginNextIterator(CIndex corner) { return Next_iterator( this, corner ); }
  Next_iterator endNextIterator(CIndex corner) { return Next_iterator( this, corner, true/*fDoneAtleastOneNext*/ ); }
  Next_iterator cBeginNextIterator(CIndex corner) const { return Next_iterator( this, corner ); }
  Next_iterator cEndNextIterator(CIndex corner) const { return Next_iterator( this, corner, true/*fDoneAtleastOneNext*/ ); }
  Next_iterator beginNextIteratorWithT(TIndex triangle) { return Next_iterator( this, c( triangle ) ); }
  Next_iterator endNextIteratorWithT(TIndex triangle) { return Next_iterator( this, c( triangle ) ); }
#pragma endregion NextIterator

#pragma region PrevIterator
  class Prev_iterator : public std::iterator<std::input_iterator_tag, CIndex, ptrdiff_t, const CIndex*, const CIndex&> // Info about iterator
  {
  private:
    CIndex m_corner;
    bool m_fDoneAtleastOneNext;
    const Mesh<T,U>* m_pMesh;
  public:
    Prev_iterator( const Mesh<T,U>* pMesh, CIndex corner, bool fDoneAtleastOneNext = false) : m_pMesh( pMesh ), m_corner( corner ), 
                                                                                             m_fDoneAtleastOneNext( fDoneAtleastOneNext ) {}
    const CIndex& operator*() const throw() { return m_corner; }
    const CIndex* operator->() const throw() { return &m_corner; }

    Prev_iterator& operator++() 
    {
      m_corner = m_pMesh->p( m_corner ); 
      m_fDoneAtleastOneNext = true; 
      return *this; 
    } 

    Prev_iterator& operator++(int) 
    { 
      m_corner = m_pMesh->p( m_corner ); 
      m_fDoneAtleastOneNext = true; 
      return *this; 
    }

    Prev_iterator& operator--() 
    {
      m_corner = m_pMesh->n( m_corner ); 
      m_fDoneAtleastOneNext = true; 
      return *this; 
    } 
    
    Prev_iterator& operator--(int) 
    { 
      m_corner = m_pMesh->n( m_corner ); 
      m_fDoneAtleastOneNext = true; 
      return *this; 
    }

    Prev_iterator& operator=( const Next_iterator& other ) 
    { 
      m_corner = other.m_corner;
      m_pMesh = other.m_pMesh;
      m_fDoneAtleastOneNext = other.m_fDoneAtleastOneNext;
      return *this;
    }

    bool operator!=( const Prev_iterator& other ) const throw() { return !equals( other ); }
    bool operator==( const Prev_iterator& other ) const throw() { return equals( other ); }

  private:
    bool equals( const Prev_iterator& other ) const throw() 
    { 
      return ( m_pMesh == other.m_pMesh && m_corner == other.m_corner && m_fDoneAtleastOneNext == other.m_fDoneAtleastOneNext );
    }
  };

  Prev_iterator beginPrevIterator(CIndex corner) { return Prev_iterator( this, corner ); }
  Prev_iterator endPrevIterator(CIndex corner) { return Prev_iterator( this, corner, true/*fDoneAtleastOneNext*/ ); }
  Prev_iterator cBeginPrevIterator(CIndex corner) const { return Prev_iterator( this, corner ); }
  Prev_iterator cEndPrevIterator(CIndex corner) const { return Prev_iterator( this, corner, true/*fDoneAtleastOneNext*/ ); }
#pragma endregion PrevIterator

#pragma region TriangleIterator
  class Triangle_iterator : public std::iterator<std::input_iterator_tag, TIndex, ptrdiff_t, const TIndex*, const TIndex&> // Info about iterator
  {
  private:
    TIndex m_triangle;
    const Mesh<T,U>* m_pMesh;
  public:
    Triangle_iterator( const Mesh<T,U>* const pMesh, TIndex triangle ) : m_pMesh( pMesh ), m_triangle( triangle ) {}
    const TIndex& operator*() const throw() { return m_triangle; }
    const TIndex* operator->() const throw() { return &m_triangle; }
    
    Triangle_iterator& operator++() 
    {
      m_triangle++; 
      return *this; 
    } 
    
    Triangle_iterator& operator++(int) 
    {
      m_triangle++;
      return *this; 
    }

    bool operator!=( const Triangle_iterator& other ) const throw() { return !equals( other ); }
    bool operator==( const Triangle_iterator& other ) const throw() { return equals( other ); }

  private:
    bool equals( const Triangle_iterator& other ) const throw() 
    {
      return ( m_pMesh == other.m_pMesh && m_triangle == other.m_triangle );
    }
  };

  Triangle_iterator beginTriangleIterator() { return Triangle_iterator( this, TIndex(0) ); }
  Triangle_iterator endTriangleIterator() { return Triangle_iterator( this, m_nt ); }
  Triangle_iterator cBeginTriangleIterator() const { return Triangle_iterator( this, TIndex(0) ); }
  Triangle_iterator cEndTriangleIterator() const { return Triangle_iterator( this, m_nt ); }
#pragma endregion TriangleIterator

#pragma region CornerIterator
  class Corner_iterator : public std::iterator<std::input_iterator_tag, CIndex, ptrdiff_t, const CIndex*, const CIndex&> // Info about iterator
  {
  private:
    mutable CIndex m_corner;
    const Mesh<T,U>* m_pMesh;
  public:
    Corner_iterator( const Mesh<T,U>* pMesh, CIndex corner ) : m_pMesh( pMesh ), m_corner( corner ) {}
    const CIndex& operator*() const throw() { return m_corner; }
    const CIndex* operator->() const throw() { return &m_corner; }
    
    Corner_iterator& operator++() 
    {
      m_corner++; 
      return *this; 
    } 
    
    Corner_iterator& operator++(int) 
    {
      m_corner++;
      return *this; 
    }

    Corner_iterator& operator=( const Corner_iterator& other ) 
    { 
      m_corner = other.m_corner;
      m_pMesh = other.m_pMesh;
      return *this;
    }

    bool operator!=( const Corner_iterator& other ) const throw() { return !equals( other ); }
    bool operator==( const Corner_iterator& other ) const throw() { return equals( other ); }

  private:
    bool equals( const Corner_iterator& other ) const throw() 
    { 
      return (m_pMesh == other.m_pMesh && m_corner == other.m_corner);
    }
  };

  Corner_iterator beginCornerIterator() const { return Corner_iterator( this, CIndex(0) ); }
  Corner_iterator endCornerIterator() const { return Corner_iterator( this, m_nc ); }
  Corner_iterator cBeginCornerIterator() const { return Corner_iterator( this, CIndex(0) ); }
  Corner_iterator cEndCornerIterator() const { return Corner_iterator( this, m_nc ); }
#pragma endregion CornerIterator

#pragma region VertexIterator
  class Vertex_iterator : public std::iterator<std::input_iterator_tag, VIndex, ptrdiff_t, const VIndex*, const VIndex&> // Info about iterator
  {
  private:
    VIndex m_vertex;
    const Mesh<T,U>* const m_pMesh;
  public:
    Vertex_iterator( const Mesh<T,U>* const pMesh, VIndex vertex ) : m_pMesh( pMesh ), m_vertex( vertex ) {}
    const VIndex& operator*() const throw() { return m_vertex; }
    const VIndex* operator->() const throw() { return &m_vertex; }
    
    Vertex_iterator& operator++() 
    {
      m_vertex++; 
      return *this; 
    } 
    
    Vertex_iterator& operator++(int) 
    {
      m_vertex++;
      return *this; 
    }

    bool operator!=( const Vertex_iterator& other ) const throw() { return !equals( other ); }
    bool operator==( const Vertex_iterator& other ) const throw() { return equals( other ); }

  private:
    bool equals( const Vertex_iterator& other ) const throw() 
    {
      return (m_pMesh == other.m_pMesh && m_vertex == other.m_vertex);
    }
  };

  Vertex_iterator beginVertexIterator() { return Vertex_iterator( this, VIndex(0) ); }
  Vertex_iterator endVertexIterator() { return Vertex_iterator( this, m_nv ); }
  Vertex_iterator cBeginVertexIterator() const { return Vertex_iterator( this, VIndex(0) ); }
  Vertex_iterator cEndVertexIterator() const { return Vertex_iterator( this, m_nv ); }
#pragma endregion VertexIterator

  void computeO()
  {
    std::vector<CIndex> valence( m_nv );

    for (VIndex i=VIndex(0); i < m_nv; i++) 
    {
      valence[i]=CIndex(0);
    }
    for (CIndex i=CIndex(0); i < m_nc; i++)
    {
      valence[v(i)]++;
    }

    std::vector<CIndex> firstIncidentCorner(m_nv); 
    int runningFirstIncidentCorner=0; 
    
    for (VIndex i = VIndex(0); i < m_nv; i++) 
    {
      firstIncidentCorner[i] = runningFirstIncidentCorner; 
      runningFirstIncidentCorner += valence[i];
    };  // head of list of incident corners
    
    for (VIndex i=VIndex(0); i < m_nv; i++)
    {
      valence[i]=CIndex(0);   // valences wil be reused to track how many incident corners were encountered for each vertex
    }

    std::vector<CIndex> C(m_nc); 
    std::for_each ( beginCornerIterator(), endCornerIterator(), [this, &C, &firstIncidentCorner, &valence]( const CIndex& corner ) {
      C[firstIncidentCorner[v(corner)] + valence[v(corner)]++] = corner;  // for each vertex: the list of val[v] incident corners starts at C[fic[v]]
      m_OTable[corner] = -1;
    } );
    
    for ( VIndex i = VIndex(0); i < m_nv; i++ )
    {
      for ( CIndex c1 = firstIncidentCorner[i]; c1 < firstIncidentCorner[i] + valence[i] - 1; c1++ )
      {
        for ( CIndex c2 = CIndex(c1 + 1); c2 < firstIncidentCorner[i] + valence[i]; c2++) 
        { 
          // for each pair (C[a],C[b[]) of its incident corners
          if ( v( n( C[c1] ) ) == v( p( C[c2] ) ) )
          {
            m_OTable[ p( C[c1] ) ]=n( C[c2] ); 
            m_OTable[ n( C[c2] ) ]=p( C[c1] );
          } 
          // if C[a] follows C[b] around v, then p(C[a]) and n(C[b]) are opposite
          if ( v( n( C[c2] ) )==v( p( C[c1] ) ) )
          {
            m_OTable[ p( C[c2] ) ]=n( C[c1] ); 
            m_OTable[ n( C[c1] ) ]=p( C[c2] );
          }
        }
      }
    }
  }

  Vector<U> triangleNormal( CIndex corner, bool fNormalized ) const
  {
    //Vector<U> normal = Vector<U>( g(corner), g( p( corner ) )).cross(Vector<U>( g( corner ),g( n( corner ) ) ) );
    Vector<U> normal = Vector<U>(g(corner), g(n(corner))).cross(Vector<U>(g(corner), g(p(corner))));
    return fNormalized? normal.normalize() : normal;
  }

  void populateNormals()
  {
    std::for_each( cBeginVertexIterator(), cEndVertexIterator(), [ this ]( VIndex vIndex ) {
      m_normals.push_back( Vector<float>(0,0,0) );
    } );

    std::for_each( cBeginCornerIterator(), cEndCornerIterator(), [ this ]( CIndex cIndex ) {
      m_normals[v(cIndex)].add(triangleNormal(cIndex, false));
  } );

    std::for_each( cBeginVertexIterator(), cEndVertexIterator(), [ this ]( VIndex vIndex ) {
      m_normals[vIndex].normalize();
    } );
  }

  Vector<U> vNormal( CIndex corner )
  {
    return m_normals[v(corner)];
    /*Vector<U> normal;
    std::for_each( beginSwingIterator( corner ), endSwingIterator( corner ), [ this, &normal ] ( CIndex cIndex ) {
      normal.add( triangleNormal( cIndex, false ) );
    } );
    normal.normalize();
    return normal;*/
  }

  void computeBox() 
  { 
    // computes center of the bounding box
    Point<U> lowBox = m_GTable[0];
    Point<U> highBox = m_GTable[0];
    std::for_each( cBeginVertexIterator(), cEndVertexIterator(), [ this, &highBox, &lowBox ] ( const VIndex& vIndex ) {
      lowBox.set( 0, std::min<U>( lowBox.x(), m_GTable[ vIndex ].x() ) );
      lowBox.set( 1, std::min<U>( lowBox.y(), m_GTable[ vIndex ].y() ) );
      lowBox.set( 2, std::min<U>( lowBox.z(), m_GTable[ vIndex ].z() ) );

      highBox.set( 0, std::max<U>( highBox.x(), m_GTable[ vIndex ].x() ) );
      highBox.set( 1, std::max<U>( highBox.y(), m_GTable[ vIndex ].y() ) );
      highBox.set( 2, std::max<U>( highBox.z(), m_GTable[ vIndex ].z() ) );
    } );
    m_boxCenter = Point<U>( lowBox, highBox );
    m_boundingBox = BoundingBox<U>( lowBox, highBox );
  };

  void centerMesh()
  {
    std::for_each(cBeginVertexIterator(), cEndVertexIterator(), [this](VIndex vIndex) {
      m_GTable[vIndex] = m_GTable[vIndex] - m_boxCenter;
    });
    computeBox();
  }

  void scaleMesh(float desiredBoundingBoxSize)
  {
    float boundingBoxSize = std::max<float>(m_boundingBox.high().x() - m_boundingBox.low().x(), m_boundingBox.high().y() - m_boundingBox.low().y());
    boundingBoxSize = std::max<float>(m_boundingBox.high().z() - m_boundingBox.low().z(), boundingBoxSize);
    float scale = desiredBoundingBoxSize / boundingBoxSize;
    std::for_each(cBeginVertexIterator(), cEndVertexIterator(), [this, &scale](VIndex vIndex) {
      m_GTable[vIndex].set(scale * m_GTable[vIndex].x(), scale * m_GTable[vIndex].y(), scale * m_GTable[vIndex].z());
    });
    computeBox();
  }

  Point<float> boundingBoxCenter() const throw() override
  {
    return m_boxCenter;
  }

  Point<float> boundingBoxSize() const throw() override
  {
    return Point<float>(m_boundingBox.high() - m_boundingBox.low());
  }

  Point<float> lookAtLocation() const throw() override
  {
    if ( m_selectedCorner != -1 )
    {
      return g( m_selectedCorner );
    }
    return Point<float>(0.0f, 0.0f, 0.0f);
  }

  //Diagnostic
  void checkMesh()
  {
    checkOConsistency();
    checkMaxValence();
  }

  bool isValenceBounded( CIndex cIndex, int maxValence )
  {
    int valence = 0;
    for ( Swing_iterator iter = beginSwingIterator( cIndex ); iter != endSwingIterator( cIndex ); iter++ )
    {
      valence++;
      if ( valence > maxValence )
        return false;
    }
    return true;
  }

  void checkMaxValence()
  {
    std::for_each( beginCornerIterator(), endCornerIterator(), [ this ] ( const CIndex& cIndex ) {
      if ( !isValenceBounded( cIndex, MAX_VALENCE ) )
      {
        std::cout<<"Error "<<cIndex;
        LOG( "Incorrect Mesh as valence > MAX_VALENCE ", DEBUG_LEVELS::LOW );
      }
    } );
  }

  void checkOConsistency()
  {
    std::for_each( beginCornerIterator(), endCornerIterator(), [ this ] ( const CIndex& cIndex ) {
      assert( o( cIndex ) < m_nc );
      assert( o( o( cIndex ) ) == cIndex );
      if ( o ( o ( cIndex ) ) != cIndex )
      {
        std::cout<<"Error o consistency"<<cIndex<<" "<<o(cIndex);
        return;
      }
    } );
  }


#pragma region LOADING AND SAVING
  //TODO msati3: Push this out to a builder class sometime later
  void loadMeshVTS( const boost::filesystem::path& path, int scale = 1 ) 
  {
    LOGPERF;
    boost::iostreams::mapped_file_source file( path );
    const char *currentPtr = file.data();
    char *end = '\0';

    m_nv = VIndex(strtoT<T>( currentPtr, &end ));
    m_GTable.reserve( m_nv );
    currentPtr = end + 1;

    T currentLine = 0;
    Point<U> readPoint;
    while ( errno == 0 && (currentLine++ != m_nv) ) 
    {
      for (int i = 0; i < 3; i++)
      {
        U readValue = strtoT<U>( currentPtr, &end );
        readPoint.set(i, scale * readValue );
        currentPtr = end + 1;
      }
      m_GTable.emplace_back( readPoint );
    }

    m_nt = TIndex(strtoT<T>( currentPtr, &end ));
    m_nc = 3 * m_nt;
    m_VTable.reserve( m_nc );
    m_OTable.resize( m_nc, CIndex(0) );
    currentPtr = end + 1;

    currentLine = 0;
    while ( errno == 0 && (currentLine++ != m_nt) ) 
    {
      for (int i = 0; i < 3; i++)
      {
        VIndex readVIndex = VIndex(strtoT<T>( currentPtr, &end ));
        m_VTable.push_back( readVIndex );
        currentPtr = end + 1;
      }
    }

    computeBox();
    centerMesh();
    scaleMesh(500);
    computeO();
    populateAuxMembers();
  }

  void saveMeshVTS( const std::string& fileName )
  {
    std::fstream file;
    file.open( fileName, std::ios_base::out | std::ios_base::binary );

    file<<m_nv<<"\n";
    assert( m_nv == m_GTable.size() );

    std::for_each( m_GTable.begin(), m_GTable.end(), [ &file ] ( const Point<U>& point ) {
      file<<point.x()<<","<<point.y()<<","<<point.z()<<"\n";
    } );

    file<<m_nt<<"\n";
    assert( 3 * m_nt == m_VTable.size() );

    CIndex count = CIndex(0);
    std::for_each( m_VTable.begin(), m_VTable.end(), [&file, &count ] ( const VIndex& vIndex ) {
      file<<vIndex;
      count++;
      file<<( ( ( count % 3 ) == 0 ) ? "\n":",");
    } );
  }

  void quantizeGeometry( int numBits, std::vector< Point<int> >& quantizedGeometry )
  {
    std::for_each( m_GTable.begin(), m_GTable.end(), [ &quantizedGeometry, this, &numBits ] ( const Point<U> & point ) {
      quantizedGeometry.push_back( point.quantizePoint<int>( m_boundingBox.low(), m_boundingBox.high(), numBits ) );
    } );
  }

protected:
  virtual void serializeVertexData( VIndex vIndex, std::fstream& file )
  {
    Point<U>& point = m_GTable[ vIndex ];
    file<<point.x()<<" "<<point.y()<<" "<<point.z();
  }

  virtual void deserializeVertexData( VIndex vIndex, std::fstream& file )
  {
    for ( int i = 0; i < 3; i++ )
    {
      U value;
      file>>value;
      m_GTable[ vIndex ].set( i, value );
    }
  }

  virtual void onNumVerticesDeserialized()
  {
    m_GTable.assign( m_nv, Point<U>( 0.0f, 0.0f, 0.0f ) );
  }

  virtual void onNumCornersDeserialized()
  {
    m_VTable.resize( m_nc );
    m_OTable.resize( m_nc );
  }

public:
  void serializeMeshVTS( const std::string& fileName )
  {
    std::fstream file;
    file.open( fileName, std::ios_base::out | std::ios_base::binary );

    file<<m_nv<<"\n";
    assert( m_nv == m_GTable.size() );

    std::for_each( beginVertexIterator(), endVertexIterator(), [ this, &file ] ( VIndex vIndex ) {
      serializeVertexData( vIndex, file );
      file<<"\n";
    } );

    file<<m_nt<<"\n";
    assert( 3 * m_nt == m_VTable.size() );

    std::for_each( beginCornerIterator(), endCornerIterator(), [ this, &file ] ( const CIndex& cIndex ) {
      file<<m_VTable[ cIndex ]<<" "<<m_OTable[ cIndex ]<<"\n";
    } );
  }

  void deserializeVTS( const std::string& fileName )
  {
    std::fstream file;
    file.open( fileName, std::ios_base::in | std::ios_base::binary );

    file>>m_nv;
    onNumVerticesDeserialized();

    std::for_each( beginVertexIterator(), endVertexIterator(), [ this, &file ] ( VIndex vIndex ) {
      deserializeVertexData( vIndex, file );
    } );

    file>>m_nt;
    m_nc = 3*m_nt;
    onNumCornersDeserialized();

    std::for_each( beginCornerIterator(), endCornerIterator(), [ this, &file ] ( const CIndex& cIndex ) {
      file>>m_VTable[ cIndex ]>>m_OTable[ cIndex ];
    } );

    computeBox();
    populateAuxMembers();
  }
#pragma endregion LOADING AND SAVING

#pragma region DISPLAY
  virtual bool displayCorner( CIndex corner ) const 
  { 
    if ( m_cm.size() == 0 ) 
      return false;
    if ( (*m_pColorMap)[ m_cm[ corner ] ] == COLORS::NONE )
      return false; 
    return true;
  }

  virtual bool displayVertex( VIndex vIndex ) const
  {
    if ( m_vm.size() == 0 )
      return false;
    if ( (*m_pColorMap)[ m_vm[ vIndex ] ] == COLORS::NONE )
      return false;
    return true;
  }

  void draw() override
  {
    glColor3f( 0.0, 1.0, 0.0 );

    if ( m_fDrawPlane )
    {
      glBegin( GL_TRIANGLES );
        glVertex3f( m_boundingBox.low().x() - 10000, m_boundingBox.low().y() - 10000, m_boundingBox.low().z() );
        glVertex3f( m_boundingBox.low().x() - 10000, m_boundingBox.high().y() + 10000, m_boundingBox.low().z() );
        glVertex3f( m_boundingBox.high().x() + 10000, m_boundingBox.high().y() + 10000, m_boundingBox.low().z() );

        glVertex3f( m_boundingBox.low().x() - 10000, m_boundingBox.low().y() - 10000, m_boundingBox.low().z() );
        glVertex3f( m_boundingBox.high().x() + 10000, m_boundingBox.high().y() + 10000, m_boundingBox.low().z() );
        glVertex3f( m_boundingBox.high().x() + 10000, m_boundingBox.low().y() - 10000, m_boundingBox.low().z() );
      glEnd();
    }

    // First draw back triangles unshaded
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDisable(GL_LIGHTING);
    //Draw triangles
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnable( GL_COLOR_MATERIAL );

    glBindBuffer( GL_ARRAY_BUFFER, m_vertexVBO );
    glVertexPointer(3, GL_FLOAT, 0, 0);
    
    glBindBuffer( GL_ARRAY_BUFFER, m_normalVBO );
    glNormalPointer(GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, m_colorVBO );
    glColorPointer( 4, GL_UNSIGNED_BYTE , 0, 0);

    glDrawArrays( GL_TRIANGLES, 0, 3 * m_nc );

    glBindBuffer( GL_ARRAY_BUFFER, 0);
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );

    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);

    //Draw shaded triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnable( GL_COLOR_MATERIAL );

    glBindBuffer( GL_ARRAY_BUFFER, m_vertexVBO );
    glVertexPointer(3, GL_FLOAT, 0, 0);
    
    glBindBuffer( GL_ARRAY_BUFFER, m_normalVBO );
    glNormalPointer(GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, m_colorVBO );
    glColorPointer( 4, GL_UNSIGNED_BYTE , 0, 0);

    glDrawArrays( GL_TRIANGLES, 0, 3 * m_nc );

    glBindBuffer( GL_ARRAY_BUFFER, 0);
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );

    glDisable(GL_CULL_FACE);

    //Draw edges
    if ( m_fShowEdges )
    {
      glColor3f( 0, 0, 0 );
      glEnableClientState( GL_VERTEX_ARRAY );
      glBindBuffer( GL_ARRAY_BUFFER, m_edgeVBO );
      glVertexPointer(3, GL_FLOAT, 0, 0 );
      glDrawArrays( GL_LINES, 0, 6 * m_nc );
      glBindBuffer( GL_ARRAY_BUFFER, 0 );
      glDisableClientState( GL_VERTEX_ARRAY );
    }

    if ( m_fShowCorners )
    {
      std::for_each( cBeginCornerIterator(), cEndCornerIterator(), [ this ] ( const CIndex& cIndex ) {
        if ( displayCorner( cIndex ) )
        {
          uint32_t value =  (unsigned int)(*(m_pColorMap))[ m_cm[ cIndex ] ];
          uint8_t b = (value >> 8) & 0xFF;
          uint8_t g = (value >> 16) & 0xFF;
          uint8_t r = (value >> 24) & 0xFF;
          glColor3ub( r, g, b );
          const Point<U>& point = offsetPointForCorner( cIndex );
          glTranslatef( point.x(), point.y(), point.z() );
          gluSphere( m_pCornerQuadric.getRaw(), 5, 5, 5 );
          glTranslatef( -point.x(), -point.y(), -point.z() );
        }
      } );
    }

    if ( m_fShowVertices )
    {
      std::for_each( cBeginVertexIterator(), cEndVertexIterator(), [ this ] ( const VIndex& vIndex ) {
        if ( displayVertex( vIndex ) )
        {
          uint32_t value =  (unsigned int)(*(m_pColorMap))[ m_vm[ vIndex ] ];
          uint8_t b = (value >> 8) & 0xFF;
          uint8_t g = (value >> 16) & 0xFF;
          uint8_t r = (value >> 24) & 0xFF;
          glColor3ub( r, g, b );
          const Point<U>& point = m_GTable[ vIndex ];
          glTranslatef( point.x(), point.y(), point.z() );
          gluSphere( m_pVertexQuadric.getRaw(), 5, 5, 5 );
          glTranslatef( -point.x(), -point.y(), -point.z() );
        }
      } );
    }

    if ( m_fShowNormals )
    {
      std::for_each( cBeginCornerIterator(), cEndCornerIterator(), [ this ] ( const CIndex& cIndex ) {
        DrawArrow2D( g(cIndex), vNormal(cIndex), 10 );
      } );
    }
  }

  void updateColorsVBO()
  {
    std::vector<uint8_t> col( 4*m_nc );
    std::for_each( beginCornerIterator(), endCornerIterator(), [this, &col]( const CIndex& cIndex ) {
      uint32_t value =  (unsigned int)(*(m_pColorMap))[ m_tm[ t( cIndex ) ] ];
      uint8_t lolo = (value >> 0) & 0xFF;
      uint8_t lohi = (value >> 8) & 0xFF;
      uint8_t hilo = (value >> 16) & 0xFF;
      uint8_t hihi = (value >> 24) & 0xFF;
      col[ 4*cIndex ] = hihi;
      col[ 4*cIndex+1 ] = hilo;
      col[ 4*cIndex+2 ] = lohi;
      col[ 4*cIndex+3 ] = lolo;
    } );

    glBindBuffer( GL_ARRAY_BUFFER, m_colorVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( uint8_t ) * col.size(), col.data(), GL_DYNAMIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }

  void updateGeometryVBO( int typeMesh = 0 )//0 static, 1 dynamic
  {
    std::vector< U > geometry;
    std::vector< U > normals;
    std::for_each ( beginCornerIterator(), endCornerIterator(), [this, &geometry] ( const CIndex& corner ) {
      const Point<U>& currentPoint = g( corner );
      geometry.push_back( currentPoint.x() );
      geometry.push_back( currentPoint.y() );
      geometry.push_back( currentPoint.z() );
    } );

    std::for_each ( beginCornerIterator(), endCornerIterator(), [this, &normals] ( const CIndex& corner ) {
      const Vector<U>& currentNormal = vNormal( corner );
      normals.push_back( currentNormal.x() );
      normals.push_back( currentNormal.y() );
      normals.push_back( currentNormal.z() );
    } );

    std::vector< U > edgeGeometry;
    std::for_each ( beginCornerIterator(), endCornerIterator(), [this, &edgeGeometry] ( const CIndex& corner ) {
      const Point<U>& currentPoint = g( corner );
      edgeGeometry.push_back( currentPoint.x() );
      edgeGeometry.push_back( currentPoint.y() );
      edgeGeometry.push_back( currentPoint.z() );

      CIndex offsetForNext = CIndex(1);
      if ( (corner+1)%3 == CIndex(0) )
      {
        offsetForNext = CIndex(-2);
      }
      const Point<U>& nextPoint = g( CIndex(corner + offsetForNext) );
      edgeGeometry.push_back( nextPoint.x() );
      edgeGeometry.push_back( nextPoint.y() );
      edgeGeometry.push_back( nextPoint.z() );
    } );

    glBindBuffer( GL_ARRAY_BUFFER, m_vertexVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( U ) * geometry.size(), geometry.data(), typeMesh == 0 ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, m_normalVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( U ) * normals.size(), normals.data(), typeMesh == 0 ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, m_edgeVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( U ) * edgeGeometry.size(), edgeGeometry.data(), typeMesh == 0 ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }

  void initVBO( int typeMesh )
  {
    glGenBuffers( 1, &m_vertexVBO );
    glGenBuffers( 1, &m_edgeVBO );
    glGenBuffers( 1, &m_colorVBO );
    glGenBuffers( 1, &m_normalVBO );

    updateGeometryVBO( typeMesh );
    updateColorsVBO();
  }
#pragma endregion DISPLAY

#pragma region HELPERS
  int getValence( CIndex corner ) const
  {
    int valence = 0;
    std::for_each( cBeginSwingIterator( corner ), cEndSwingIterator( corner ), [&valence]( const CIndex& /*cIndex*/ ) { valence++; } );
    return valence;
  }

  Point< U > offsetPointForCorner( CIndex corner ) const
  {
    return Point< U >( m_GTable [ v( corner ) ], 0.2f, centroid( t( corner ) ) );
  }

#pragma endregion HELPERS

#pragma region CONTROL
  int onMousePressed(int mouseX, int mouseY, const IKeyboardState& keyboardState) override { return 0; }
  int onMouseReleased(int mouseX, int mouseY, const IKeyboardState& keyboardState) override{ return 0; }
  int onMouseDragged(int mouseX, int mouseY, const IKeyboardState& keyboardState) override { return 0; }
  int onMouseMoved(int deltaX, int deltaY, const IKeyboardState& keyboardState) override { return 0; }
  int onClick(int mouseX, int mouseY, const IKeyboardState& keyboardState) override { return 0; }
  int onKeyReleased( int ch ) override { return 0; }

  //TODO msati3: This has a dependency on FLTK right now. Can we remove this without too much effort?
  int onKeyPressed( int ch ) override
  {
    bool fHandled = true;
    switch( ch )
    {
      case 'h': selectCorner( unprojectPoint( Fl::event_x(), Fl::event_y() ) );
        break;
      case 'n': setSelectedCorner( n( m_selectedCorner ) );
        break;
      case 'p': setSelectedCorner( p( m_selectedCorner ) );
        break;
      case 'o': setSelectedCorner( o( m_selectedCorner ) );
        break;
      case 'l': setSelectedCorner( l( m_selectedCorner ) );
        break;
      case 'r': setSelectedCorner( r( m_selectedCorner ) );
        break;
      case 's': setSelectedCorner( s( m_selectedCorner ) );
        break;
      case 'u': setSelectedCorner( u( m_selectedCorner ) );
        break;
      case 'v': m_fShowVertices = !m_fShowVertices;
        break;
      case 'c': m_fShowCorners = !m_fShowCorners;
        if ( !m_fShowCorners )
        {
          m_cm.resize(0);
          m_cm.shrink_to_fit();
        }
        break;
      case 'e': m_fShowEdges = !m_fShowEdges;
        break;
      case FL_Up: 
        break;
      default: fHandled = false;
        break;
    }
    return fHandled ? 1 : 0;
  }

  int onCommand( const std::string& command ) override
  {
    return interpretCommand( command );
  }
#pragma endregion CONTROL

#pragma region CONTROL_HELPERS
private:
  void selectCorner( const Point<U> point )
  {
    CIndex candidateNewCorner = m_selectedCorner == -1 ? CIndex( 0 ) : m_selectedCorner;
    U minDistance = std::numeric_limits<U>::max();
    std::for_each( beginCornerIterator(), endCornerIterator(), [ this, &point, &candidateNewCorner, &minDistance ]( const CIndex& cIndex ) {
      Point<U> ctr = centroid( t( cIndex ) );
      Point<U> shifted = Point<U>( g( cIndex ), 0.33f, ctr );
      U currentDistance = point.distance( shifted );
        if ( currentDistance < minDistance ) {
          candidateNewCorner = cIndex; 
          minDistance = currentDistance;
        }
    } );
    setSelectedCorner( candidateNewCorner );
    std::stringstream logStatement;
    logStatement<<"Corner picked : "<<m_selectedCorner<<"  vertex: "<<v( m_selectedCorner )<<" vertex location "<<m_GTable[ v( m_selectedCorner ) ];
    LOG_NO_DECORATIONS( logStatement.str(), DEBUG_LEVELS::LOW );
  }

  int interpretCommand( const std::string& command )
  {
    int handled = 1;
    switch( command[0] )
    {
      case 'c': setSelectedCorner( (CIndex)( strtoT<T>( command.c_str() + 1, nullptr ) ) );
        break;
      case 'v': {
          VIndex vertex = (VIndex)strtoT<T>( command.c_str() + 1, nullptr );
          auto iter = std::find_if( beginCornerIterator(), endCornerIterator(), [this, &vertex]( CIndex index ) {
            return ( v( index ) == vertex );
          } );
          setSelectedCorner( *iter );
        }
        break;
      default: handled = 0;
        break;
    }
    return handled;
  }
#pragma endregion PRIVATE_HELPERS

#pragma region MESH_ALGORITHMS
private:
  void zipAdjacent( CIndex c1, CIndex c2 ) //Zip two adjacent triangles
  {
    assert( c1 == o( c2 ) );
    CIndex l1 = l( c1 );
    CIndex l2 = l( c2 );
    CIndex r1 = r( c1 );
    CIndex r2 = r( c2 );
    setOpposites( l1, r1 );
    setOpposites( l2, r2 );
  }

  void addVertex( const Point<U>& p )
  {
    m_GTable.emplace_back( p );
    m_vm.push_back( 0 );
    m_fVRemoved.push_back( false );
    m_nv++;
  }

  void replaceVertex( VIndex vIndex, const Point<U>& newVertex )
  {
    m_GTable[ vIndex ] = newVertex;
  }

  void addTriangle( VIndex v1, VIndex v2, VIndex v3 )
  {
    m_VTable.push_back( v1 );
    m_VTable.push_back( v2 );
    m_VTable.push_back( v3 );
    m_OTable.resize( m_OTable.size() + 3 );
    m_tm.push_back( 0 );
    m_nt++;
    m_nc += 3;
  }

  void removeTriangle( CIndex corner )
  {
    TIndex toTIndex = t( corner );
    TIndex fromTIndex = TIndex( nt() - 1 );
    if ( toTIndex != fromTIndex )
    {
      CIndex initToCIndex = c( toTIndex );
      CIndex initFromCIndex = c( fromTIndex );
      Next_iterator fromCIndexIterator = beginNextIterator( initFromCIndex );
      std::for_each( beginNextIterator( initToCIndex ), endNextIterator( initToCIndex ), [ this, &fromCIndexIterator ] ( const CIndex& toCIndex ) {
        setOpposites( toCIndex, o( *fromCIndexIterator ) );
        setVTable( toCIndex, v( *fromCIndexIterator ) );
        fromCIndexIterator++;
      } );

      m_tm[ toTIndex ] = m_tm[ fromTIndex ];

      notifyTIndexChange( fromTIndex, toTIndex );
    }

    m_nt--;
    m_nc-=3;
  }

  void removeVertex( VIndex fromVIndex )
  {
    VIndex toVIndex = VIndex(-1);
    m_fVRemoved[ fromVIndex ] = true;
    notifyVIndexChange( fromVIndex, toVIndex );
  }

  void moveVertex( VIndex oldIndex, VIndex newIndex )
  {
    setGTable( newIndex, m_GTable[ oldIndex ] );
    notifyVIndexChange( oldIndex, newIndex );
  }

  void compressVTable()
  {
    assert( m_fVRemoved.size() == (int)nv() );
    VIndex newIndex = VIndex(0);
    std::vector< VIndex > vToCompressedVMap;
    vToCompressedVMap.resize( nv() );

    //Compress the m_GTable, populate mapping from V index to compressed V index
    std::for_each( beginVertexIterator(), endVertexIterator(), [ this, &newIndex, &vToCompressedVMap ] ( const VIndex& vIndex ) {
      if ( !m_fVRemoved[ vIndex ] )
      {
        vToCompressedVMap[ vIndex ] = newIndex;
        moveVertex( vIndex, newIndex );
        newIndex++;
      }
    } );

    //Use the mapping populated above to change the V table in accordance with the new indices
    std::for_each( beginCornerIterator(), endCornerIterator(), [ this, &vToCompressedVMap ] ( const CIndex& cIndex ) {
      setVTable( cIndex, vToCompressedVMap[ v( cIndex ) ] );
    } );

    m_nv = newIndex;
    m_fVRemoved.assign( m_nv, false );
  }

public:
  void reclaimMemory()
  {
    compressVTable();

#if _DEBUG
    std::for_each( beginCornerIterator(), endCornerIterator(), [ this ] ( const CIndex& cIndex ) {
      assert( v( cIndex ) < m_nv );
      assert( o( cIndex ) < m_nc );
    } );
#endif

    m_tm.resize( m_nt );
    m_OTable.resize( m_nc );
    m_VTable.resize( m_nc );
    m_GTable.resize( m_nv );
    m_vm.resize( m_nv );

    m_tm.shrink_to_fit();
    m_OTable.shrink_to_fit();
    m_VTable.shrink_to_fit();
    m_GTable.shrink_to_fit();
    m_vm.shrink_to_fit();
  }

  //Make it so that this cIndex is moved to c( t ( cIndex ) ) -- cyclically rotate
  //cIndex => 0, (cIndex+1)%3 => 1, (cIndex+2)%3 => 2
  //This is a cyclic permutation => cIndex -> 0, 0 -> offset, offset -> (offset + offset)%3, etc
  void shiftTriangleCorners( CIndex cNewStartIndex )
  {
    CIndex leftShiftAmount = CIndex( cNewStartIndex % 3 );
    if ( leftShiftAmount == 0 )
    {
      return;
    }
    CIndex initCorner = c ( t ( cNewStartIndex ) );
    cyclicallyPermute<VIndex>( &m_VTable[ initCorner ], 3, leftShiftAmount );
    cyclicallyPermute<CIndex>( &m_OTable[ initCorner ], 3, leftShiftAmount );
    std::for_each( beginNextIterator( initCorner ), endNextIterator( initCorner ), [ this ] ( const CIndex& cIndex ) {
      m_OTable[ m_OTable[ cIndex ] ] = cIndex;
    } );
  }

  void triangleExpandOperation( const std::vector< CIndex >& expansionCorners, const std::vector< Point< U > >& expandedGeometry,
                                  std::vector< VIndex >& addedVIndices, std::vector< TIndex >& addedTIndices )
  {
    expandVertex( expansionCorners[1], expansionCorners[2], expandedGeometry[1], expandedGeometry[2], true );
    expandVertex( expansionCorners[0], s( expansionCorners[1] ), expandedGeometry[0], expandedGeometry[1], false );

    addedVIndices.push_back( v( expansionCorners[ 0 ] ) );
    addedVIndices.push_back( VIndex( m_nv - 1 ) );
    addedVIndices.push_back( VIndex( m_nv - 2 ) );

    addedTIndices.push_back( TIndex( m_nt - 1 ) );
    addedTIndices.push_back( TIndex( m_nt - 2 ) );
    addedTIndices.push_back( TIndex( m_nt - 4 ) );
    addedTIndices.push_back( TIndex( m_nt - 3 ) );
  }

  //Expand the vertex given by corner1 and corner2. After expanding, corners from s(corner1) to corner2 are incident on geom2. The rest on geom1
  VIndex expandVertex( CIndex corner1, CIndex corner2, const Point<U>& geom1, const Point<U>& geom2, bool fAddTriangles )
  {
    assert( v( corner1 ) == v( corner2 ) );
    replaceVertex( v( corner1 ), geom1 );
    addVertex( geom2 );

    for( auto iter = beginSwingIterator( s( corner1 ) ); iter != endSwingIterator( s( corner2 ) ); iter++ )
    {
      setVTable( *iter, VIndex( m_nv - 1 ) );
    }

    CIndex offsetCorner = m_nc;
    addTriangle( v( corner1 ), v( p( corner1 ) ), v( corner2 ) );

    if ( fAddTriangles )
      addTriangle( v( corner2 ), v( p( corner2 ) ), v( corner1 ) );
    else
      addTriangle( v( corner1 ), v( corner2 ), v( p( corner2 ) ) );

    setOpposites( p( s( corner1 ) ), CIndex( offsetCorner ) );
    setOpposites( n( corner1 ), CIndex( offsetCorner + 2 ) );

    if ( fAddTriangles )
    {
      setOpposites( p( s( corner2 ) ), CIndex( offsetCorner + 3 ) );
      setOpposites( n( corner2 ), CIndex( offsetCorner + 5 ) );
      setOpposites( CIndex( offsetCorner + 1 ), CIndex( offsetCorner + 4 ) );
    }
    else
    {
      setOpposites( p( s( corner2 ) ), CIndex( offsetCorner + 4 ) );
      setOpposites( n( corner2 ), CIndex( offsetCorner + 3 ) );
      setOpposites( CIndex( offsetCorner + 1 ), CIndex( offsetCorner + 5 ) );
    }

    return VIndex( m_nv - 1 );
  }

  class LRTriangleIndexChangeHandler
  {
  private:
    TIndex m_lTriangle;
    TIndex m_rTriangle;

  public:
    LRTriangleIndexChangeHandler( TIndex lTriangle, TIndex rTriangle ) : m_lTriangle( lTriangle ), m_rTriangle( rTriangle ) {}
    void onTriangleIndexChanged( TIndex oldIndex, TIndex newIndex )
    {
      if ( oldIndex == m_lTriangle ) 
      {
        m_lTriangle = newIndex; 
      }
      else if ( oldIndex == m_rTriangle ) 
      { 
        m_rTriangle = newIndex; 
      }
    }
    TIndex lTriangle() { return m_lTriangle; }
    TIndex rTriangle() { return m_rTriangle; }
  };

  VIndex collapseTriangle( CIndex corner, const Point<U>& pointAfterCollapse )
  {
    CIndex opposite = o( corner );
    CIndex cLIndex = l( corner );
    CIndex cRIndex = r( corner );
    TIndex tLIndex = t( cLIndex );
    TIndex tRIndex = t( cRIndex );
    CIndex cLOffset = offset( cLIndex );
    CIndex cROffset = offset( cRIndex );

    LRTriangleIndexChangeHandler lrTriangleChangeHandler( tLIndex, tRIndex );
    std::function< void( typename Mesh<T,U>::TIndex&, typename Mesh<T,U>::TIndex& ) > triangleIndexChangeCallback( std::bind( &LRTriangleIndexChangeHandler::onTriangleIndexChanged, &lrTriangleChangeHandler, std::tr1::placeholders::_1, std::tr1::placeholders::_2 ) );
    auto iterTriangleIndexChangeCallback = registerForTriangleMoveOperation( triangleIndexChangeCallback );

    Mesh<T,U>::VIndex simplifiedVIndex = collapseEdge( corner, opposite, pointAfterCollapse );
    unregisterForTriangleMoveOperation( iterTriangleIndexChangeCallback );
    simplifiedVIndex = collapseEdge( Mesh<T,U>::CIndex( c( lrTriangleChangeHandler.lTriangle() ) + cLOffset), Mesh<T,U>::CIndex( c( lrTriangleChangeHandler.rTriangle() ) + cROffset), pointAfterCollapse );

    return simplifiedVIndex;
  }

  VIndex collapseEdge( CIndex corner, CIndex oppositeCorner, const Point<U>& pointAfterCollapse )
  {
    assert( corner == o( oppositeCorner ) );

    CIndex cEdge1 = n( corner );
    CIndex cEdge2 = p( corner );
    VIndex vEdge1 = v( n( corner ) );
    VIndex vEdge2 = v( p( corner ) );

    std::for_each( beginSwingIterator( cEdge2 ), endSwingIterator( cEdge2 ), [ this, &vEdge1 ] ( const CIndex& cIndex ) {
      setVTable( cIndex, vEdge1 );
    } );

    zipAdjacent( corner, oppositeCorner );
    setGTable( vEdge1, pointAfterCollapse );

    removeVertex( vEdge2 );

    //Invalidate the o table of the triangle that is to be removed later. This prevents overwriting of m_oppositeTable of the opposites when the oppositeCorner triangle is the last triangle
    std::array< CIndex, 2 > corners = { corner, oppositeCorner };
    std::for_each( corners.begin(), corners.end(), [ this ] ( const CIndex& corner ) {
      std::for_each( beginNextIterator( corner ), endNextIterator( corner ), [ this ] ( const CIndex& cIndex ) {
        setOTable( cIndex, CIndex( -1 ) );
      } );
    } );

    removeTriangle( oppositeCorner > corner ? oppositeCorner : corner );
    removeTriangle( oppositeCorner < corner ? oppositeCorner : corner );
    return vEdge1;
  }

#pragma endregion MESH_ALGORITHMS

#pragma region NOTIFICATIONS
public:
  typename std::list< std::function< void( CIndex&, CIndex& ) > >::const_iterator registerForCornerSelectionChangeOperation( std::function< void ( CIndex&, CIndex& ) > function )
  {
    return m_selectedCornerChangeOperationNotifiers.insert( m_selectedCornerChangeOperationNotifiers.end(), function );
  }

  void unregisterForCornerSelectionChangeOperation( typename std::list< std::function< void( CIndex&, CIndex& ) > >::const_iterator& functionIter )
  {
    m_selectedCornerChangeOperationNotifiers.erase( functionIter );
  }

  typename std::list< std::function< void( VIndex&, VIndex& ) > >::const_iterator registerForVertexMoveOperation( std::function< void ( VIndex&, VIndex& ) > function )
  {
    return m_vertexMoveOperationNotifiers.insert( m_vertexMoveOperationNotifiers.end(), function );
  }

  void unregisterForVertexMoveOperation( typename std::list< std::function< void( VIndex&, VIndex& ) > >::const_iterator& functionIter )
  {
    m_vertexMoveOperationNotifiers.erase( functionIter );
  }

  typename std::list< std::function< void( TIndex&, TIndex& ) > >::const_iterator registerForTriangleMoveOperation( std::function< void ( TIndex&, TIndex& ) > function )
  {
    return m_triangleMoveOperationNotifiers.insert( m_triangleMoveOperationNotifiers.end(), function );
  }

  void unregisterForTriangleMoveOperation( typename std::list< std::function< void( TIndex&, TIndex& ) > >::const_iterator& functionIter )
  {
    m_triangleMoveOperationNotifiers.erase( functionIter );
  }

private:
  void notifyVIndexChange( VIndex oldIndex, VIndex newIndex )
  {
    std::for_each( m_vertexMoveOperationNotifiers.begin(), m_vertexMoveOperationNotifiers.end(), [ &oldIndex, &newIndex ] ( std::function< void ( VIndex&, VIndex& ) > function ) {
      function( oldIndex, newIndex );
    } );
  }

  void notifyTIndexChange( TIndex oldIndex, TIndex newIndex )
  {
    std::for_each( m_triangleMoveOperationNotifiers.begin(), m_triangleMoveOperationNotifiers.end(), [ &oldIndex, &newIndex ] ( std::function< void (TIndex&, TIndex& ) > function ) {
      function( oldIndex, newIndex );
    } );
  }

  void notifySelectedCornerChange( CIndex oldCorner, CIndex newCorner )
  {
    std::for_each( m_selectedCornerChangeOperationNotifiers.begin(), m_selectedCornerChangeOperationNotifiers.end(), [ &oldCorner, &newCorner ] ( std::function< void (CIndex&, CIndex& ) > function ) {
      function( oldCorner, newCorner );
    } );
  }

private:
  std::list< std::function< void( TIndex&, TIndex& ) > > m_triangleMoveOperationNotifiers; //Must use a container that doesn't invalidate iterators
  std::list< std::function< void( VIndex&, VIndex& ) > > m_vertexMoveOperationNotifiers; //Must use a container that doesn't invalidate iterators
  std::list< std::function< void( CIndex&, CIndex& ) > > m_selectedCornerChangeOperationNotifiers; //Must use a container that doesn't invalidate iterators

#pragma endregion NOTIFICATIONS

#pragma region STATSFINDING
public:
  void findValenceHistogram()
  {
    std::vector<int> valence;
    valence.assign( m_nv, 0 );
    std::for_each( beginCornerIterator(), endCornerIterator(), [ this, &valence ] ( CIndex cIndex ) {
      valence[v(cIndex)]++;
    } );
    std::map< int, int > histogram = computeHistogram<int>(valence);
    for ( std::map< int, int >::iterator iter = histogram.begin(); iter != histogram.end(); iter++ )
    {
      std::cout<<iter->first<<" "<<iter->second<<"\n";
    }
  }

#pragma endregion STATSFINDING

#pragma region DEBUG
public:
  void colorTriangles( const std::vector< TIndex >& triangleList, COLORS color )
  {
    unsigned int colorIndex = m_pColorMap->getIndexForColor( color );
    std::for_each( triangleList.begin(), triangleList.end(), [ this, &colorIndex ] ( TIndex tIndex ) {
      m_tm[ tIndex ] = colorIndex;
    } );
  }

  void colorVertices( const std::vector< VIndex >& vertexList, COLORS color )
  {
    m_fShowVertices = true;
    if ( m_vm.size() == 0 )
    {
      unsigned int colorIndex = m_pColorMap->getIndexForColor( COLORS::NONE );
      m_vm.assign( m_nv, colorIndex );
    }
    unsigned int colorIndex = m_pColorMap->getIndexForColor( color );
    std::for_each( vertexList.begin(), vertexList.end(), [ this, &colorIndex ] ( VIndex vIndex ) {
      m_vm[ vIndex ] = colorIndex;
    } );
  }

  void colorCorners( const std::vector< CIndex >& cornerList, COLORS color )
  {
    m_fShowCorners = true;
    if ( m_cm.size() == 0 )
    {
      unsigned int colorIndex = m_pColorMap->getIndexForColor( COLORS::NONE );
      m_cm.assign( m_nc, colorIndex );
    }
    unsigned int colorIndex = m_pColorMap->getIndexForColor( color );
    std::for_each( cornerList.begin(), cornerList.end(), [ this, &colorIndex ] ( CIndex cIndex ) {
      m_cm[ cIndex ] = colorIndex;
    } );
  }
#pragma endregion DEBUG
};

//Helpers for hashing mesh types
//#ifdef _DEBUG
namespace std {
  template <>
  struct hash< Mesh< int, float >::VIndex >
  {
    std::size_t operator()( const Mesh< int, float >::VIndex& vIndex ) const
    {
      std::hash< int > intHasher;
      return intHasher( (int)vIndex );
    }
  };

  template <>
  class numeric_limits< Mesh< int, float >::TIndex > : public numeric_limits< int >
  {
  };

  template <>
  class numeric_limits< Mesh< int, float >::VIndex > : public numeric_limits< int >
  {
  };

  template <>
  class numeric_limits< Mesh< int, float >::CIndex > : public numeric_limits< int >
  {
  };
}
//#endif

#endif//_MESH_H_