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

// callback wrapper to bind user-defined callback to python
int py_callback_wrapper(
        unsigned char *  _header,
        int              _header_valid,
        unsigned char *  _payload,
        unsigned int     _payload_len,
        int              _payload_valid,
        framesyncstats_s _stats,
        void *           _userdata);

// function definition for pythonic callback
typedef std::function<py::object(py::object,
                                 py::array_t<uint8_t>,
                                 py::array_t<uint8_t>,
                                 py::dict)> py_framesync_callback;
} // namespace liquid

#endif // __LIQUID_PYTHON_HH__

#endif // PYTHONLIB

