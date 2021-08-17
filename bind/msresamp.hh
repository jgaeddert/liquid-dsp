// msresamp c++/python bindings
#ifndef __MSRESAMP_HH__
#define __MSRESAMP_HH__

#include <complex>
#include <iostream>
#include <string>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

class msresamp : public object
{
  public:
    // external coefficients
    msresamp(float _r, float _As=60.0f)
        { q = msresamp_crcf_create(_r, _As); }

    // destructor
    ~msresamp() { msresamp_crcf_destroy(q); }

    // reset object
    void reset() { msresamp_crcf_reset(q); }

    // representation
    std::string repr() const { return std::string("<liquid.msresamp") +
                    ", rate=" + std::to_string(get_rate()) +
                    ", delay=" + std::to_string(get_delay()) +
                    ">";}

    // get sample delay
    float get_delay() const { return msresamp_crcf_get_delay(q); }

    // get overall sample rate
    float get_rate() const { return msresamp_crcf_get_rate(q); }

    // get overall sample rate
    // TODO: do a proper job of computing this value
    unsigned int compute_output_length(unsigned int _nx)
        { return 1+(unsigned int)(get_rate() * _nx); }

    // execute on a block of samples
    void execute(std::complex<float> * _x,
                 unsigned int          _nx,
                 std::complex<float> * _y,
                 unsigned int *        _ny)
        { msresamp_crcf_execute(q, _x, _nx, _y, _ny); }

  private:
    msresamp_crcf q;

#ifdef PYTHONLIB
  public:
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

        // compute number of output samples
        unsigned int num_output = compute_output_length(nx);

        // allocate output buffer
        py::array_t<std::complex<float>> buf_out(num_output);

        // execute on data samples
        unsigned int num_written;
        execute((std::complex<float>*) info.ptr, nx,
                (std::complex<float>*) buf_out.request().ptr, &num_written);
        if (num_written != num_output)
            throw std::runtime_error("output length did not match expected");
        return buf_out;
    }
#endif
};

#ifdef PYTHONLIB
void init_msresamp(py::module &m)
{
    py::class_<msresamp>(m, "msresamp")
        //.def(py::init<std::string, py::kwargs>())
        .def(py::init<float, float>(),
             py::arg("rate")=1.0f,
             py::arg("As")=60.)
        .def("__repr__", &msresamp::repr)
        .def("reset", &msresamp::reset,      "reset object's internal state")
        .def_property_readonly("delay", &msresamp::get_delay, "get delay in samples")
        .def_property_readonly("rate",  &msresamp::get_rate,  "get resampling rate")
        .def("execute", &msresamp::py_execute, "execute on a block of samples")
        ;
}
#endif

} // namespace liquid

#endif //__MSRESAMP_HH__
