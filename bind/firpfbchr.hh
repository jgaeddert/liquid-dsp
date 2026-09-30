// firpfbchr c++/python bindings
#ifndef __FIRPFBCHR_HH__
#define __FIRPFBCHR_HH__

#include <complex>
#include <iostream>
#include <stdexcept>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class firpfbchr : public object
{
  public:
    // Kaiser prototype
    firpfbchr(unsigned int _chans, unsigned int _decim,
              unsigned int _m=4,   float _As=60.0f)
        { q = firpfbchr_crcf_create_kaiser(_chans, _decim, _m, _As); }

    // destructor
    ~firpfbchr() { firpfbchr_crcf_destroy(q); }

    // reset object
    void reset() { firpfbchr_crcf_reset(q); }

    // object type
    std::string type() const { return "firpfbchr"; }

    // representation
    std::string repr() const { return std::string("<liquid.firpfbchr") +
                    ", chans=" + std::to_string(get_num_channels()) +
                    ", decim=" + std::to_string(get_decim_rate()) +
                    ", m=" + std::to_string(get_m()) +
                    ">"; }

    // accessors
    unsigned int get_num_channels() const { return firpfbchr_crcf_get_num_channels(q); }
    unsigned int get_decim_rate  () const { return firpfbchr_crcf_get_decim_rate  (q); }
    unsigned int get_m           () const { return firpfbchr_crcf_get_m           (q); }

    // push a block of decim-rate input samples
    void push(std::complex<float> * _x) { firpfbchr_crcf_push(q, _x); }

    // execute and write one output sample per channel
    void execute(std::complex<float> * _y) { firpfbchr_crcf_execute(q, _y); }

  private:
    firpfbchr_crcf q;

#ifdef LIQUID_PYTHONLIB
  public:
    py::array_t<std::complex<float>> py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.itemsize != sizeof(std::complex<float>))
            throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // compute number of input samples, blocks, and output channels
        size_t       ss           = sizeof(std::complex<float>);
        size_t       num_inputs   = static_cast<size_t>(info.shape[0]);
        unsigned int decim        = get_decim_rate();
        unsigned int num_channels = get_num_channels();
        if (num_inputs % decim)
            throw std::runtime_error("input size (" + std::to_string(num_inputs) +
                                     ") must be divisible by decim (" + std::to_string(decim) + ")");

        // verify input stride
        if (num_inputs && (info.strides[0] < 0 ||
                           static_cast<size_t>(info.strides[0]) != ss))
            throw std::runtime_error("invalid input stride, must be 1");

        // allocate output buffer [num_blocks x num_channels]
        size_t num_blocks = num_inputs / decim;
        py::array_t<std::complex<float>> buf_out({num_blocks, static_cast<size_t>(num_channels)});

        // run channelizer on each block and save output samples
        std::complex<float> * x = static_cast<std::complex<float> *>(info.ptr);
        std::complex<float> * y = static_cast<std::complex<float> *>(buf_out.request().ptr);
        for (size_t i=0; i<num_blocks; i++) {
            push(x + i*decim);
            execute(y + i*num_channels);
        }
        return buf_out;
    }
#endif
};

#ifdef LIQUID_PYTHONLIB
static void init_firpfbchr(py::module &m)
{
    py::class_<firpfbchr>(m, "firpfbchr",
        "Finite impulse response polyphase filterbank channelizer with rational output rate")
        .def(py::init<unsigned int, unsigned int, unsigned int, float>(),
             py::arg("chans"),
             py::arg("decim"),
             py::arg("m")=4,
             py::arg("As")=60.0f,
             "create rational-rate channelizer using a Kaiser prototype")
        .def("__repr__", &firpfbchr::repr)
        .def("reset", &firpfbchr::reset, "reset object's internal state")
        .def_property_readonly("chans", &firpfbchr::get_num_channels, "get number of output channels")
        .def_property_readonly("decim", &firpfbchr::get_decim_rate, "get input decimation factor")
        .def_property_readonly("m", &firpfbchr::get_m, "get prototype filter semi-length")
        .def("execute", &firpfbchr::py_execute,
             "process 1-D input (length multiple of decim), return shape (num_blocks, chans)")
        ;
}
#endif

} // namespace liquid

#endif //__FIRPFBCHR_HH__
