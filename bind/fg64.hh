// framegen64 bindings
#ifndef __FG64_HH__
#define __FG64_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.h"
#include "liquid.python.hh"

namespace liquid {

class fg64
{
  public:
    fg64()   { q = framegen64_create();  }
    ~fg64()  { framegen64_destroy(q);    }
    void display() { framegen64_print(q); }
    void execute(unsigned char * _header,
                 unsigned char * _payload,
                 std::complex<float> * _frame)
    { framegen64_execute(q, _header, _payload, _frame); }

    unsigned int get_frame_length() const { return LIQUID_FRAME64_LEN; }

  private:
    framegen64 q;

#ifdef PYTHONLIB
  public:
    void py_execute(py::array_t<uint8_t>             & _header,
                    py::array_t<uint8_t>             & _payload,
                    py::array_t<std::complex<float>> & _buf)
    {
        // get output info and validate size/shape
        py::buffer_info header = _header.request();
        if (header.itemsize != sizeof(uint8_t))
            throw std::runtime_error("invalid header input numpy size, use dtype=np.uint8");
        if (header.ndim != 1)
            throw std::runtime_error("invalid header number of input dimensions, must be 1-D array");
        if (header.shape[0] != 8)
            throw std::runtime_error("invalid header length; expected 8");

        // get output info and validate size/shape
        py::buffer_info payload = _payload.request();
        if (payload.itemsize != sizeof(uint8_t))
            throw std::runtime_error("invalid payload input numpy size, use dtype=np.uint8");
        if (payload.ndim != 1)
            throw std::runtime_error("invalid payload number of input dimensions, must be 1-D array");
        if (payload.shape[0] != 64)
            throw std::runtime_error("invalid payload length; expected 64");

        // get output info and validate size/shape
        py::buffer_info info = _buf.request();
        if (info.itemsize != sizeof(std::complex<float>))
            throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");
        if (info.shape[0] != get_frame_length())
            throw std::runtime_error("invalid frame length; expected " + std::to_string(get_frame_length()));

        // pass to top-level execute method
        execute((unsigned char*)       header.ptr,
                (unsigned char*)       payload.ptr,
                (std::complex<float>*) info.ptr);
    }

    // execute with random header/payload
    void py_execute_random(py::array_t<std::complex<float>> & _buf)
    {
        // get output info and validate size/shape
        py::buffer_info info = _buf.request();
        if (info.itemsize != sizeof(std::complex<float>))
            throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");
        if (info.shape[0] != get_frame_length())
            throw std::runtime_error("invalid frame length; expected " + std::to_string(get_frame_length()));

        // pass to top-level execute method
        execute(NULL, NULL, (std::complex<float>*) info.ptr);
    }

#endif
};

#ifdef PYTHONLIB
void init_fg64(py::module &m)
{
    py::class_<fg64>(m, "fg64")
        .def(py::init<>())
        .def("__repr__", [](const fg64 &q) {
                return std::string("<liquid.fg64") +
                    ", header="  + std::to_string( 8) +
                    ", payload=" + std::to_string(64) +
                    ", samples=" + std::to_string(LIQUID_FRAME64_LEN) +
                    ">";
            })
        .def_property_readonly("frame_len", &fg64::get_frame_length, "get length of output frame (samples)")
        .def("execute", &fg64::py_execute, "generate a frame given header and payload")
        .def("execute", &fg64::py_execute_random, "generate a frame with random header and payload")
        ;
}
#endif

} // namespace liquid

#endif //__FG64_HH__
