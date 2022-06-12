#ifndef __LIQUID_HH__
#define __LIQUID_HH__

#include <complex>
#include <iostream>
#include "liquid.h"

namespace liquid {

class object {
  public:
    object() {};

    /*! copy constructor throws exception as way to enforce run-time implementation
     *  in derived classes. Note that ideally this would be a compile-time error for
     *  all derived classes, but because this class is virtual we cannot make this
     *  method pure or virtual.
     */
    object(object &)
        { throw std::runtime_error("copy constructor not implemented for this class"); }

    virtual ~object() {};
    virtual void reset()=0;
    virtual std::string repr() const
        { return std::string("<liquid.object>"); }
  protected:
    std::string type;
};

inline std::ostream& operator<<(std::ostream& _os, const object& _object)
{
    _os << _object.repr();
    return _os;
}

} // namespace liquid

#endif // __LIQUID_HH__

