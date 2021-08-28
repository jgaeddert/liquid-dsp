// firfilt c++ bindings
#ifndef __FIRFILT_HH__
#define __FIRFILT_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class firfilt : public object
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

    // representation
    std::string repr() const { return std::string("<liquid.firfilt") +
                    ", n=" + std::to_string(get_length()) +
                    ", scale=" + std::to_string(get_scale()) +
                    ">"; }

    // output scale
    void  set_scale(float _scale) { firfilt_crcf_set_scale(q,_scale); }
    float get_scale() const { float s; firfilt_crcf_get_scale(q,&s); return s; }

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
    unsigned int get_length() const { return firfilt_crcf_get_length(q); }

    // [c++ exclusive] push/execute one sample at a time
    std::complex<float> step(std::complex<float> _x)
        { push(_x); return execute(); }

  private:
    firfilt_crcf q;

#ifdef PYTHONLIB
  public:
    // python-specific constructor with keyword arguments
    firfilt(std::string ftype, py::kwargs o) {
        // check types
        if (ftype == "lowpass") {
            auto v = update_dict(o, py::dict("n"_a=21, "fc"_a=0.25f, "As"_a=60.0f, "mu"_a=0.0f, "scale"_a=1.0f));
            q = firfilt_crcf_create_kaiser(
                py::int_(v["n"]), py::float_(v["fc"]), py::float_(v["As"]), py::float_(v["mu"]));
        } else if (ftype == "firdespm") {
            auto v = update_dict(o, py::dict("n"_a=21, "fc"_a=0.25f, "As"_a=60.0f, "scale"_a=1.0f));
            q = firfilt_crcf_create_firdespm(py::int_(v["n"]), py::float_(v["fc"]), py::float_(v["As"]));
        } else if (ftype == "rect") {
            q = firfilt_crcf_create_rect(o.contains("n") ? int(py::int_(o["n"])) : 5);
        } else if (ftype == "dcblock" || ftype == "notch") {
            auto v = update_dict(o, py::dict("m"_a=7, "As"_a=60.0f, "f0"_a=0.0f));
            q = firfilt_crcf_create_notch(
                py::int_(v["m" ]), py::float_(v["As"]), py::float_(v["f0"]));
        } else {
            int prototype = liquid_getopt_str2firfilt(ftype.c_str());
            if (prototype != LIQUID_FIRFILT_UNKNOWN) {
                auto v = update_dict(o, py::dict("k"_a=2, "m"_a=5, "beta"_a=0.2f, "mu"_a=0.0f));
                q = firfilt_crcf_create_rnyquist(prototype,
                    py::int_(v["k"]), py::int_(v["m"]), py::float_(v["beta"]), py::float_(v["mu"]));
            } else {
                throw std::runtime_error("invalid/unsupported filter type: " + ftype);
            }
        }
        if (o.contains("scale"))
            set_scale(py::float_(o["scale"]));
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

    // get coefficients as array
    py::array_t<float> get_coefficients() const
    {
        // create output array for storing result
        py::array_t<float> h( firfilt_crcf_get_length(q) );
        firfilt_crcf_get_coefficients(q, (float*)h.request().ptr);
        return h;
    }

    // execute filter on buffer in place
    void py_execute_in_place(py::array_t<std::complex<float>> & _buf)
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
        if (stride==1) {
            firfilt_crcf_execute_block(q, ptr, num_samples, ptr);
        } else {
            for (auto i=0U; i<num_samples; i++) {
                firfilt_crcf_push   (q, ptr[i*stride]);
                firfilt_crcf_execute(q, ptr+i*stride );
            }
        }
    }

    // execute like above but run out of place
    py::array_t<std::complex<float>> py_execute_out_of_place(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // comptue sample size, number of samples in buffer, and stride between samples
        size_t       s           = sizeof(std::complex<float>);
        unsigned int num_samples = info.shape[0];
        unsigned int stride      = info.strides[0]/s;

        // convert buffer to pointer
        std::complex<float> * x = (std::complex<float>*) info.ptr;

        // create output array for storing result
        //py::array_t<std::complex<float>> y({num_samples,},{sizeof(std::complex<float>),},s);
        py::array_t<std::complex<float>> buf_out(num_samples);
        std::complex<float> * y = (std::complex<float>*) buf_out.request().ptr;

        // run filter (out of place)
        if (stride==1) {
            firfilt_crcf_execute_block(q, x, num_samples, y);
        } else {
            for (auto i=0U; i<num_samples; i++) {
                firfilt_crcf_push   (q, x[i*stride]);
                firfilt_crcf_execute(q, y+i);
            }
        }
        return buf_out;
    }
#endif
};

#ifdef PYTHONLIB
void init_firfilt(py::module &m)
{
    py::class_<firfilt>(m, "firfilt",
        "Finite impulse response filter")
        .def(py::init<py::array_t<float>>(),
            py::arg("h"),
            "create filter from coefficients")
        .def(py::init<std::string,
            py::kwargs>(),
            "create filter from prototype")
        .def("__repr__", &firfilt::repr)
        .def("reset",
             &firfilt::reset,
             "reset object's internal state")
        .def("execute_in_place",
             &firfilt::py_execute_in_place,
             "execute on a block of samples in place")
        .def("execute",
             &firfilt::py_execute_out_of_place,
             "execute on a block of samples out of place")
        .def_property_readonly("length",
            &firfilt::get_length,
            "get length of filter")
        .def_property_readonly("coefficients",
            &firfilt::get_coefficients,
            "get coefficients from filter")
        .def_property("scale",
            &firfilt::get_scale,
            &firfilt::set_scale,
            "get/set filter output scaling property")
        ;
}
#endif

} // namespace liquid

#endif //__FIRFILT_HH__
