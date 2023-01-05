// firhilb c++/python bindings
#ifndef __FIRHILB_HH__
#define __FIRHILB_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class firhilb : public object
{
  public:
    // external coefficients
    firhilb(unsigned int _m, float _as)
        { q = firhilbf_create(_m, _as); }

    // copy constructor
    firhilb(const firhilb &m) { q = firhilbf_copy(m.q); }

    // destructor
    ~firhilb() { firhilbf_destroy(q); }

    // reset object
    void reset() { firhilbf_reset(q); }

    // object type
    std::string type() const { return "firhilb"; }

    // representation
    std::string repr() const { return std::string("<liquid.firhilb") +
                    ">"; }

    // print
    void display() { firhilbf_print(q); }

    // output scale
    //void set_scale(float _scale) { firhilbf_set_scale(q, _scale); }
    //float get_scale() const { float s; firhilbf_get_scale(q, &s); return s; }

    // get sub-filter length
    //unsigned int get_sub_len() const { return firhilbf_get_sub_len(q); }

    // real-to-complex: execute on a single sample
    std::complex<float> execute_r2c(float _v)
        { std::complex<float> y; firhilbf_r2c_execute(q, _v, &y); return y; }

    // real-to-complex: execute on block of samples
#if 0
    void execute(float               * _x,
                 unsigned int          _n,
                 std::complex<float> * _y)
        { firhilbf_r2c_execute_block(q, _x, _n, _y); }
#endif

    // complex-to-real: execute on a single sample
    float execute_c2r(std::complex<float> _v)
        { float y0, y1; firhilbf_c2r_execute(q, _v, &y0, &y1); return y0; }

    // complex-to-real: execute on block of samples
#if 0
    void execute(std::complex<float> * _x,
                 unsigned int          _n,
                 float *               _y)
        { firhilbf_c2r_execute_block(q, _x, _n, _y); }
#endif

    // decim: execute on a single pair of samples
    void execute_decim(float * _x, std::complex<float> * _y)
        { firhilbf_decim_execute(q, _x, _y); }

    // decim: execute on a block of samples
    void execute_decim(float * _x, unsigned int _n, std::complex<float> * _y)
        { firhilbf_decim_execute_block(q, _x, _n, _y); }

    // interp: execute on a single sample
    void execute_interp(std::complex<float> _x, float * _y)
        { firhilbf_interp_execute(q, _x, _y); }

    // interp: execute on a block of samples
    void execute_interp(std::complex<float> * _x, unsigned int _n, float * _y)
        { firhilbf_interp_execute_block(q, _x, _n, _y); }

  private:
    firhilbf q;

#ifdef LIQUID_PYTHONLIB
  public:

    /// decim: run on array of values
    py::array_t<std::complex<float>> py_execute_decim(py::array_t<float> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // comptue sample size, number of samples in buffer, and stride between samples
        size_t       ss = sizeof(float);
        unsigned int n  = info.shape[0]; // number of input samples
        if (info.strides[0]/ss != 1)
            throw std::runtime_error("invalid input stride (" + std::to_string(info.strides[0]/ss) + "), must be 1");
        if (n % 2)
            throw std::runtime_error("invalid input size, must be divisible by 2");

        // allocate output buffer
        py::array_t<std::complex<float>> buf_out(n/2);

        // execute on data samples
        execute_decim((float*) info.ptr, n/2, (std::complex<float>*) buf_out.request().ptr);
        return buf_out;
    }

    /// interp: run on array of values
    py::array_t<float> py_execute_interp(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // comptue sample size, number of samples in buffer, and stride between samples
        size_t       ss = sizeof(std::complex<float>);
        unsigned int n  = info.shape[0];
        if (info.strides[0]/ss != 1)
            throw std::runtime_error("invalid input stride, must be 1");

        // allocate output buffer
        py::array_t<float> buf_out(n*2);

        // execute on data samples
        execute_interp((std::complex<float>*) info.ptr, n, (float*) buf_out.request().ptr);
        return buf_out;
    }
#endif
};

#ifdef LIQUID_PYTHONLIB
static void init_firhilb(py::module &m)
{
    py::class_<firhilb>(m, "firhilb",
        "Finite impulse response Hilbert transform (convert to/from complex values)")
        //.def(py::init<std::string, py::kwargs>())
        .def(py::init<unsigned int, float>(),
             py::arg("m")=12, py::arg("As")=60.,
             "create default Hilbert transform")
        .def("__repr__", &firhilb::repr)
        .def("reset",    &firhilb::reset,      "reset object's internal state")
        .def("execute_decim",
             &firhilb::py_execute_decim,
             "convert a block of real samples to complex, decimating by 2")
        .def("execute_interp",
             &firhilb::py_execute_interp,
             "convert a block of complex samples to real, interpolating by 2")
        /*
        .def_property_readonly("semilength",
            &firhilb::get_sub_len,
            "get interpolator semi-rate")
        .def_property("scale",
            &firhilb::get_scale,
            &firhilb::set_scale,
            "get/set filter output scaling property")
        */
        ;
}
#endif

} // namespace liquid

#endif //__FIRHILB_HH__
