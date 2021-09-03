// spectral periodogram waterfall object
#ifndef __SPWATERFALL_HH__
#define __SPWATERFALL_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

#pragma GCC visibility push(hidden)
class spwaterfall : public object
{
  public:
    spwaterfall(unsigned int _nfft=800,
                unsigned int _time=800,
                unsigned int _wlen=600,
                unsigned int _delay=40,
                std::string  _wtype="hamming")
        {
            liquid_window_type wtype = liquid_getopt_str2window(_wtype.c_str());
            if (wtype == LIQUID_WINDOW_UNKNOWN)
                throw std::runtime_error("invalid/unknown window type: " + _wtype);
            q = spwaterfallcf_create(_nfft, wtype, _wlen, _delay, _time);
        }

    spwaterfall(unsigned int _nfft,
                unsigned int _time)
        { q = spwaterfallcf_create_default(_nfft, _time); }

    ~spwaterfall() { spwaterfallcf_destroy(q); }

    void display() { spwaterfallcf_print(q); }

    void reset() { spwaterfallcf_reset(q); }

    // representation
    std::string repr() const { return std::string("<liquid.spwaterfall") +
                    ", nfft=" + std::to_string(get_num_freq()) +
                    ", time=" + std::to_string(get_num_time()) +
                    ", wlen=" + std::to_string(get_window_len()) +
                    ", delay="+ std::to_string(get_delay()) +
                    ", " + get_wtype() +
                    ">";}

    uint64_t get_num_samples_total() const {
        return spwaterfallcf_get_num_samples_total(q); }

    unsigned int get_num_freq() const {
        return spwaterfallcf_get_num_freq(q); }

    unsigned int get_num_time() const {
        return spwaterfallcf_get_num_time(q); }

    unsigned int get_window_len() const {
        return spwaterfallcf_get_window_len(q); }

    unsigned int get_delay() const {
        return spwaterfallcf_get_delay(q); }

    std::string get_wtype() const {
        int wtype = spwaterfallcf_get_wtype(q);
        if (wtype >= LIQUID_WINDOW_NUM_FUNCTIONS)
            throw std::runtime_error("invalid internal window type: " + std::to_string(wtype));
        return std::string( liquid_window_str[wtype][0] );
    }

    const float * get_psd() const {
        return spwaterfallcf_get_psd(q); }

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
        // get buffer info and verify dimensions
        py::buffer_info info = _buf.request();
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // execute on input
        execute((std::complex<float>*) info.ptr, info.shape[0]);
    }

    py::tuple py_get_psd(float _fs, float _fc)
    {
        unsigned int nfreq = get_num_freq();
        unsigned int ntime = get_num_time();
        uint64_t     nsamp = get_num_samples_total();

        // make return arrays
        py::array_t<float> freq(nfreq);
        py::array_t<float> time(ntime);
        py::array_t<float> Sxx({nfreq,ntime},{sizeof(float),nfreq*sizeof(float),},get_psd());

        // populate time, frequency arrays
        float * _freq = (float*) freq.request().ptr;
        float * _time = (float*) time.request().ptr;
        for (auto i=0U; i<nfreq; i++)
            _freq[i] = ((float)i/(float)nfreq - 0.5f)*_fs + _fc;
        for (auto i=0U; i<ntime; i++)
            _time[i] = (float)i/(float)ntime * (float)nsamp / _fs;

        // return tuple with spectrum matrix and time/freq arrays
        return py::make_tuple(Sxx,time,freq);
    }

#endif
};
#pragma GCC visibility pop

#ifdef PYTHONLIB
void init_spwaterfall(py::module &m)
{
    py::class_<spwaterfall>(m, "spwaterfall",
        "Spectral periodogram-based waterfall generator")
        .def(py::init<unsigned int,unsigned int,unsigned int,unsigned int,std::string>(),
             py::arg("nfft")=800,
             py::arg("time")=800,
             py::arg("wlen")=600,
             py::arg("delay")=400,
             py::arg("wtype")="hamming",
             "create spectral waterfall object from base parameter set")
        .def("__repr__",&spwaterfall::repr)
        .def("reset",   &spwaterfall::reset,      "reset waterfall object")
        .def("execute", &spwaterfall::py_execute, "execute on a block of samples")
        .def_property_readonly("num_samples_total",
            &spwaterfall::get_num_samples_total,
            "get total number of samples processed")
        .def_property_readonly("nfft",
            &spwaterfall::get_num_freq,
            "get number of frequency bins")
        .def_property_readonly("time",
            &spwaterfall::get_num_time,
            "get number of time bins")
        .def_property_readonly("window_len",
            &spwaterfall::get_window_len,
            "get window length")
        .def_property_readonly("delay",
            &spwaterfall::get_delay,
            "get sample delay between transforms")
        .def_property_readonly("wtype",
            &spwaterfall::get_wtype,
            "get window type used for spectral estimation")
        .def("get_psd",
             &spwaterfall::py_get_psd,
             py::arg("fs")=1.0f,
             py::arg("fc")=0.0f,
             "get power spectral density")
        ;
}
#endif

} // namespace liquid

#endif //__SPWATERFALL_HH__
