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

// forward declaration of object initialization methods
namespace liquid {
void init_firfilt(py::module &m);
void init_fs64   (py::module &m);
void init_fg64   (py::module &m);
} // namespace liquid

#endif // __LIQUID_PYTHON_HH__

#endif // PYTHONLIB
