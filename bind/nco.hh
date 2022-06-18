// nco_crcf bindings
#ifndef __NCO_HH__
#define __NCO_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class nco : public object
{
  public:
    nco(float _phi=0.0f, float _dphi=0.0f) {
        q = nco_crcf_create(LIQUID_VCO);
        set_phase(_phi);
        set_frequency(_dphi);
        }

    // copy constructor
    nco(const nco &m) { q = nco_crcf_copy(m.q); }

    ~nco() { nco_crcf_destroy(q); }

    // reset object internals
    void reset() { nco_crcf_reset(q); }

    // string representation
    std::string repr() const { return std::string("<liquid.nco") +
                    ", phi=" +  std::to_string(get_phase()) +
                    ", dphi=" + std::to_string(get_frequency()) +
                    ">"; }

    float get_phase    () const      { return nco_crcf_get_phase(q);        }
    void  set_phase    (float _phi ) { nco_crcf_set_phase       (q, _phi ); }
    void  adjust_phase (float _phi ) { nco_crcf_adjust_phase    (q, _phi ); }

    float get_frequency    () const       { return nco_crcf_get_frequency(q);         }
    void  set_frequency    (float _dphi ) { nco_crcf_set_frequency       (q, _dphi ); }
    void  adjust_frequency (float _dphi ) { nco_crcf_adjust_frequency    (q, _dphi ); }

    // mix a block of samples up
    void mix_block_up(std::complex<float> * _x, std::complex<float> * _y, unsigned int _n)
        { nco_crcf_mix_block_up(q, _x, _y, _n); }

    // mix a block of samples down
    void mix_block_down(std::complex<float> * _x, std::complex<float> * _y, unsigned int _n)
        { nco_crcf_mix_block_down(q, _x, _y, _n); }

  private:
    nco_crcf q;

#ifdef PYTHONLIB
  public:
    py::buffer_info py_validate_array(py::array_t<std::complex<float>> & _buf)
    {
        // get buffer info
        py::buffer_info info = _buf.request();

        // verify input size and dimensions
        if (info.ndim != 1)
            throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

        // compute sample size, number of samples in buffer, and stride between samples
        size_t       ss = sizeof(std::complex<float>);
        // TODO: break into pieces if stride > 1
        if (info.strides[0]/ss != 1)
            throw std::runtime_error("invalid input stride, must be 1");

        return info;
    }

    py::array_t<std::complex<float>> py_mix_up(py::array_t<std::complex<float>> & _buf)
    {
        py::buffer_info info = py_validate_array(_buf);

        // allocate output buffer
        unsigned int nx = info.shape[0];
        py::array_t<std::complex<float>> buf_out(nx);

        // execute on data samples
        mix_block_up((std::complex<float>*) info.ptr,
                     (std::complex<float>*) buf_out.request().ptr, nx);
        return buf_out;
    }

    py::array_t<std::complex<float>> py_mix_down(py::array_t<std::complex<float>> & _buf)
    {
        py::buffer_info info = py_validate_array(_buf);

        // allocate output buffer
        unsigned int nx = info.shape[0];
        py::array_t<std::complex<float>> buf_out(nx);

        // execute on data samples
        mix_block_down((std::complex<float>*) info.ptr,
                       (std::complex<float>*) buf_out.request().ptr, nx);
        return buf_out;
    }
#endif
};

#ifdef PYTHONLIB
void init_nco(py::module &m)
{
    py::class_<nco>(m, "nco", "numerically-controlled oscillator")
        .def(py::init<float,float>(),
            py::arg("phi")=0.0f,
            py::arg("dphi")=0.0f,
            "create nco, specifying phase (radians) and frequency (radians/sample)")
        .def("__repr__", &nco::repr)
        .def("mix_up",
            &nco::py_mix_up,
            "mix block of samples up")
        .def("mix_down",
            &nco::py_mix_down,
            "mix block of samples down")
        .def_property("phase",
            &nco::get_phase,
            &nco::set_phase,
            "get/set phase (radians)")
        .def_property("frequency",
            &nco::get_frequency,
            &nco::set_frequency,
            "get/set frequency (radians/sample)")
        ;
}
#endif

} // namespace liquid

#endif //__NCO_HH__
