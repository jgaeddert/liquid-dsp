// framegen64 bindings
#ifndef __FG64_HH__
#define __FG64_HH__

#include <complex>
#include <iostream>
#include <string>
#include <vector>
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

    // object type
    std::string type() const { return "fg64"; }

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

#ifdef LIQUID_PYTHONLIB
  private:
    std::vector<uint8_t> py_header  = std::vector<uint8_t>( 8); ///< internal header
    std::vector<uint8_t> py_payload = std::vector<uint8_t>(64); ///< internal payload

  public:
    py::array_t<std::complex<float>> py_execute(py::object & _header,
                                                py::object & _payload)
    {
        //py::print("payload type:", get_instance_as_string(_payload));
        // create and copy header contents
        py_copy_object_to_array(_header,  py_header);
        py_copy_object_to_array(_payload, py_payload);

        // run internal method to generate frame samples
        return py_execute_internal();
    }

  private:
    // internal method to operate on internal arrays (data copied to py_header, py_payload)
    py::array_t<std::complex<float>> py_execute_internal()
    {
        // allocate output buffer
        py::array_t<std::complex<float>> buf(get_frame_length());

        // pass to top-level execute method
        execute(py_header.data(), py_payload.data(), (std::complex<float>*) buf.request().ptr);
        return buf;
    }
#endif
};

#ifdef LIQUID_PYTHONLIB
static void init_fg64(py::module &m)
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
            "generate a frame given header and payload, header and payload must be of type str, bytes, or np.array(np.uint8)",
            py::arg("header")=py::none(),
            py::arg("payload")=py::none())
        ;
}
#endif

} // namespace liquid

#endif //__FG64_HH__
