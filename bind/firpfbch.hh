// firpfbch c++/python bindings
#ifndef __FIRPFBCH_HH__
#define __FIRPFBCH_HH__

#include <complex>
#include <iostream>
#include <stdexcept>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class firpfbch : public object
{
  public:
    // Kaiser prototype
    firpfbch(int _type, unsigned int _M, unsigned int _m=4,
             float _As=60.0f, float _bw=-1.0f)
        { q = firpfbch_crcf_create_prototype(_type, _M, _m, _As, _bw); }

    // destructor
    ~firpfbch() { firpfbch_crcf_destroy(q); }

    // reset object
    void reset() { firpfbch_crcf_reset(q); }

    // object type
    std::string type() const { return "firpfbch"; }

    // representation
    std::string repr() const { return std::string("<liquid.firpfbch") +
                    ", type=" + (get_type()==LIQUID_ANALYZER?"analysis":"synthesis")+
                    ", M=" + std::to_string(get_num_channels()) +
                    ", sub_len=" + std::to_string(get_sub_len()) +
                    ">"; }

    int          get_type        () const { return firpfbch_crcf_get_type        (q); }
    unsigned int get_num_channels() const { return firpfbch_crcf_get_num_channels(q); }
    unsigned int get_sub_len     () const { return firpfbch_crcf_get_sub_len     (q); }
    unsigned int get_semi_len    () const { return get_sub_len() / 2; }

    // execute on a block of samples
    void execute(std::complex<float> * _x, std::complex<float> * _y)
    {
        if (get_type() == LIQUID_ANALYZER)
            firpfbch_crcf_analyzer_execute(q, _x, _y);
        else
            firpfbch_crcf_synthesizer_execute(q, _x, _y);
    }

  protected:
    firpfbch_crcf q;

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
        unsigned int num_channels = get_num_channels();
        if (num_inputs % num_channels)
            throw std::runtime_error("input size (" + std::to_string(num_inputs) +
                                     ") must be divisible by the number of channels (" +
                                     std::to_string(num_channels) + ")");

        // verify input stride
        if (num_inputs && (info.strides[0] < 0 ||
                           static_cast<size_t>(info.strides[0]) != ss))
            throw std::runtime_error("invalid input stride, must be 1");

        // allocate output buffer [num_blocks x num_channels]
        size_t num_blocks = num_inputs / num_channels;
        py::array_t<std::complex<float>> buf_out({num_blocks, static_cast<size_t>(num_channels)});

        // run channelizer on each block and save output samples
        std::complex<float> * x = static_cast<std::complex<float> *>(info.ptr);
        std::complex<float> * y = static_cast<std::complex<float> *>(buf_out.request().ptr);
        for (size_t i=0; i<num_blocks; i++)
            execute(x + i*num_channels, y + i*num_channels);
        return buf_out;
    }
#endif
};

// specific analysis channelizer
class firpfbcha : public firpfbch
{
  public:
    // Kaiser prototype
    firpfbcha(unsigned int _M, unsigned int _m=4,
              float _As=60.0f, float _bw=-1.0f) :
        firpfbch(LIQUID_ANALYZER, _M, _m, _As, _bw) {}
};

// specific synthesis channelizer
class firpfbchs : public firpfbch
{
  public:
    // Kaiser prototype
    firpfbchs(unsigned int _M, unsigned int _m=4,
              float _As=60.0f, float _bw=-1.0f) :
        firpfbch(LIQUID_SYNTHESIZER, _M, _m, _As, _bw) {}
};

#ifdef LIQUID_PYTHONLIB
static void init_firpfbcha(py::module &m)
{
    py::class_<firpfbcha>(m, "firpfbcha",
        "Finite impulse response polyphase filterbank analysis channelizer")
        .def(py::init<unsigned int, unsigned int, float, float>(),
             py::arg("M"),
             py::arg("m")=4,
             py::arg("As")=60.0f,
             py::arg("bw")=-1.0f,
             "create analysis channelizer given number of channels")
        .def("__repr__", &firpfbcha::repr)
        .def("reset", &firpfbcha::reset,      "reset object's internal state")
        .def_property_readonly("type", &firpfbcha::get_type, "get channelizer type")
        .def_property_readonly("M",       &firpfbcha::get_num_channels, "get number of channels")
        .def_property_readonly("sub_len", &firpfbcha::get_sub_len,      "get prototype sub-filter length")
        .def_property_readonly("semi_len", &firpfbcha::get_semi_len,    "get prototype semi-length")
        .def("execute", &firpfbcha::py_execute,
             "process 1-D input in M-sample blocks and return shape (num_blocks, M)")
        ;
}

static void init_firpfbchs(py::module &m)
{
    py::class_<firpfbchs>(m, "firpfbchs",
        "Finite impulse response polyphase filterbank synthesis channelizer")
        .def(py::init<unsigned int, unsigned int, float, float>(),
             py::arg("M"),
             py::arg("m")=4,
             py::arg("As")=60.0f,
             py::arg("bw")=-1.0f,
             "create synthesis channelizer given number of channels")
        .def("__repr__", &firpfbchs::repr)
        .def("reset", &firpfbchs::reset,      "reset object's internal state")
        .def_property_readonly("type", &firpfbchs::get_type, "get channelizer type")
        .def_property_readonly("M",       &firpfbchs::get_num_channels, "get number of channels")
        .def_property_readonly("sub_len", &firpfbchs::get_sub_len,      "get prototype sub-filter length")
        .def_property_readonly("semi_len", &firpfbchs::get_semi_len,    "get prototype semi-length")
        .def("execute", &firpfbchs::py_execute,
             "process 1-D input in M-sample blocks and return shape (num_blocks, M)")
        ;
}
#endif

} // namespace liquid

#endif //__FIRPFBCH_HH__
