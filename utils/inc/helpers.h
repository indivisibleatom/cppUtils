#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <cassert>
#include <map>
#include <thread>
#include <chrono>

const float PI = 3.14159265359f;

template <class T> T strtoT( const char* str, char **endPgettr );

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

template <class T, class U> 
T quantizeValue( U value, U minValue, U maxValue, int numBits );

template <class T, class U>
T quantizeValue( U value, U minValue, int numBits );

template <class T>
T deltaEncode( T value, T prevValue, std::function< T( const T&, const T &) > diffingFunc )
{
  return diffingFunc( value, prevValue );
}

template <class T>
T clamp( T value, T minValue, T maxValue )
{
  if ( value < minValue )
    return minValue;
  if ( value > maxValue )
    return maxValue;
  return value;
}

template <class T, class U>
void populateReverseMap( std::vector< U >& outputMap, const std::vector< T >& inputMap, U defaultOutputValue, T defaultInputValue, std::size_t sizeOutputMap )
{
  outputMap.assign( sizeOutputMap, defaultOutputValue );
  U currentCount = U(0);

  std::for_each( inputMap.cbegin(), inputMap.cend(), [ &outputMap, &currentCount, &sizeOutputMap, &defaultInputValue, &defaultOutputValue ] ( const T& inputMapElement ) {
    if ( inputMapElement != defaultInputValue )
    {
      assert( inputMapElement <= T(sizeOutputMap) );
      if ( outputMap[ inputMapElement ] == defaultOutputValue )
      {
        outputMap[ inputMapElement ] = currentCount;
      }
    }
    currentCount++;
  } );
}

template <class T>
void cyclicallyPermute( T* pData, std::size_t size, int leftShiftAmount )
{
  if ( leftShiftAmount == 0 ) 
  {
    return;
  }
  int nextIndex = (size - leftShiftAmount) % size;
  int currentIndex = 0;

  T prevData = pData[ currentIndex ];
  T tempData;

  for ( size_t i = 0; i < size; i++ )
  {
    tempData = pData[ nextIndex ];
    pData[nextIndex] = prevData;
    
    prevData = tempData;

    currentIndex = nextIndex;
    nextIndex = (nextIndex - leftShiftAmount + size) % size;
  }
}

template <class T>
std::map<T, int> computeHistogram( const std::vector<T>& values )
{
  std::map<T,int> retVal;
  std::for_each( values.begin(), values.end(), [ &retVal ]( T value ) {
    retVal[ value ]++;
  } );
  return retVal;
}

enum class COLORS : unsigned int
{
  NONE = 0x00000000,
  WHITE = 0xffffffff,
  RED = 0xff0000ff,
  GREEN = 0x00ff00ff,
  BLUE = 0x0000ffff,
  YELLOW = 0xffff00ff,
  CYAN = 0x00ffffff,
  MAGENTA = 0xff00ffff,
  GRAY = 0x808080ff,
  LIGHT_PINK = 0xff99ffff,
  BLACK = 0x000000ff
};

namespace Colors
{
  inline unsigned int R( COLORS color ) { return (static_cast<unsigned int>(color) >> 24) & 0xff; }
  inline unsigned int G( COLORS color ) { return (static_cast<unsigned int>(color) >> 16) & 0xff; }
  inline unsigned int B( COLORS color ) { return (static_cast<unsigned int>(color) >> 8) & 0xff; }
  inline unsigned int A( COLORS color ) { return static_cast<unsigned int>(color) & 0xff; }
}

//Set that can at max contain a bounded max element. Uses arrays for faster checking of containment
template < class T, class U, class Allocator = std::allocator<T> >
class MaxElementHoldingSet
{
private:
  std::vector< bool > m_fAlreadyContained;
  std::vector< T > m_container;
  U m_maxElement;

public:
  typedef Allocator allocator_type;
  typedef typename Allocator::size_type size_type;
  typedef typename Allocator::difference_type  difference_type;
  typedef typename Allocator::reference reference;
  typedef typename Allocator::const_reference 
                                const_reference;

  // Our iterator will be a simple pointer
  typedef typename std::vector< T >::iterator iterator;
  typedef typename std::vector< T >::const_iterator const_iterator;

  iterator begin() { return m_container.begin(); }
  iterator end() { return m_container.end(); }
  const_iterator cbegin() const { return m_container.cbegin(); }
  const_iterator cend() const { return m_container.cend(); }
  const_iterator& operator++() { return m_container.operator()++; }

  explicit MaxElementHoldingSet( U maxSize )
  {
    m_maxElement = maxSize;
    m_fAlreadyContained.assign( maxSize, false );
  }

  bool insert( const T& element, U key )
  {
    assert( key < m_maxElement );
    if ( !m_fAlreadyContained[ key ] )
    {
      m_container.push_back( element );
      m_fAlreadyContained[ key ] = true;
      return true;
    }
    return false;
  }

  const std::vector< T >& getElements() const throw()
  {
    return m_container;
  }

  U maxValue() const throw() { return m_maxElement; }

  void increaseSize( U sizeIncrement )
  {
    m_maxElement += U(sizeIncrement);
    m_fAlreadyContained.insert( m_fAlreadyContained.end(), sizeIncrement, false );
  }

  void reset( U newSize )
  {
    m_maxElement = newSize;
    clear();
  }

  void clear()
  {
    m_fAlreadyContained.assign( m_maxElement, false );
    m_container.clear();
  }
};

template <class T>
void transposeMatrix( T* pMatrix, int dim )
{
  for (int i = 1; i < dim; i++)
  {
    for (int j = 0; j < i; j++ )
    {
      std::swap( pMatrix[i*dim + j], pMatrix[j*dim + i] );
    }
  }
}

bool replace(std::string& str, const std::string& from, const std::string& to);

template <typename Function>
void callAfter(long millis, Function const & function)
{
  std::thread([millis,function](){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    function();
  }).detach();
}

#endif//_HELPERS_H_