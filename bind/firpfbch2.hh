// firpfbch2 c++/python bindings
#ifndef __FIRPFBCH2_HH__
#define __FIRPFBCH2_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class firpfbch2 : public object
{
  public:
    // Kaiser prototype
    firpfbch2(int _type, unsigned int _M, unsigned int _m=4, float _As=60.0f)
        { q = firpfbch2_crcf_create_kaiser(_type, _M, _m, _As); }

    // destructor
    ~firpfbch2() { firpfbch2_crcf_destroy(q); }

    // reset object
    void reset() { firpfbch2_crcf_reset(q); }

    // representation
    std::string repr() const { return std::string("<liquid.firpfbch2") +
                    ", type=" + (get_type()==LIQUID_ANALYZER?"analysis":"synthesis")+
                    ", M=" + std::to_string(get_M()) +
                    ", m=" + std::to_string(get_m()) +
                    ">";}

    int          get_type() const { return firpfbch2_crcf_get_type(q); }
    unsigned int get_M   () const { return firpfbch2_crcf_get_M   (q); }
    unsigned int get_m   () const { return firpfbch2_crcf_get_m   (q); }

    // execute on a block of samples
    void execute(std::complex<float> * _x, std::complex<float> * _y)
        { firpfbch2_crcf_execute(q, _x, _y); }

  protected:
    firpfbch2_crcf q;

#ifdef PYTHONLIB
  public:
    py::array_t<std::complex<float>> py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // expected sizes
        unsigned int M  = get_M();
        unsigned int M2 = M / 2;
        unsigned int num_input  = get_type()==LIQUID_ANALYZER ? M2 : M ;
        unsigned int num_output = get_type()==LIQUID_ANALYZER ? M  : M2;

        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // comptue sample size, number of samples in buffer, and stride between samples
        size_t ss = sizeof(std::complex<float>);
        if ((unsigned int)info.shape[0] != num_input) {
            throw std::runtime_error("invalid input shape " + std::to_string(info.shape[0]) +
                                     ", expected " + std::to_string(num_input));
        } else if (info.strides[0]/ss != 1) {
            // TODO: break into pieces if stride > 1
            throw std::runtime_error("invalid input stride, must be 1");
        }

        // allocate output buffer
        py::array_t<std::complex<float>> buf_out(num_output);

        // execute on data samples
        execute((std::complex<float>*) info.ptr,
                (std::complex<float>*) buf_out.request().ptr);
        return buf_out;
    }
#endif
};

// specific analysis channelizer
class firpfbch2a : public firpfbch2
{
  public:
    // Kaiser prototype
    firpfbch2a(unsigned int _M, unsigned int _m=4, float _As=60.0f) :
        firpfbch2(LIQUID_ANALYZER, _M, _m, _As) {}
};

// specific synthesis channelizer
class firpfbch2s : public firpfbch2
{
  public:
    // Kaiser prototype
    firpfbch2s(unsigned int _M, unsigned int _m=4, float _As=60.0f) :
        firpfbch2(LIQUID_SYNTHESIZER, _M, _m, _As) {}
};

#ifdef PYTHONLIB
static void init_firpfbch2a(py::module &m)
{
    py::class_<firpfbch2a>(m, "firpfbch2a",
        "Finite impulse response polyphase filterbank analysis channelizer, oversampled by 2")
        .def(py::init<unsigned int, unsigned int, float>(),
             py::arg("M"),
             py::arg("m")=1.0f,
             py::arg("As")=60.,
             "create analysis channelizer given number of channels")
        .def("__repr__", &firpfbch2a::repr)
        .def("reset", &firpfbch2a::reset,      "reset object's internal state")
        .def_property_readonly("type", &firpfbch2a::get_type, "get delay in samples")
        .def_property_readonly("M",    &firpfbch2a::get_M,    "get resampling rate")
        .def_property_readonly("m",    &firpfbch2a::get_m,    "get resampling rate")
        .def("execute", &firpfbch2a::py_execute, "execute on a block of samples")
        ;
}

static void init_firpfbch2s(py::module &m)
{
    py::class_<firpfbch2s>(m, "firpfbch2s",
        "Finite impulse response polyphase filterbank synthesis channelizer, oversampled by 2")
        .def(py::init<unsigned int, unsigned int, float>(),
             py::arg("M"),
             py::arg("m")=1.0f,
             py::arg("As")=60.,
             "create synthesis channelizer given number of channels")
        .def("__repr__", &firpfbch2s::repr)
        .def("reset", &firpfbch2s::reset,      "reset object's internal state")
        .def_property_readonly("type", &firpfbch2s::get_type, "get delay in samples")
        .def_property_readonly("M",    &firpfbch2s::get_M,    "get resampling rate")
        .def_property_readonly("m",    &firpfbch2s::get_m,    "get resampling rate")
        .def("execute", &firpfbch2s::py_execute, "execute on a block of samples")
        ;
}
#endif

} // namespace liquid

#endif //__FIRPFBCH2_HH__
