// spectral periodogram waterfall object
#ifndef __SPGRAM_HH__
#define __SPGRAM_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

#pragma GCC visibility push(hidden)
class spgram : public object
{
  public:
    spgram(unsigned int _nfft=800,
           unsigned int _wlen=600,
           unsigned int _delay=40,
           std::string  _wtype="hamming")
        {
            liquid_window_type wtype = liquid_getopt_str2window(_wtype.c_str());
            if (wtype == LIQUID_WINDOW_UNKNOWN)
                throw std::runtime_error("invalid/unknown window type: " + _wtype);
            q = spgramcf_create(_nfft, wtype, _wlen, _delay);
        }

    spgram(unsigned int _nfft)
        { q = spgramcf_create_default(_nfft); }

    ~spgram() { spgramcf_destroy(q); }

    void display() { spgramcf_print(q); }

    void reset() { spgramcf_reset(q); }

    // representation
    std::string repr() const { return std::string("<liquid.spgram") +
                    ", nfft=" + std::to_string(get_num_freq()) +
                    ", wlen=" + std::to_string(get_window_len()) +
                    ", delay="+ std::to_string(get_delay()) +
                    ", " + get_wtype() +
                    ">";}

    uint64_t get_num_samples_total() const {
        return spgramcf_get_num_samples_total(q); }

    unsigned int get_num_freq() const {
        return spgramcf_get_nfft(q); }

    unsigned int get_window_len() const {
        return spgramcf_get_window_len(q); }

    unsigned int get_delay() const {
        return spgramcf_get_delay(q); }

    std::string get_wtype() const {
        int wtype = spgramcf_get_wtype(q);
        if (wtype >= LIQUID_WINDOW_NUM_FUNCTIONS)
            throw std::runtime_error("invalid internal window type: " + std::to_string(wtype));
        return std::string( liquid_window_str[wtype][0] );
    }

    void get_psd(float * _psd) const { spgramcf_get_psd(q,_psd); }

    void execute(std::complex<float> _v)
        { spgramcf_push(q, _v); }

    void execute(std::complex<float> * _buf, unsigned int _buf_len)
        { spgramcf_write(q, _buf, _buf_len); }

  private:
    spgramcf q;

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

        // make return arrays
        py::array_t<float> freq(nfreq);
        py::array_t<float> Sxx (nfreq);

        // poulate frequency array
        float * _freq = (float*) freq.request().ptr;
        for (auto i=0U; i<nfreq; i++)
            _freq[i] = ((float)i/(float)nfreq - 0.5f)*_fs + _fc;

        // populate spectrum
        get_psd( (float*)Sxx.request().ptr );

        // return tuple with spectrum matrix and frequency arrays
        return py::make_tuple(Sxx,freq);
    }

#endif
};
#pragma GCC visibility pop

#ifdef PYTHONLIB
void init_spgram(py::module &m)
{
    py::class_<spgram>(m, "spgram",
        "Spectral periodogram-based waterfall generator")
        .def(py::init<unsigned int,unsigned int,unsigned int,std::string>(),
             py::arg("nfft")=800,
             py::arg("wlen")=600,
             py::arg("delay")=400,
             py::arg("wtype")="hamming",
             "create spectral waterfall object from base parameter set")
        .def("__repr__",&spgram::repr)
        .def("reset",   &spgram::reset,      "reset waterfall object")
        .def("execute", &spgram::py_execute, "execute on a block of samples")
        .def_property_readonly("num_samples_total",
            &spgram::get_num_samples_total,
            "get total number of samples processed")
        .def_property_readonly("nfft",
            &spgram::get_num_freq,
            "get number of frequency bins")
        .def_property_readonly("window_len",
            &spgram::get_window_len,
            "get window length")
        .def_property_readonly("delay",
            &spgram::get_delay,
            "get sample delay between transforms")
        .def_property_readonly("wtype",
            &spgram::get_wtype,
            "get window type used for spectral estimation")
        .def("get_psd",
             &spgram::py_get_psd,
             py::arg("fs")=1.0f,
             py::arg("fc")=0.0f,
             "get power spectral density, frequency array,  scaling for sample rate & center frequency")
        ;
}
#endif

} // namespace liquid

#endif //__SPGRAM_HH__
