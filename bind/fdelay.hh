// fdelay c++ bindings
#ifndef __FDELAY_HH__
#define __FDELAY_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class fdelay : public object
{
  public:
    fdelay(unsigned int _nmax,
           unsigned int _m=12,
           unsigned int _npfb=64)
    { q = fdelay_crcf_create(_nmax, _m, _npfb); }

    ~fdelay() { fdelay_crcf_destroy(q); }

    // reset object
    void reset() { fdelay_crcf_reset(q); }

    // representation
    std::string repr() const { return std::string("<liquid.fdelay") +
                    ", nmax="  + std::to_string(nmax     ()) +
                    ", m="     + std::to_string(m        ()) +
                    ", npfb="  + std::to_string(npfb     ()) +
                    ", delay=" + std::to_string(get_delay()) +
                    ">"; }

    // accessor methods
    unsigned int nmax () const { return fdelay_crcf_get_nmax   (q); }
    unsigned int m    () const { return fdelay_crcf_get_m      (q); }
    unsigned int npfb () const { return fdelay_crcf_get_npfb   (q); }
    float    get_delay() const { return fdelay_crcf_get_delay  (q); }
    void     set_delay(float _delay) { fdelay_crcf_set_delay   (q,_delay); }
    void  adjust_delay(float _delay) { fdelay_crcf_adjust_delay(q,_delay); }

    // push one sample
    void push(std::complex<float> _x) { fdelay_crcf_push(q,_x); }

    // write block of samples
    void write(std::complex<float> * _x, unsigned int _n)
        { fdelay_crcf_write(q,_x,_n); }

    // execute vector dot product on filter's internal buffer
    std::complex<float> execute()
    {
        std::complex<float> y;
        fdelay_crcf_execute(q, &y);
        return y;
    }

    // execute on block of samples
    void execute(std::complex<float> * _x,
                 unsigned int          _n,
                 std::complex<float> * _y)
    { fdelay_crcf_execute_block(q, _x, _n, _y); }

    // [c++ exclusive] push/execute one sample at a time
    std::complex<float> execute(std::complex<float> _x)
        { push(_x); return execute(); }

  private:
    fdelay_crcf q;

#ifdef PYTHONLIB
  public:
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
            fdelay_crcf_execute_block(q, ptr, num_samples, ptr);
        } else {
            for (auto i=0U; i<num_samples; i++) {
                fdelay_crcf_push   (q, ptr[i*stride]);
                fdelay_crcf_execute(q, ptr+i*stride );
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
            fdelay_crcf_execute_block(q, x, num_samples, y);
        } else {
            for (auto i=0U; i<num_samples; i++) {
                fdelay_crcf_push   (q, x[i*stride]);
                fdelay_crcf_execute(q, y+i);
            }
        }
        return buf_out;
    }
#endif
};

#ifdef PYTHONLIB
void init_fdelay(py::module &m)
{
    py::class_<fdelay>(m, "fdelay",
        "Fractional delay")
        .def(py::init<unsigned int,unsigned int,unsigned int>(),
            py::arg("nmax"),
            py::arg("m")=12,
            py::arg("npfb")=64,
            "create delay with maximum number of offset samples")
        .def("__repr__", &fdelay::repr)
        .def("reset",
             &fdelay::reset,
             "reset object's internal state")
        .def("execute_in_place",
             &fdelay::py_execute_in_place,
             "execute on a block of samples in place")
        .def("execute",
             &fdelay::py_execute_out_of_place,
             "execute on a block of samples out of place")
        .def_property_readonly("nmax",
            &fdelay::nmax,
            "get maximum allowable delay")
        .def_property_readonly("m",
            &fdelay::m,
            "get filter semi-length (also filter delay)")
        .def_property_readonly("npfb",
            &fdelay::npfb,
            "get number of filters in polyphase filter-bank (resolution for fractional interp.)")
        .def_property("delay",
            &fdelay::get_delay,
            &fdelay::set_delay,
            "get/set delay")
        ;
}
#endif

} // namespace liquid

#endif //__FDELAY_HH__
