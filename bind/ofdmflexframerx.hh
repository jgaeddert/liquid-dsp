// ofdmflexframesync bindings
#ifndef __OFDMFLEXFRAMERX_HH__
#define __OFDMFLEXFRAMERX_HH__

#include <complex>
#include <iostream>
#include <string>
#include <functional>
#include "liquid.hh"
#include "liquid.python.hh"

namespace liquid {

#ifdef LIQUID_PYTHONLIB
// forward declaration of callback wrapper
int py_callback_wrapper_ofdmflexframerx(
        unsigned char *  _header,
        int              _header_valid,
        unsigned char *  _payload,
        unsigned int     _payload_len,
        int              _payload_valid,
        framesyncstats_s _stats,
        void *           _userdata);
#endif

// suppress "declared with greater visibility than the type of its field" warnings
// see: https://stackoverflow.com/questions/2828738/c-warning-declared-with-greater-visibility-than-the-type-of-its-field#3170163
// see: https://gcc.gnu.org/wiki/Visibility
#pragma GCC visibility push(hidden)
class ofdmflexframerx : public object
{
  public:
    ofdmflexframerx(unsigned int _M,
                    unsigned int _cp_len=0,
                    unsigned int _taper_len=0,
                    //unsigned char * _p=NULL,
                    framesync_callback _callback=NULL,
                    void * _userdata=NULL)
    { q = ofdmflexframesync_create(_M, _cp_len, _taper_len, NULL, _callback, _userdata); }

    ~ofdmflexframerx() { ofdmflexframesync_destroy(q); }

    void display() { ofdmflexframesync_print(q); }

    void reset() { ofdmflexframesync_reset(q); }

    // representation
    std::string repr() const { return std::string("<liquid.ofdmflexframerx>"); }

    void execute(std::complex<float> * _buf, unsigned int _buf_len)
        { ofdmflexframesync_execute(q, _buf, _buf_len); }

    //unsigned int get_header_length()  const { return 8; }
    //unsigned int get_payload_length() const { return 64; }
    //unsigned int get_frame_length()   const { return LIQUID_FRAME64_LEN; }

    // specific frame data statistics
    unsigned int get_num_frames_detected() const
        { return get_framedatastats().num_frames_detected; }
    unsigned int get_num_headers_valid() const
        { return get_framedatastats().num_headers_valid; }
    unsigned int get_num_payloads_valid() const
        { return get_framedatastats().num_payloads_valid; }
    unsigned int get_num_bytes_received() const
        { return get_framedatastats().num_bytes_received; }

    void reset_framedatastats() { ofdmflexframesync_reset_framedatastats(q); }

    framedatastats_s get_framedatastats() const
        { return ofdmflexframesync_get_framedatastats(q); }

  private:
    ofdmflexframesync q;

#ifdef LIQUID_PYTHONLIB
  private:
    py_framesync_callback py_callback;
    friend int py_callback_wrapper_ofdmflexframerx(
            unsigned char *  _header,
            int              _header_valid,
            unsigned char *  _payload,
            unsigned int     _payload_len,
            int              _payload_valid,
            framesyncstats_s _stats,
            void *           _userdata);
  public:
    // python-specific constructor with keyword arguments
    ofdmflexframerx(unsigned int          _M,
                    unsigned int          _cp_len,
                    unsigned int          _taper_len,
                    py_framesync_callback _callback,
                    py::object            _context)
    {
        q = ofdmflexframesync_create(_M, _cp_len, _taper_len, NULL,
                                     py_callback_wrapper_ofdmflexframerx, this);
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
        framedatastats_s v = ofdmflexframesync_get_framedatastats(q);
        return py::make_tuple(v.num_frames_detected,
            v.num_headers_valid, v.num_payloads_valid, v.num_bytes_received);
    }
  protected:
    py::object context;
#endif
};
#pragma GCC visibility pop

#ifdef LIQUID_PYTHONLIB
int py_callback_wrapper_ofdmflexframerx(
        unsigned char *  _header,
        int              _header_valid,
        unsigned char *  _payload,
        unsigned int     _payload_len,
        int              _payload_valid,
        framesyncstats_s _stats,
        void *           _userdata)
{
    // type cast user data as frame synchronizer
    ofdmflexframerx * fs = (ofdmflexframerx*) _userdata;

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

static void init_ofdmflexframerx(py::module &m)
{
    py::class_<ofdmflexframerx>(m, "ofdmflexframerx", "Frame synchronizer with 64-byte payload")
        .def(py::init<unsigned int,unsigned int,unsigned int,py_framesync_callback,py::object>(),
             py::arg("M"),
             py::arg("cp"),
             py::arg("taper"),
             py::arg("callback") = py_framesync_callback_default,
             py::arg("context")  = py::none(),
             "create synchronizer given callback function and context")
        .def("__repr__", &ofdmflexframerx::repr)
        .def("reset",
             &ofdmflexframerx::reset,
             "reset frame synchronizer object")
        .def("reset_framedatastats",
             &ofdmflexframerx::reset_framedatastats,
             "reset frame statistics data")
        .def_property_readonly("framedatastats",
            &ofdmflexframerx::py_get_framedatastats,
            "get frame data statistics")
#if 0
        .def_property_readonly("header_len",
            &ofdmflexframerx::get_header_length,
            "get length of header (bytes)")
        .def_property_readonly("payload_len",
            &ofdmflexframerx::get_payload_length,
            "get length of payload (bytes)")
        .def_property_readonly("frame_len",
            &ofdmflexframerx::get_frame_length,
            "get length of output frame (samples)")
#endif
        .def_property_readonly("num_frames_detected",
            &ofdmflexframerx::get_num_frames_detected,
            "get number of frames currently detected")
        .def_property_readonly("num_headers_valid",
            &ofdmflexframerx::get_num_headers_valid,
            "get number of headers currently valid")
        .def_property_readonly("num_payloads_valid",
            &ofdmflexframerx::get_num_payloads_valid,
            "get number of payloads currently valid")
        .def_property_readonly("num_bytes_received",
            &ofdmflexframerx::get_num_bytes_received,
            "get number of bytes currently received")
        .def("execute",
             &ofdmflexframerx::py_execute,
             "execute on a block of samples")
        ;
}
#endif

} // namespace liquid

#endif //__OFDMFLEXFRAMERX_HH__
