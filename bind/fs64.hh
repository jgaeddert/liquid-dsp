// framesync64 bindings
#ifndef __FS64_HH__
#define __FS64_HH__

#include <complex>
#include <iostream>
#include <string>
#include <functional>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

// suppress "declared with greater visibility than the type of its field" warnings
// see: https://stackoverflow.com/questions/2828738/c-warning-declared-with-greater-visibility-than-the-type-of-its-field#3170163
// see: https://gcc.gnu.org/wiki/Visibility
#pragma GCC visibility push(hidden)
class fs64 : public object
{
  public:
    fs64(framesync_callback _callback, void * _userdata)
        { q = framesync64_create(_callback, _userdata); }

    ~fs64() { framesync64_destroy(q); }

    void display() { framesync64_print(q); }

    void reset() { framesync64_reset(q); }

    // representation
    std::string repr() const { return std::string("<liquid.fs64") +
                    ", header="  + std::to_string( 8) +
                    ", payload=" + std::to_string(64) +
                    ", samples=" + std::to_string(LIQUID_FRAME64_LEN) +
                    ">";}

    void execute(std::complex<float> * _buf, unsigned int _buf_len)
        { framesync64_execute(q, _buf, _buf_len); }

    void reset_framedatastats() { framesync64_reset_framedatastats(q); }

    framedatastats_s get_framedatastats() const
        { return framesync64_get_framedatastats(q); }

  private:
    framesync64 q;

#ifdef PYTHONLIB
  private:
    py_framesync_callback py_callback;
    friend int py_callback_wrapper(
            unsigned char *  _header,
            int              _header_valid,
            unsigned char *  _payload,
            unsigned int     _payload_len,
            int              _payload_valid,
            framesyncstats_s _stats,
            void *           _userdata);
  public:
    // python-specific constructor with keyword arguments
    fs64(py_framesync_callback _callback,
         py::object            _context)
    {
        q = framesync64_create(py_callback_wrapper, this);
        py_callback = _callback;
        context     = _context;
    }

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

    py::tuple py_get_framedatastats() const
    {
        framedatastats_s v = framesync64_get_framedatastats(q);
        return py::make_tuple(v.num_frames_detected,
            v.num_headers_valid, v.num_payloads_valid, v.num_bytes_received);
    }
  protected:
    py::object context;
#endif
};
#pragma GCC visibility pop

#ifdef PYTHONLIB
py::dict framesyncstats_to_dict(framesyncstats_s _stats,
                                bool             _header_valid=true,
                                bool             _payload_valid=true)
{
    std::complex<float> * s = (std::complex<float>*) _stats.framesyms;
    py::array_t<std::complex<float>> syms({_stats.num_framesyms,},{sizeof(std::complex<float>),},s);
    return py::dict(
        "header"_a = _header_valid, "payload"_a = _payload_valid, "evm"_a = _stats.evm,
        "rssi"_a = _stats.rssi,  "cfo"_a = _stats.cfo,
        "syms"_a = syms);
}

int py_callback_wrapper(
        unsigned char *  _header,
        int              _header_valid,
        unsigned char *  _payload,
        unsigned int     _payload_len,
        int              _payload_valid,
        framesyncstats_s _stats,
        void *           _userdata)
{
    // type cast user data as frame synchronizer
    fs64 * fs = (fs64*) _userdata;

    // wrap C-style callback and invoke python callback
    py::array_t<uint8_t> header ({8,           },{1,},(uint8_t*)_header);
    py::array_t<uint8_t> payload({_payload_len,},{1,},(uint8_t*)_payload);
    py::dict stats =  framesyncstats_to_dict(_stats, _header_valid, _payload_valid);
    py::object o = fs->py_callback(fs->context,header,payload,stats);

    // interpret return value
    if (py::isinstance<py::bool_>(o)) {
        return bool(py::bool_(o)) ? 0 : 1;
    } else if (py::isinstance<py::int_>(o)) {
        return int(py::int_(o));
    }
    return 0;
}

// default callback function
py_framesync_callback py_fs64_default_callback = [](py::object,
                                                    py::array_t<uint8_t>,
                                                    py::array_t<uint8_t>,
                                                    py::dict) { return py::none(); };
void init_fs64(py::module &m)
{
    py::class_<fs64>(m, "fs64")
        .def(py::init<py_framesync_callback,py::object>(),
             py::arg("callback") = py_fs64_default_callback,
             py::arg("context") = py::none())
        .def("__repr__", &fs64::repr)
        .def("reset",
             &fs64::reset,
             "reset frame synchronizer object")
        .def("reset_framedatastats",
             &fs64::reset_framedatastats,
             "reset frame statistics data")
        .def_property_readonly("framedatastats",
            [](const fs64 &q) { return q.py_get_framedatastats(); },
            "get frame data statistics")
        .def("execute",
             &fs64::py_execute,
             "execute on a block of samples")
        ;
}
#endif

} // namespace liquid

#endif //__FS64_HH__
