// symstreamrcf bindings
#ifndef __SYMSTREAMR_HH__
#define __SYMSTREAMR_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class symstreamr : public object
{
  public:
    symstreamr(int          _ms = LIQUID_MODEM_QPSK,
               float        _bw=0.5f,
               int          _ftype=LIQUID_FIRFILT_RRC,
               unsigned int _m=12,
               float        _beta=0.25f,
               float        _gain=1.0f)
    { q = symstreamrcf_create_linear(_ftype, _bw, _m, _beta, _ms); set_gain(_gain); }

    ~symstreamr()  { symstreamrcf_destroy(q);   }

    // reset object internals
    void reset() { symstreamrcf_reset(q); }

    // string representation
    // TODO: print values appropriately
    std::string repr() const
        { return std::string("<liquid.symstreamr>"); }

    // output scale
    void  set_gain(float _gain) { symstreamrcf_set_gain(q,_gain); }
    float get_gain() const { return symstreamrcf_get_gain(q); }

    // write samples to buffer
    void generate(std::complex<float> * _buf, unsigned int _buf_len)
        { symstreamrcf_write_samples(q, _buf, _buf_len); }

  private:
    symstreamrcf q;

#ifdef PYTHONLIB
  public:
    // python-specific constructor with keyword arguments
    symstreamr(py::kwargs o) {
        auto defaults = py::dict("ms"_a="qpsk", "bw"_a=0.5f, "ftype"_a="rrcos",
                "m"_a=12, "beta"_a=0.25f, "gain"_a=1.0f);
        auto v = update_dict(o, defaults);
        //py::print(v);

        // parse values
        std::string ms_str = std::string(py::str(v["ms"]));
        int ms = liquid_getopt_str2mod(ms_str.c_str());
        if (ms == LIQUID_MODEM_UNKNOWN)
            throw std::runtime_error("invalid/unsupported modulation scheme: " + ms_str);

        // parse values
        std::string ftype_str = std::string(py::str(v["ftype"]));
        int ftype = liquid_getopt_str2firfilt(ftype_str.c_str());
        if (ftype == LIQUID_FIRFILT_UNKNOWN)
            throw std::runtime_error("invalid/unsupported modulation scheme: " + ftype_str);

        // create the main object
        q = symstreamrcf_create_linear(ftype, py::float_(v["bw"]), py::int_(v["m"]),
                py::float_(v["beta"]), ms);
        set_gain(py::float_(v["gain"]));
    }

    py::array_t<std::complex<float>> py_generate(unsigned int _n)
    {
        // allocate output buffer
        py::array_t<std::complex<float>> buf(_n);

        // pass to top-level generate method
        generate((std::complex<float>*) buf.request().ptr, _n);
        return buf;
    }
#endif
};

#ifdef PYTHONLIB
void init_symstreamr(py::module &m)
{
    py::class_<symstreamr>(m, "symstreamr", "Symbol streaming generator")
        .def(py::init<py::kwargs>(),
            "create symbol streaming object from keyword arguments")
        .def(py::init<unsigned int,unsigned int,unsigned int>(),
            py::arg("M"),
            py::arg("cp"),
            py::arg("taper"),
            "create object")
        .def("__repr__", &symstreamr::repr)
        .def("generate",
            &symstreamr::py_generate,
            "generate a block of samples",
            py::arg("n")=256)
        .def_property("gain",
            &symstreamr::get_gain,
            &symstreamr::set_gain,
            "get/set output gain property")
        ;
}
#endif

} // namespace liquid

#endif //__SYMSTREAMR_HH__
