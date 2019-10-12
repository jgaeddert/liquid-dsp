#ifndef __LIQUID_PYTHON_HH__
#define __LIQUID_PYTHON_HH__

#ifdef PYTHONLIB

#include <complex>
#include <pybind11/pybind11.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <liquid/liquid.h>
namespace py = pybind11;
using namespace pybind11::literals;

void print_object(py::object o);

#endif // __LIQUID_PYTHON_HH__

#endif // PYTHONLIB
