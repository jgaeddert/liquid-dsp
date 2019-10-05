#ifndef __LIQUID_PYTHON_HH__
#define __LIQUID_PYTHON_HH__

#ifdef PYTHONLIB
#include <complex>
#include <pybind11/pybind11.h>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <liquid/liquid.h>
namespace py = pybind11;
using namespace pybind11::literals;

void dict_update(py::dict & a, py::dict & b);
void print_object(py::object o);

void init_firfilt(py::module &);
#endif // PYTHONLIB

#endif // __LIQUID_PYTHON_HH__
