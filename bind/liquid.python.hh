#ifndef __LIQUID_PYTHON_HH__
#define __LIQUID_PYTHON_HH__

#ifdef PYTHONLIB

#include <complex>
#include <pybind11/pybind11.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "liquid.hh"
namespace py = pybind11;
using namespace pybind11::literals;

namespace liquid {

// validate keys ('dst' cannot contain any keys not in 'src')
bool validate_dict(py::dict dst, py::dict src);

// validate and update
py::dict update_dict(py::dict dst, py::dict src);

py::dict framesyncstats_to_dict(framesyncstats_s _stats,
                                bool             _header_valid=true,
                                bool             _payload_valid=true);

// function definition for pythonic callback
typedef std::function<py::object(py::object,
                                 py::array_t<uint8_t>,
                                 py::array_t<uint8_t>,
                                 py::dict)> py_framesync_callback;

// default callback function
static py_framesync_callback py_framesync_callback_default =
    [](py::object,
       py::array_t<uint8_t>,
       py::array_t<uint8_t>,
       py::dict)
    { return py::none(); };

} // namespace liquid

#endif // __LIQUID_PYTHON_HH__

#endif // PYTHONLIB

