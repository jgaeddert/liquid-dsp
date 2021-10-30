// modem c++ bindings
#ifndef __MODEM_HH__
#define __MODEM_HH__

#include <complex>
#include <iostream>
#include <string>
#include <vector>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class modem : public object
{
  public:
    // create standard modulation scheme from index
    modem(modulation_scheme _scheme) {
        if (_scheme <= 0 || _scheme >= LIQUID_MODEM_NUM_SCHEMES)
            throw std::runtime_error("invalid/unsupported modulation scheme index " + std::to_string(_scheme));
        q = modemcf_create(_scheme);
    }

    // create standard modulation scheme from string
    modem(std::string _scheme)
    { 
        modulation_scheme ms = liquid_getopt_str2mod(_scheme.c_str());
        if (ms == LIQUID_MODEM_UNKNOWN)
            throw std::runtime_error("invalid/unsupported modulation scheme " + _scheme);
        q = modemcf_create(ms);
    }

    // create arbitrary scheme from table
    modem(std::complex<float> * _table, unsigned int _M)
        { q = modemcf_create_arbitrary(_table, _M); }
    modem(std::vector<std::complex<float>> _table)
        { q = modemcf_create_arbitrary(_table.data(), _table.size()); }

    // destructor
    ~modem() { modemcf_destroy(q); }

    // reset object
    void reset() { modemcf_reset(q); }

    // get modulation scheme as a string
    std::string get_scheme() const
        { return std::string(modulation_types[modemcf_get_scheme(q)].name); }

    // get number of bits per symbol
    unsigned int get_bits_per_symbol() const { return modemcf_get_bps(q); }

    // representation
    std::string repr() const { return std::string("<liquid.modem") +
                    ", type=" + modulation_types[modemcf_get_scheme(q)].name +
                    ", bits/symbol=" + std::to_string(get_bits_per_symbol()) +
                    ">"; }

    // modulate a single value
    std::complex<float> modulate(unsigned int _s)
        { std::complex<float> y; modemcf_modulate(q,_s,&y); return y; }

    // modulate a block of samples

    // demodulate a single value (hard decision)
    unsigned int demodulate(std::complex<float> _y)
        { unsigned int s; modemcf_demodulate(q,_y,&s); return s; }

    // demodulate a single value (hard decision)
    unsigned int demodulate_soft(std::complex<float> _y, unsigned char * _soft_bits)
        { unsigned int s; modemcf_demodulate_soft(q,_y,&s,_soft_bits); return s; }

    // demodulate a block of samples

  private:
    modemcf q;

#ifdef PYTHONLIB
  public:
#endif
};

#ifdef PYTHONLIB
void init_modem(py::module &m)
{
    py::class_<modem>(m, "modem",
        "Linear modulation and demodulation")
        .def(py::init<std::string>(),
            "create modem from type string")
#if 0
        .def(py::init<int>(),
            "create modem from standard index")
        .def(py::init<py::array_t<float>>(),
            py::arg("h"),
            "create filter from coefficients")
#endif
        .def("__repr__", &modem::repr)
        .def("reset",
             &modem::reset,
             "reset object's internal state")
        .def("modulate",
             &modem::modulate,
             "modulate a single sample")
        .def("demodulate",
             &modem::demodulate,
             "demodulate a single sample")
        .def_property_readonly("scheme",
            &modem::get_scheme,
            "get modulation scheme as a string")
        .def_property_readonly("bits_per_symbol",
            &modem::get_bits_per_symbol,
            "get modulation scheme as a string")
        ;
}
#endif

} // namespace liquid

#endif //__MODEM_HH__
