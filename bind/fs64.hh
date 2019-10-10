// framesync64 bindings
#ifndef __FS64_HH__
#define __FS64_HH__

/* TODO
 *  - check python callback and handle if it returns anything other than an int
 *  - simplify wrapper declaration (use function definition type)
 */

#include <complex>
#include <iostream>
#include <string>
#include <liquid/liquid.h>
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
typedef std::function<int(py::array_t<char>,py::array_t<char>,py::dict)> py_framesync_callback;
//typedef std::function<int(py::dict)> py_framesync_callback;
#endif

class fs64
{
  public:
    fs64(framesync_callback _callback, void * _userdata)
        { q = framesync64_create(_callback, _userdata); }

    ~fs64() { framesync64_destroy(q); }

    void display() { framesync64_print(q); }

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
        std::cout << "running on " + std::to_string(info.shape[0]) + " samples" << std::endl;
        execute((std::complex<float>*) info.ptr, info.shape[0]);
    }
#endif
};

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
    std::cout << "python callback wrapper invoked!" << std::endl;
    fs64 * obj = (fs64*) _userdata;
    // TODO: check for none type as return
    py::array_t<char> header ({8,},{1,},(char*)_header);
    py::array_t<char> payload({_payload_len,},{1,},(char*)_payload);
    py::dict stats =  framesyncstats_to_dict(_stats, _header_valid, _payload_valid);
    return obj->py_callback(header,payload,stats);
}
void init_fs64(py::module &m)
{
    py::class_<fs64>(m, "fs64")
        .def(py::init<py_framesync_callback>())
        .def("display", &fs64::display,    "print object properties to stdout")
        .def("execute", &fs64::py_execute, "execute on a block of samples")
        ;
}
#endif

} // namespace liquid

#endif //__FS64_HH__
