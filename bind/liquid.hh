#ifndef __LIQUID_HH__
#define __LIQUID_HH__

#include <complex>
#include <iostream>
#include "liquid.h"

namespace liquid {

class object {
  public:
    object() {};
    virtual ~object() {};
    virtual void reset()=0;
    virtual std::string repr() const
        { return std::string("<liquid.object>"); }
  protected:
    std::string type;
};

static std::ostream& operator<<(std::ostream& _os, const object& _object)
{
    _os << _object.repr();
    return _os;
}

} // namespace liquid

#endif // __LIQUID_HH__

