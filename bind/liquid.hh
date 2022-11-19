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
    virtual std::string type() const =0;
    virtual std::string repr() const
        { return std::string("<liquid.object>"); }
  protected:
};

inline std::ostream& operator<<(std::ostream& _os, const object& _object)
{
    _os << _object.repr();
    return _os;
}

// abstract base frame synchronizer class
class framesync : public object {
  public:
    framesync() : object() {};
    virtual ~framesync() {};

    virtual void execute(std::complex<float> * _buf, unsigned int _buf_len) = 0;

    virtual void set_callback(framesync_callback _callback=NULL) = 0;

    virtual void set_userdata(void * _userdata=NULL) = 0;

    // specific frame data statistics
    virtual unsigned int get_num_frames_detected() const = 0;
    virtual unsigned int get_num_headers_valid()   const = 0;
    virtual unsigned int get_num_payloads_valid()  const = 0;
    virtual unsigned int get_num_bytes_received()  const = 0;
    virtual void         reset_framedatastats()          = 0;
    virtual framedatastats_s get_framedatastats()  const = 0;

  protected:
};

} // namespace liquid

#endif // __LIQUID_HH__

