// symtrack_cccf bindings
#ifndef __SYMTRACK_HH__
#define __SYMTRACK_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class symtrack : public object
{
  public:
    symtrack(int          _ftype= LIQUID_FIRFILT_RRC,
             unsigned int _k    = 2,
             unsigned int _m    = 12,
             float        _beta = 0.25f,
             int          _ms   = LIQUID_MODEM_QPSK)
    { q = symtrack_cccf_create(_ftype, _k, _m, _beta, _ms); }

    ~symtrack()  { symtrack_cccf_destroy(q);   }

    // reset object internals
    void reset() { symtrack_cccf_reset(q); }

    // string representation
    // TODO: set values appropriately
    std::string repr() const { return std::string("<liquid.symtrack") +
                    //", ftype=" + liquid_firfilt_type_str[symtrack_cccf_get_ftype(q)][0] +
                    //", k="     + std::to_string(get_k()) +
                    //", m="     + std::to_string(get_m()) +
                    //", beta="  + std::to_string(get_beta()) +
                    ">"; }

    //int          get_ftype() const { return symtrack_cccf_get_ftype(q); }
    //float        get_k()     const { return symtrack_cccf_get_k    (q); }
    //unsigned int get_m()     const { return symtrack_cccf_get_m    (q); }
    //float        get_beta()  const { return symtrack_cccf_get_beta (q); }

    //int get_mod_scheme()  const  { return symtrack_cccf_get_mod_scheme(q); }
    //void set_mod_scheme(int _ms) { symtrack_cccf_set_mod_scheme(q, _ms);   }

    // run on single sample
    void execute(std::complex<float> _x, std::complex<float> * _y, unsigned int * _nw)
        { symtrack_cccf_execute(q, _x, _y, _nw); }

    // run on block of samples
    void execute(std::complex<float> * _x, unsigned int   _n,
                 std::complex<float> * _y, unsigned int * _nw)
        { symtrack_cccf_execute_block(q, _x, _n, _y, _nw); }

  private:
    symtrack_cccf q;

#ifdef PYTHONLIB
  public:
    // python-specific constructor with keyword arguments
    symtrack(py::kwargs o) {
        auto defaults = py::dict("ftype"_a="rrcos",
                "k"_a=2, "m"_a=12, "beta"_a=0.25f, "ms"_a="qpsk");
        auto v = update_dict(o, defaults);
        //py::print(v);

        // parse values
        std::string ftype_str = std::string(py::str(v["ftype"]));
        int ftype = liquid_getopt_str2firfilt(ftype_str.c_str());
        if (ftype == LIQUID_FIRFILT_UNKNOWN)
            throw std::runtime_error("invalid/unsupported modulation scheme: " + ftype_str);

        // parse values
        std::string ms_str = std::string(py::str(v["ms"]));
        int ms = liquid_getopt_str2mod(ms_str.c_str());
        if (ms == LIQUID_MODEM_UNKNOWN)
            throw std::runtime_error("invalid/unsupported modulation scheme: " + ms_str);

        // create the main object
        q = symtrack_cccf_create(ftype, py::int_(v["k"]), py::int_(v["m"]),
                py::float_(v["beta"]), ms);
    }

    py::array_t<std::complex<float>> py_execute(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // comptue sample size, number of samples in buffer, and stride between samples
        size_t       ss = sizeof(std::complex<float>);
        unsigned int nx = info.shape[0];
        // TODO: break into pieces if stride > 1
        if (info.strides[0]/ss != 1)
            throw std::runtime_error("invalid input stride, must be 1");

        // estimate number of output samples
        //unsigned int num_output = compute_output_length(nx);
        unsigned int num_output = nx;

        // allocate output buffer
        py::array_t<std::complex<float>> buf_out(num_output);

        // execute on data samples
        unsigned int num_written;
        execute((std::complex<float>*) info.ptr, nx,
                (std::complex<float>*) buf_out.request().ptr, &num_written);
        // TODO: resize buffer here if needed
        //if (num_written != num_output)
        //    throw std::runtime_error("output length did not match expected");
        buf_out.resize({num_written,});
        return buf_out;
    }
#endif
};

#ifdef PYTHONLIB
void init_symtrack(py::module &m)
{
    py::class_<symtrack>(m, "symtrack", "Symbol stream tracking")
        .def(py::init<py::kwargs>(),
            "create symbol stream tracking object from keyword arguments")
        .def("__repr__", &symtrack::repr)
        .def("execute",
            &symtrack::py_execute,
            "run syncrhonization on a block of samples",
            py::arg("n")=256)
        /*
        .def_property("mod_scheme",
            &symtrack::get_mod_scheme,
            &symtrack::set_mod_scheme,
            "get/set modulation scheme")
        .def_property_readonly("ftype",
            &symtrack::get_ftype,
            "get filter type")
        .def_property_readonly("bw",
            &symtrack::get_bw,
            "get bandwidth")
        .def_property_readonly("k",
            &symtrack::get_k,
            "get number of samples per symbol")
        .def_property_readonly("m",
            &symtrack::get_m,
            "get filter semi-length")
        .def_property_readonly("beta",
            &symtrack::get_beta,
            "get filter excess bandwidth factor")
        */
        ;
}
#endif

} // namespace liquid

#endif //__SYMTRACK_HH__
