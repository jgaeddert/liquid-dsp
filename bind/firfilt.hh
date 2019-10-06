// firfilt c++ bindings
#ifndef __FIRFILT_HH__
#define __FIRFILT_HH__

#include <complex>
#include <iostream>
#include <string>
#include <liquid/liquid.h>
#include "liquid.python.hh"

class firfilt
{
  public:
    // external coefficients
    firfilt(float * _h, unsigned int _n) { q = firfilt_crcf_create(_h, _n); }

    // default: kaiser low-pass
    firfilt(unsigned int _n=51, float _fc=0.25, float _As=60.0f, float _mu=0.0f)
        { q = firfilt_crcf_create_kaiser(_n, _fc, _As, _mu); }

    // rnyquist
    firfilt(int _ftype, unsigned int _k, unsigned int _m=7, float _beta=0.25f, float _mu=0.0f)
        {  q = firfilt_crcf_create_rnyquist(_ftype, _k, _m, _beta, _mu); }

    // destructor
    ~firfilt() { firfilt_crcf_destroy(q); }

    // reset object
    void reset() { firfilt_crcf_reset(q); }

    // print
    void display() { firfilt_crcf_print(q); }

    // push one sample
    void push(std::complex<float> _x) { firfilt_crcf_push(q,_x); }

    // write block of samples
    void write(std::complex<float> * _x, unsigned int _n)
        { firfilt_crcf_write(q,_x,_n); }

    // execute vector dot product on filter's internal buffer
    std::complex<float> execute()
    {
        std::complex<float> y;
        firfilt_crcf_execute(q, &y);
        return y;
    }

    // execute on block of samples
    void execute(std::complex<float> * _x,
                 unsigned int          _n,
                 std::complex<float> * _y)
    { firfilt_crcf_execute_block(q, _x, _n, _y); }

    // get length of filter
    unsigned int get_length() { return firfilt_crcf_get_length(q); }

    // [c++ exclusive] push/execute one sample at a time
    std::complex<float> step(std::complex<float> _x)
        { push(_x); return execute(); }

  private:
    firfilt_crcf q;

#ifdef PYTHONLIB
  public:
    // python-specific constructor with keyword arguments
    firfilt(std::string _ftype, py::kwargs _kwargs) {
        auto lupdate = [](py::dict a, py::dict b) { for (auto p: b) a[p.first]=p.second; };
        int  prototype = liquid_getopt_str2firfilt(_ftype.c_str());
        if (prototype != LIQUID_FIRFILT_UNKNOWN) {
            auto v = py::dict("k"_a=2, "m"_a=5, "beta"_a=0.2f, "mu"_a=0.0f);
            lupdate(v,_kwargs);
            q = firfilt_crcf_create_rnyquist(prototype,
                                             int  (py::int_  (v["k"]) ),
                                             int  (py::int_  (v["m"])),
                                             float(py::float_(v["beta"])),
                                             float(py::float_(v["mu"])));
        } else if (_ftype == "lowpass") {
            auto v = py::dict("n"_a=21, "fc"_a=0.25f, "As"_a=60.0f, "mu"_a=0.0f);
            lupdate(v,_kwargs);
            q = firfilt_crcf_create_kaiser(int(  py::int_  (v["n"]) ),
                                           float(py::float_(v["fc"])),
                                           float(py::float_(v["As"])),
                                           float(py::float_(v["mu"])));
        } else if (_ftype == "rect") {
            q = firfilt_crcf_create_rect(_kwargs.contains("n") ? int(py::int_(_kwargs["n"])) : 5);
        } else if (_ftype == "dcblock") {
            auto v = py::dict("m"_a=7, "As"_a=60.0f);
            lupdate(v,_kwargs);
            q = firfilt_crcf_create_dc_blocker(int  (py::int_  (v["m"])),
                                               float(py::float_(v["As"])));
        } else {
            throw std::runtime_error("invalid/unsupported filter type: " + _ftype);
        }
    }

    // external coefficients using numpy array
    firfilt(py::array_t<float> _h) {
        // get buffer info and verify parameters
        py::buffer_info info = _h.request();
        if (info.itemsize != sizeof(float))
            throw std::runtime_error("invalid input numpy size, use dtype=np.single");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // create object
        q = firfilt_crcf_create((float*)info.ptr, info.shape[0]);
    }

    void py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.itemsize != sizeof(std::complex<float>))
            throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // comptue sample size, number of samples in buffer, and stride between samples
        size_t       s           = sizeof(std::complex<float>);
        unsigned int num_samples = info.shape[0];
        unsigned int stride      = info.strides[0]/s;

        // convert buffers to pointers
        std::complex<float> * ptr = (std::complex<float>*) info.ptr;

        // run filter (in place)
        for (auto i=0U; i<num_samples; i++) {
            firfilt_crcf_push   (q, ptr[i*stride]);
            firfilt_crcf_execute(q, ptr+i*stride );
        }
    }
#endif
};

#ifdef PYTHONLIB
void init_firfilt(py::module &m)
{
    py::class_<firfilt>(m, "firfilt")
        /*
        .def(py::init<py::array_t<float>>(),
             py::arg("h"))
        .def(py::init<unsigned int,float,float,float>(),
             py::arg("h_len")=51, py::arg("fc")=0.25, py::arg("As")=60, py::arg("mu")=0)
        .def(py::init<int,unsigned int,unsigned int,float,float>(),
             py::arg("ftype")=7, py::arg("k")=2, py::arg("m")=5, py::arg("beta")=0.25, py::arg("mu")=0)
        */
        .def(py::init<std::string, py::kwargs>())
        .def("reset",      &firfilt::reset,      "reset object's internal state")
        .def("display",    &firfilt::display,    "print object properties to stdout")
        .def("execute",    &firfilt::py_execute, "execute on a block of samples")
        .def("get_length", &firfilt::get_length, "get length of filter")
        ;
}
#endif

#endif //__FIRFILT_HH__
