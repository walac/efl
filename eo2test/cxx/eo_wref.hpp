
///
/// @file eo_wref.hpp
///

#ifndef EO3_WREF_HPP
#define EO3_WREF_HPP

#include <boost/optional.hpp>

namespace efl { namespace eo {

/// @brief TODO
///
template<typename T>
struct wref {

  /// @brief TODO
  ///
  explicit wref(Eo* obj) : _eo_wref(obj)
  {
    _add();
  }

  /// @brief TODO
  ///
  wref(T obj) : _eo_wref(obj._eo_ptr())
  {
    _add();
  }

  /// @brief TODO
  ///
  ~wref()
  {
    if(boost::optional<T> p = lock())
      _del();
  }

  /// @brief TODO
  ///
  boost::optional<T> lock()
  {
    if(_eo_wref) // eo_ref() should work on a multi-threaded environment
      eo_ref(_eo_wref);
    else
      return boost::none;
    return T(_eo_wref);
  }

  /// @brief Copy constructor.
  ///
  wref(wref const& other)
    : _eo_wref(other._eo_wref)
  {
    if(boost::optional<T> p = lock())
      _add();
    else
      _eo_wref = 0;
  }

  /// @brief Assignment operator.
  ///
  wref& operator=(wref const& other)
  {
    _eo_wref = other._eo_wref;
    if(boost::optional<T> p = lock())
      _add();
    else
      _eo_wref = 0;
  }

private:
  void _add()
  {
    eo2_do(_eo_wref, eo2_wref_add(&_eo_wref));
  }
  void _del()
  {
    eo2_do(_eo_wref, eo2_wref_del(&_eo_wref));
  }

  Eo* _eo_wref;

//   wref(T obj)
//   {
//     _eo_wref.reset(new Eo*);
//     *_eo_wref = obj._eo_ptr();
//   }
//
//   boost::optional<T> lock()
//   {
//     eo_ref(*wref);
//     if(!*wref) 
//       return boost::none;
//     return T(*wref);
//   }
//
// private:
//   void _w(Eo* obj)
//   {
//     eo2_do(obj, eo2_wref_add(_eo_wref.get()));
//   }
//   boost::shared_ptr<Eo*> _eo_wref;
};

} } // namespace efl { namespace eo {

#endif // EO3_WREF_HPP
