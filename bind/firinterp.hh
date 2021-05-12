// firinterp c++/python bindings
#ifndef __FIRINTERP_HH__
#define __FIRINTERP_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.h"
#include "liquid.python.hh"

namespace liquid {

class firinterp
{
  public:
    // external coefficients
    firinterp(unsigned int _M, float * _h, unsigned int _n)
        { q = firinterp_crcf_create(_M, _h, _n); }

    // default: kaiser low-pass
    firinterp(unsigned int _M, unsigned int _m=12, float _As=60.0f)
        { q = firinterp_crcf_create_kaiser(_M, _m, _As); }

    // rnyquist
    firinterp(int _ftype, unsigned int _M, unsigned int _m=7, float _beta=0.25f, float _mu=0.0f)
        {  q = firinterp_crcf_create_prototype(_ftype, _M, _m, _beta, _mu); }

    // destructor
    ~firinterp() { firinterp_crcf_destroy(q); }

    // reset object
    void reset() { firinterp_crcf_reset(q); }

    // print
    void display() { firinterp_crcf_print(q); }

    // set scale
    void set_scale(float _scale) { firinterp_crcf_set_scale(q, _scale); }

    // get scale
    float get_scale() { float s; firinterp_crcf_get_scale(q, &s); return s; }

    // execute on a single sample
    void execute(std::complex<float> _v, std::complex<float> * _buf)
        { firinterp_crcf_execute(q, _v, _buf); }

    // execute on block of samples
    void execute(std::complex<float> * _x,
                 unsigned int          _n,
                 std::complex<float> * _y)
    { firinterp_crcf_execute_block(q, _x, _n, _y); }

  private:
    firinterp_crcf q;

#ifdef PYTHONLIB
  public:
#if 0
    // python-specific constructor with keyword arguments
    firinterp(std::string ftype, py::kwargs o) {
        auto lupdate = [](py::dict o, py::dict d) {auto r(d);for (auto p: o){r[p.first]=p.second;} return r;};
        int  prototype = liquid_getopt_str2firinterp(ftype.c_str());
        if (prototype != LIQUID_firinterp_UNKNOWN) {
            auto v = lupdate(o, py::dict("k"_a=2, "m"_a=5, "beta"_a=0.2f, "mu"_a=0.0f));
            q = firinterp_crcf_create_rnyquist(prototype,
                py::int_(v["k"]), py::int_(v["m"]), py::float_(v["beta"]), py::float_(v["mu"]));
        } else if (ftype == "lowpass") {
            auto v = lupdate(o, py::dict("n"_a=21, "fc"_a=0.25f, "As"_a=60.0f, "mu"_a=0.0f));
            q = firinterp_crcf_create_kaiser(
                py::int_(v["n"]), py::float_(v["fc"]), py::float_(v["As"]), py::float_(v["mu"]));
        } else if (ftype == "firdespm") {
            auto v = lupdate(o, py::dict("n"_a=21, "fc"_a=0.25f, "As"_a=60.0f));
            q = firinterp_crcf_create_firdespm(py::int_(v["n"]), py::float_(v["fc"]), py::float_(v["As"]));
        } else if (ftype == "rect") {
            q = firinterp_crcf_create_rect(o.contains("n") ? int(py::int_(o["n"])) : 5);
        } else if (ftype == "dcblock" || ftype == "notch") {
            auto v = lupdate(o, py::dict("m"_a=7, "As"_a=60.0f, "f0"_a=0.0f));
            q = firinterp_crcf_create_notch(
                py::int_(v["m" ]), py::float_(v["As"]), py::float_(v["f0"]));
        } else {
            throw std::runtime_error("invalid/unsupported filter type: " + ftype);
        }
    }

    // external coefficients using numpy array
    firinterp(py::array_t<float> _h) {
        // get buffer info and verify parameters
        py::buffer_info info = _h.request();
        if (info.itemsize != sizeof(float))
            throw std::runtime_error("invalid input numpy size, use dtype=np.single");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // create object
        q = firinterp_crcf_create((float*)info.ptr, info.shape[0]);
    }
#endif

    py::array_t<std::complex<float>> py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.itemsize != sizeof(std::complex<float>))
            throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // comptue sample size, number of samples in buffer, and stride between samples
        size_t       ss = sizeof(std::complex<float>);
        unsigned int n0 = info.shape[0];
        if (info.strides[0]/ss != 1)
            throw std::runtime_error("invalid input stride, must be 1");

        // allocate output buffer
        unsigned int num_output = n0*firinterp_crcf_get_interp_rate(q);
        py::array_t<std::complex<float>> buf_out(num_output);

        // execute on data samples
        execute((std::complex<float>*) info.ptr, n0,
                (std::complex<float>*) buf_out.request().ptr);
        return buf_out;
    }
#endif
};

#ifdef PYTHONLIB
void init_firinterp(py::module &m)
{
    py::class_<firinterp>(m, "firinterp")
        //.def(py::init<std::string, py::kwargs>())
        .def(py::init<unsigned int, unsigned int, float>(),
             py::arg("M"), py::arg("m")=12, py::arg("As")=60.)
        .def("reset",      &firinterp::reset,      "reset object's internal state")
        .def("display",    &firinterp::display,    "print object properties to stdout")
        .def("execute",    &firinterp::py_execute, "execute on a block of samples")
        ;
}
#endif

} // namespace liquid

#endif //__FIRINTERP_HH__
