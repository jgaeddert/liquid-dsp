// framegen64 bindings
#ifndef __FG64_HH__
#define __FG64_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class fg64 : public object
{
  public:
    // default constructor
    fg64() { q = framegen64_create();  }

    // copy constructor
    fg64(const fg64 &m) { q = framegen64_copy(m.q); }

    // destructor
    ~fg64() { framegen64_destroy(q); }

    void reset() { }
    std::string repr() const { return std::string("<liquid.fg64") +
                    ", header="  + std::to_string(get_header_length()) +
                    ", payload=" + std::to_string(get_payload_length()) +
                    ", samples=" + std::to_string(get_frame_length()) +
                    ">"; }
    void display() { framegen64_print(q); }
    void execute(unsigned char * _header,
                 unsigned char * _payload,
                 std::complex<float> * _frame)
    { framegen64_execute(q, _header, _payload, _frame); }

    unsigned int get_header_length()  const { return 8; }
    unsigned int get_payload_length() const { return 64; }
    unsigned int get_frame_length()   const { return LIQUID_FRAME64_LEN; }

  private:
    framegen64 q;

#ifdef PYTHONLIB
  public:
    py::array_t<std::complex<float>> py_execute(py::object & _header,
                                                py::object & _payload)
    {
        // initialize pointers for header and payload
        unsigned char * header_ptr(NULL);
        unsigned char * payload_ptr(NULL);

        // determine header
        if (py::isinstance<py::array_t<uint8_t>>(_header)) {
            // get output info and validate size/shape
            py::buffer_info header = py::cast<py::array_t<uint8_t>>(_header).request();
            //if (header.itemsize != sizeof(uint8_t))
            //    throw std::runtime_error("invalid header input numpy size, use dtype=np.uint8");
            if (header.ndim != 1)
                throw std::runtime_error("invalid header number of input dimensions, must be 1-D array");
            if (header.shape[0] != 8)
                throw std::runtime_error("invalid header length; expected 8");
            header_ptr = (unsigned char*) header.ptr;
        } else if (!py::isinstance<py::none>(_header)) {
            throw std::runtime_error("invalid header type");
        }

        // determine payload
        if (py::isinstance<py::array_t<uint8_t>>(_payload)) {
            py::buffer_info payload = py::cast<py::array_t<uint8_t>>(_payload).request();
            //if (payload.itemsize != sizeof(uint8_t))
            //    throw std::runtime_error("invalid payload input numpy size, use dtype=np.uint8");
            if (payload.ndim != 1)
                throw std::runtime_error("invalid payload number of input dimensions, must be 1-D array");
            if (payload.shape[0] != 64)
                throw std::runtime_error("invalid payload length; expected 64");
            payload_ptr = (unsigned char*) payload.ptr;
        } else if (!py::isinstance<py::none>(_payload)) {
            throw std::runtime_error("invalid payload type");
        }

        // allocate output buffer
        py::array_t<std::complex<float>> buf(get_frame_length());

        // pass to top-level execute method
        execute(header_ptr, payload_ptr, (std::complex<float>*) buf.request().ptr);
        return buf;
    }
#endif
};

#ifdef PYTHONLIB
void init_fg64(py::module &m)
{
    py::class_<fg64>(m, "fg64", "Frame generator with 64-byte payload")
        .def(py::init<>())
        .def("__repr__", &fg64::repr)
        .def_property_readonly("header_len",
            &fg64::get_header_length,
            "get length of header (bytes)")
        .def_property_readonly("payload_len",
            &fg64::get_payload_length,
            "get length of payload (bytes)")
        .def_property_readonly("frame_len",
            &fg64::get_frame_length,
            "get length of output frame (samples)")
        .def("execute",
            &fg64::py_execute,
            "generate a frame given header and payload",
            py::arg("header")=py::none(),
            py::arg("payload")=py::none())
        ;
}
#endif

} // namespace liquid

#endif //__FG64_HH__
