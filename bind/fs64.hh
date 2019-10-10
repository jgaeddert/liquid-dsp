// framesync64 bindings
#ifndef __FS64_HH__
#define __FS64_HH__

#include <complex>
#include <iostream>
#include <string>
#include <liquid/liquid.h>
#include "liquid.python.hh"

namespace liquid {

#ifdef PYTHONLIB
int fscb(unsigned char *  _header,
         int              _header_valid,
         unsigned char *  _payload,
         unsigned int     _payload_len,
         int              _payload_valid,
         framesyncstats_s _stats,
         void *           _userdata)
{
    std::cout << "python callback invoked!" << std::endl;
    return 0;
}
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
  public:
    // python-specific constructor with keyword arguments
    fs64()
    {
        q = framesync64_create(fscb, NULL);
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
void init_fs64(py::module &m)
{
    py::class_<fs64>(m, "fs64")
        .def(py::init<>())
        .def("display", &fs64::display,    "print object properties to stdout")
        .def("execute", &fs64::py_execute, "execute on a block of samples")
        ;
}
#endif

} // namespace liquid

#endif //__FS64_HH__
