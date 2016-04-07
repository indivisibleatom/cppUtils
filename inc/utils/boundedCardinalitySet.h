#ifndef _BOUNDED_CARDINALITY_SET_H_
#define _BOUNDED_CARDINALITY_SET_H_

#include <vector>

// Set that can contain a max number of elements of type T. Uses arrays for
// faster checking of containment.
template <class T, class U, class Allocator = std::allocator<T> >
class BoundedCardinalitySet {
 private:
  std::vector<bool> m_fAlreadyContained;
  std::vector<T> m_container;
  U m_maxElement;

 public:
  typedef Allocator allocator_type;
  typedef typename Allocator::size_type size_type;
  typedef typename Allocator::difference_type difference_type;
  typedef typename Allocator::reference reference;
  typedef typename Allocator::const_reference const_reference;

  // Our iterator will be a simple pointer
  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;

  iterator begin() { return m_container.begin(); }
  iterator end() { return m_container.end(); }
  const_iterator cbegin() const { return m_container.cbegin(); }
  const_iterator cend() const { return m_container.cend(); }
  const_iterator& operator++() { return m_container.operator()++; }

  BoundedCardinalitySet(U maxSize) {
    m_maxElement = maxSize;
    m_fAlreadyContained.assign(minSize + maxSize, false);
  }

  bool insert(const T& element, U key) {
    assert(key < m_maxElement);
    if (!m_fAlreadyContained[key]) {
      m_container.push_back(element);
      m_fAlreadyContained[key] = true;
      return true;
    }
    return false;
  }

  const std::vector<T>& getElements() const throw() { return m_container; }

  U maxValue() const throw() { return m_maxElement; }

  void increaseSize(U sizeIncrement) {
    m_maxElement += U(sizeIncrement);
    m_fAlreadyContained.insert(m_fAlreadyContained.end(), sizeIncrement, false);
  }
};

#endif  //_BOUNDED_CARDINALITY_SET_H_
