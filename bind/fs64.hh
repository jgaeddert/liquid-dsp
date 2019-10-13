// framesync64 bindings
#ifndef __FS64_HH__
#define __FS64_HH__

/* TODO
 *  - simplify wrapper declaration (use function definition type)
 */

#include <complex>
#include <iostream>
#include <string>
#include "liquid.h"
#include "liquid.python.hh"

namespace liquid {

#ifdef PYTHONLIB
int py_callback_wrapper(
        unsigned char *  _header,
        int              _header_valid,
        unsigned char *  _payload,
        unsigned int     _payload_len,
        int              _payload_valid,
        framesyncstats_s _stats,
        void *           _userdata);
typedef std::function<py::object(py::array_t<uint8_t>,py::array_t<uint8_t>,py::dict)> py_framesync_callback;
//typedef std::function<int(py::dict)> py_framesync_callback;
#endif

// suppress "declared with greater visibility than the type of its field" warnings
// see: https://stackoverflow.com/questions/2828738/c-warning-declared-with-greater-visibility-than-the-type-of-its-field#3170163
// see: https://gcc.gnu.org/wiki/Visibility
#pragma GCC visibility push(hidden)
class fs64
{
  public:
    fs64(framesync_callback _callback, void * _userdata)
        { q = framesync64_create(_callback, _userdata); }

    ~fs64() { framesync64_destroy(q); }

    void display() { framesync64_print(q); }

    void reset() { framesync64_reset(q); }

    void execute(std::complex<float> * _buf, unsigned int _buf_len)
        { framesync64_execute(q, _buf, _buf_len); }

  private:
    framesync64 q;

#ifdef PYTHONLIB
  private:
    py_framesync_callback py_callback;
    friend int py_callback_wrapper(
            unsigned char *  _header,
            int              _header_valid,
            unsigned char *  _payload,
            unsigned int     _payload_len,
            int              _payload_valid,
            framesyncstats_s _stats,
            void *           _userdata);
  public:
    // python-specific constructor with keyword arguments
    fs64(py_framesync_callback _callback)
    {
        q = framesync64_create(py_callback_wrapper, this);
        py_callback = _callback;
    }

    void py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.itemsize != sizeof(std::complex<float>))
            throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // execute on input
        execute((std::complex<float>*) info.ptr, info.shape[0]);
    }
#endif
};
#pragma GCC visibility pop

#ifdef PYTHONLIB
py::dict framesyncstats_to_dict(framesyncstats_s _stats,
                                bool             _header_valid=true,
                                bool             _payload_valid=true)
{
    return py::dict(
        "header"_a  = _header_valid,
        "payload"_a = _payload_valid,
        "evm"_a     = _stats.evm,
        "rssi"_a    = _stats.rssi,
        "cfo"_a     = _stats.cfo);
}

int py_callback_wrapper(
        unsigned char *  _header,
        int              _header_valid,
        unsigned char *  _payload,
        unsigned int     _payload_len,
        int              _payload_valid,
        framesyncstats_s _stats,
        void *           _userdata)
{
    // type cast user data as frame synchronizer
    fs64 * fs = (fs64*) _userdata;

    // wrap C-style callback and invoke python callback
    py::array_t<uint8_t> header ({8,           },{1,},(uint8_t*)_header);
    py::array_t<uint8_t> payload({_payload_len,},{1,},(uint8_t*)_payload);
    py::dict stats =  framesyncstats_to_dict(_stats, _header_valid, _payload_valid);
    py::object o = fs->py_callback(header,payload,stats);

    // interpret return value
    if (py::isinstance<py::bool_>(o)) {
        return bool(py::bool_(o)) ? 0 : 1;
    } else if (py::isinstance<py::int_>(o)) {
        return int(py::int_(o));
    }
    return 0;
}

void init_fs64(py::module &m)
{
    py::class_<fs64>(m, "fs64")
        .def(py::init<py_framesync_callback>())
        .def("display", &fs64::display,    "print object properties to stdout")
        .def("reset",   &fs64::reset,      "reset frame synchronizer object")
        .def("execute", &fs64::py_execute, "execute on a block of samples")
        ;
}
#endif

} // namespace liquid

#endif //__FS64_HH__
