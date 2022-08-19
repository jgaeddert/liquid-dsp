// rresamp c++/python bindings
#ifndef __RRESAMP_HH__
#define __RRESAMP_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class rresamp : public object
{
  public:
    // create rational rate resampler with output rate P/Q, relative to input
    rresamp(unsigned int _P, unsigned int _Q, unsigned int _m=12,
            float _bw=0.5f, float _As=60.0f, float _scale=1.0f)
        { q = rresamp_crcf_create_kaiser(_P, _Q, _m, _bw, _As); set_scale(_scale); }

    // destructor
    ~rresamp() { rresamp_crcf_destroy(q); }

    // reset object
    void reset() { rresamp_crcf_reset(q); }

    // representation
    std::string repr() const { return std::string("<liquid.rresamp") +
                    ", P="     + std::to_string(get_P()) +
                    ", Q="     + std::to_string(get_Q()) +
                    ", rate="  + std::to_string(get_rate()) +
                    ", delay=" + std::to_string(get_delay()) +
                    ", scale=" + std::to_string(get_scale()) +
                    ">";}

    // accessor methods
    unsigned int get_P    () const { return rresamp_crcf_get_P    (q); }
    unsigned int get_Q    () const { return rresamp_crcf_get_Q    (q); }
    unsigned int get_delay() const { return rresamp_crcf_get_delay(q); }
    float        get_rate () const { return rresamp_crcf_get_rate (q); }
    float        get_scale() const { float s; rresamp_crcf_get_scale(q,&s); return s; }
    
    void set_scale(float _scale) { rresamp_crcf_set_scale(q,_scale); }

    // execute on several blocks of samples (n x Q in, n x P out)
    void execute(std::complex<float> * _buf_0,
                 std::complex<float> * _buf_1,
                 unsigned int          _n=1)
        { rresamp_crcf_execute_block(q, _buf_0, _n, _buf_1); }

  private:
    rresamp_crcf q;

#ifdef PYTHONLIB
  public:
    py::array_t<std::complex<float>> py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // comptue sample size, number of samples in buffer, and stride between samples
        size_t       ss = sizeof(std::complex<float>);
        // TODO: break into pieces if stride > 1
        if (info.strides[0]/ss != 1)
            throw std::runtime_error("invalid input stride, must be 1");

        unsigned int num_inputs = info.shape[0];
        unsigned int P = get_P();
        unsigned int Q = get_Q();
        if (num_inputs % Q) {
            throw std::runtime_error("input size (" + std::to_string(num_inputs) + 
                    ") must be divisible by P (" + std::to_string(Q) + ")");
        }
        unsigned int num_blocks = num_inputs / Q;

        // allocate output buffer
        py::array_t<std::complex<float>> buf_out(P*num_blocks);

        // execute on data samples
        execute((std::complex<float>*) info.ptr,
                (std::complex<float>*) buf_out.request().ptr,
                num_blocks);
        return buf_out;
    }
#endif
};

#ifdef PYTHONLIB
static void init_rresamp(py::module &m)
{
    py::class_<rresamp>(m, "rresamp",
        "Multi-stage arbitrary rate resampler")
        //.def(py::init<std::string, py::kwargs>())
        .def(py::init<unsigned int, unsigned int, unsigned int, float, float, float>(),
             py::arg("P"),
             py::arg("Q"),
             py::arg("m")=12,
             py::arg("bw")=0.5f,
             py::arg("As")=60.,
             py::arg("scale")=1.,
             "create resampler at output rate Q/P")
        .def("__repr__", &rresamp::repr)
        .def("reset", &rresamp::reset, "reset object's internal state")
        .def("execute", &rresamp::py_execute, "execute on a block of samples")
        .def_property_readonly("P",     &rresamp::get_P,     "get P, original interp rate")
        .def_property_readonly("Q",     &rresamp::get_Q,     "get Q, original decim rate")
        .def_property_readonly("delay", &rresamp::get_delay, "get delay in samples")
        .def_property_readonly("rate",  &rresamp::get_rate,  "get resampling rate")
        .def_property("scale",
            &rresamp::get_scale,
            &rresamp::set_scale,
            "get/set filter output scaling property")
        ;
}
#endif

} // namespace liquid

#endif //__RRESAMP_HH__
