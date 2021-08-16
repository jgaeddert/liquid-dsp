#ifndef __LIQUID_HH__
#define __LIQUID_HH__

#include <complex>
#include <iostream>
#include "liquid.h"

namespace liquid {

class obj {
  public:
    obj() {};
    virtual ~obj() {};
    virtual void reset()=0;
    virtual std::string repr() const { return std::string("<liquid.obj>"); };
};

static std::ostream& operator<<(std::ostream& _os, const obj& _obj)
{
    _os << _obj.repr();
    return _os;
}

} // namespace liquid

#endif // __LIQUID_HH__

