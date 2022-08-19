// ofdmflexframegen bindings
#ifndef __OFDMFLEXFRAMETX_HH__
#define __OFDMFLEXFRAMETX_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class ofdmflexframetx : public object
{
  public:
    ofdmflexframetx(unsigned int _M,
                    unsigned int _cp_len=0,
                    unsigned int _taper_len=0)
                    //unsigned char * _p=NULL,
                    //ofdmflexframegenprops_s * _fgprops=NULL
    { q = ofdmflexframegen_create(_M, _cp_len, _taper_len, NULL, NULL); }

    ~ofdmflexframetx()  { ofdmflexframegen_destroy(q);   }

    // reset object internals
    void reset() { ofdmflexframegen_reset(q); }
    void display() const{ ofdmflexframegen_print(q); }

    std::string repr() const
        { return std::string("<liquid.ofdmflexframetx>"); }

    // assemble frame
    void assemble(unsigned char * _header=NULL,
                  unsigned char * _payload=NULL,
                  unsigned int    _payload_len=0)
        { ofdmflexframegen_assemble(q, _header, _payload, _payload_len); }

    // write samples to buffer
    void generate(std::complex<float> * _buf, unsigned int _buf_len)
        { ofdmflexframegen_write(q, _buf, _buf_len); }

    bool is_assembled() const { return ofdmflexframegen_is_assembled(q); }

  private:
    ofdmflexframegen q;

#ifdef PYTHONLIB
  public:
    void py_assemble(py::object & _header,
                     py::object & _payload)
    {
        // initialize pointers for header and payload
        unsigned char * header_ptr (NULL);
        unsigned char * payload_ptr(NULL);

        // determine header
        if (py::isinstance<py::array_t<uint8_t>>(_header)) {
            // get output info and validate size/shape
            py::buffer_info header = py::cast<py::array_t<uint8_t>>(_header).request();
            if (header.ndim != 1)
                throw std::runtime_error("invalid header number of input dimensions, must be 1-D array");
            if (header.shape[0] != 8)
                throw std::runtime_error("invalid header length; expected 8");
            header_ptr = (unsigned char*) header.ptr;
        } else if (!py::isinstance<py::none>(_header)) {
            throw std::runtime_error("invalid header type; expected None or dtype=np.uint8");
        }

        // determine payload
        unsigned int payload_len = 64; // default if type is None
        if (py::isinstance<py::array_t<uint8_t>>(_payload)) {
            py::buffer_info payload = py::cast<py::array_t<uint8_t>>(_payload).request();
            if (payload.ndim != 1)
                throw std::runtime_error("invalid payload number of input dimensions, must be 1-D array");
            payload_len = payload.shape[0];
            payload_ptr = (unsigned char*) payload.ptr;
        } else if (!py::isinstance<py::none>(_payload)) {
            throw std::runtime_error("invalid payload type; expected None or dtype=np.uint8");
        }

        // assemble frame
        assemble(header_ptr, payload_ptr, payload_len);
    }

    py::array_t<std::complex<float>> py_generate(unsigned int _n)
    {
        // allocate output buffer
        py::array_t<std::complex<float>> buf(_n);

        // pass to top-level generate method
        generate((std::complex<float>*) buf.request().ptr, _n);
        return buf;
    }
#endif
};

#ifdef PYTHONLIB
static void init_ofdmflexframetx(py::module &m)
{
    py::class_<ofdmflexframetx>(m, "ofdmflexframetx")
        .def(py::init<unsigned int,unsigned int,unsigned int>(),
            py::arg("M"),
            py::arg("cp"),
            py::arg("taper"),
            "create object")
        .def("__repr__", &ofdmflexframetx::repr)
        .def_property_readonly("assembled",
                &ofdmflexframetx::is_assembled,
                "is frame assembled")
        .def("assemble",
                &ofdmflexframetx::py_assemble,
                "assemble a frame given an header and payload",
                py::arg("header")=py::none(),
                py::arg("payload")=py::none())
        .def("generate",
                &ofdmflexframetx::py_generate,
                "generate a block of samples",
                py::arg("n")=256)
        ;
}
#endif

} // namespace liquid

#endif //__OFDMFLEXFRAMETX_HH__
