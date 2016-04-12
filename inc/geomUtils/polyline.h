#ifndef _POLYLINE_H_
#define _POLYLINE_H_

#include <boost/iterator_adaptors.hpp>

#include "point.h"

template <typename T>
class Polyline {
 public:
  Polyline();

  Polyline& operator=(const Polyline& other);
  Polyline(const Polyline& other);
  void swap(Polyline& other);

  virtual ~Polyline() {}

  class iterator
      : public boost::iterator_adaptor<iterator,
                                       std::vector<Point<T>>::iterator> {
   public:
    iterator() : iterator::iterator_adaptor_() {}

    explicit iterator(const iterator::iterator_adaptor_::base_type_& p)
        : iterator::iterator_adaptor_(p) {}
  };

 private:
  std::vector<Point<T>> m_points;
};

// Convenience class for closed polylines
// template <class T>
// class ClosedPolyline : public Polyline<T> {};

/*class PolylineFactory {
 public:
  std::auto_ptr<Polyline> get
}*/


#endif  //_POLYLINE_H_
