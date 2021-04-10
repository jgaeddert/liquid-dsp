#ifndef __LIQUID_PYTHON_HH__
#define __LIQUID_PYTHON_HH__

#ifdef PYTHONLIB

#include <complex>
#include <pybind11/pybind11.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "liquid.h"
namespace py = pybind11;
using namespace pybind11::literals;

namespace liquid {

void print_object(py::object o);

// validate keys ('dst' cannot contain any keys not in 'src')
bool validate_dict(py::dict dst, py::dict src);

// validate and update
py::dict update_dict(py::dict dst, py::dict src);

} // namespace liquid

#endif // __LIQUID_PYTHON_HH__

#endif // PYTHONLIB

