// spectral periodogram waterfall object
#ifndef __SPWATERFALL_HH__
#define __SPWATERFALL_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.h"
#include "liquid.python.hh"

namespace liquid {

#pragma GCC visibility push(hidden)
class spwaterfall
{
  public:
    spwaterfall(unsigned int _nfft,
                int          _wtype,
                unsigned int _window_len,
                unsigned int _delay,
                unsigned int _time)
        { q = spwaterfallcf_create(_nfft, _wtype, _window_len, _delay, _time); }

    spwaterfall(unsigned int _nfft,
                unsigned int _time)
        { q = spwaterfallcf_create_default(_nfft, _time); }

    ~spwaterfall() { spwaterfallcf_destroy(q); }

    void display() { spwaterfallcf_print(q); }

    void reset() { spwaterfallcf_reset(q); }

    uint64_t      get_num_samples_total() { return spwaterfallcf_get_num_samples_total(_q); }
    unsigned int  get_num_freq() { return spwaterfallcf_get_num_freq(_q); }
    unsigned int  get_num_time() { return spwaterfallcf_get_num_time(_q); }
    const float * get_psd()      { return spwaterfallcf_get_psd     (_q); }

    void execute(std::complex<float> _v)
        { spwaterfallcf_push(q, _v); }

    void execute(std::complex<float> * _buf, unsigned int _buf_len)
        { spwaterfallcf_write(q, _buf, _buf_len); }

  private:
    spwaterfallcf q;

#ifdef PYTHONLIB
  public:
    void py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.itemsize != sizeof(std::complex<float>))
            throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // execute on input
        execute((std::complex<float>*) info.ptr, info.shape[0]);
    }

    py::tuple py_get_psd()
    {
        unsigned int nfreq = get_num_freq();
        unsigned int ntime = get_num_time();
        uint64_t     nsamp = get_num_samples_total();

        // make return arrays
        py::array_t<float> freq(nfreq);
        py::array_t<float> time(nfreq);
        py::array_t<float> Sxx({nfreq,ntime},{sizeof(float),nfreq*sizeof(float),},get_psd());

        // populate time, frequency arrays
        float * freq.request().ptr;
        for (unsigned int i=0; i<nfreq; i++) {
            //...
        }

        // 
        return py::make_tuple(Sxx,time,freq);
    }

    py::tuple py_get_framedatastats()
    {
        framedatastats_s v = spwaterfallcf_get_framedatastats(q);
        return py::make_tuple(v.num_frames_detected,
            v.num_headers_valid, v.num_payloads_valid, v.num_bytes_received);
    }
#endif
};
#pragma GCC visibility pop

#ifdef PYTHONLIB
void init_spwaterfall(py::module &m)
{
    py::class_<spwaterfall>(m, "spwaterfall")
        .def(py::init<unsigned int,unsigned int>(),
             py::arg("nfft"), py::arg("time"))
        .def("reset",   &spwaterfall::reset,      "reset frame synchronizer object")
        .def("execute", &spwaterfall::py_execute, "execute on a block of samples")
        .def("get_psd", &spwaterfall::get_psd,    "get power spectral density")
        ;
}
#endif

} // namespace liquid

#endif //__SPWATERFALL_HH__
