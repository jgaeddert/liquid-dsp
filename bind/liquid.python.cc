#include <string>
#include "liquid.python.hh"

#include "fdelay.hh"
#include "fg64.hh"
#include "firfilt.hh"
#include "firhilb.hh"
#include "firinterp.hh"
#include "firpfbch2.hh"
#include "fs64.hh"
#include "modem.hh"
#include "msresamp.hh"
#include "nco.hh"
#include "ofdmflexframetx.hh"
#include "ofdmflexframerx.hh"
#include "rresamp.hh"
#include "spgram.hh"
#include "spwaterfall.hh"
#include "symstreamr.hh"
#include "symtrack.hh"

namespace liquid {

// validate keys ('dst' cannot contain any keys not in 'src')
bool validate_dict(py::dict dst, py::dict src)
{
    for (auto p: dst) {
        if (!src.contains(p.first))
            throw std::runtime_error("invalid key: "+p.first.cast<std::string>());
    }
    return true;
}

// validate and update
py::dict update_dict(py::dict dst, py::dict src)
{
    validate_dict(dst,src);

    auto r(src);
    for (auto p: dst)
        r[p.first]=p.second;
    return r;
}

// get instance type as string (debugging)
// see: https://pybind11.readthedocs.io/en/stable/reference.html#convenience-classes-for-specific-python-types
std::string get_instance_as_string(const py::object & object)
{
    // primitive types
    if (py::isinstance<py::bool_>(object))
        return "py::bool_";
    if (py::isinstance<py::none>(object))
        return "py::none";
    if (py::isinstance<py::int_>(object))
        return "py::int_";
    if (py::isinstance<py::float_>(object))
        return "py::float_";
    if (py::isinstance<py::str>(object))
        return "py::str";

    // still basic but more complex types
    if (py::isinstance<py::tuple>(object))
        return "py::tuple";
    if (py::isinstance<py::list>(object))
        return "py::list";
    if (py::isinstance<py::dict>(object))
        return "py::dict";
    if (py::isinstance<py::set>(object))
        return "py::set";

    // bytes
    if (py::isinstance<py::bytes>(object))
        return "py::bytes";
    if (py::isinstance<py::bytearray>(object))
        return "py::bytearray";

    // arrays of simple type
    if (py::isinstance<py::array_t<uint8_t>>(object))
        return "py::array_t<uint8_t>";
    if (py::isinstance<py::array_t<int8_t>>(object))
        return "py::array_t<int8_t>";
    if (py::isinstance<py::array_t<char>>(object))
        return "py::array_t<char>";

    //
    return "unknown";
}

void py_copy_object_to_array(const py::object &     object,
                             std::vector<uint8_t> & buffer,
                             uint8_t                pad_value,
                             bool                   error_on_truncate)
{
    // lambda function to copy data to destination
    auto copy_pad = [](uint8_t * src, int src_len, uint8_t * dst, int dst_len, uint8_t pad)
    {
        for (int i=0; i<dst_len; i++)
            dst[i] = i < src_len ? src[i] : pad;
    };

    // determine object type (cast for any 8-bit value)
    if (py::isinstance<py::array_t<uint8_t>>(object)) {
        // get output info and validate size/shape
        py::buffer_info info = py::cast<py::array_t<uint8_t>>(object).request();
        if (info.ndim != 1)
            throw std::runtime_error("object shape must be 1-D array");
        auto object_len = info.shape[0];
        if (object_len > buffer.size() && error_on_truncate)
            throw std::runtime_error("object length cannot exceed output vector size");
        // TODO: check stride
        copy_pad((uint8_t*)info.ptr, object_len, buffer.data(), buffer.size(), pad_value);

    } else if (py::isinstance<py::str>(object)) {
        std::string str(py::cast<py::str>(object));
        copy_pad((uint8_t*)str.data(), str.size(), buffer.data(), buffer.size(), pad_value);

    } else if (py::isinstance<py::none>(object)) {
        for (auto & b: buffer)
            b = pad_value;
    } else {
        throw std::invalid_argument("cannot copy contents of input type: " +
            get_instance_as_string(object));
    }
}

py::dict framesyncstats_to_dict(framesyncstats_s _stats,
                                bool             _header_valid,
                                bool             _payload_valid)
{
    std::complex<float> * s = (std::complex<float>*) _stats.framesyms;
    py::array_t<std::complex<float>> syms({_stats.num_framesyms,},{sizeof(std::complex<float>),},s);
    return py::dict(
        "header"_a = _header_valid, "payload"_a = _payload_valid, "evm"_a = _stats.evm,
        "rssi"_a = _stats.rssi,  "cfo"_a = _stats.cfo,
        "syms"_a = syms);
}


PYBIND11_MODULE(liquid, m) {
    m.doc() =
    R"pbdoc(
        liquid-dsp: software-defined radio signal processing library
        ------------------------------------------------------------
    )pbdoc";

    // initialize objects
    liquid::init_fdelay     (m);
    liquid::init_firfilt    (m);
    liquid::init_firpfbch2a (m);
    liquid::init_firpfbch2s (m);
    liquid::init_firhilb    (m);
    liquid::init_firinterp  (m);
    liquid::init_fg64       (m);
    liquid::init_fs64       (m);
    liquid::init_modem      (m);
    liquid::init_msresamp   (m);
    liquid::init_nco        (m);
    liquid::init_ofdmflexframetx(m);
    liquid::init_ofdmflexframerx(m);
    liquid::init_rresamp    (m);
    liquid::init_spgram     (m);
    liquid::init_spwaterfall(m);
    liquid::init_symstreamr (m);
    liquid::init_symtrack   (m);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

} // namespace liquid

