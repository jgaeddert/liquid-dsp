#ifndef __LIQUID_PYTHON_HH__
#define __LIQUID_PYTHON_HH__

#ifdef LIQUID_PYTHONLIB

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

// get instance type as string (debugging)
std::string get_instance_as_string(const py::object & object);

// validate and update
py::dict update_dict(py::dict dst, py::dict src);

/*! @brief copy object contents to byte array to use for frame encoding
 *  @param object input python object (array, string. etc.)
 *  @param buffer reference to fixed-size output buffer
 *  @param pad_value value used to pad if object size is too small
 *  @param error_on_truncate flag indicating if error should be thrown if object is truncated
 */
void py_copy_object_to_array(const py::object &     object,
                             std::vector<uint8_t> & buffer,
                             uint8_t                pad_value = '\0',
                             bool                   error_on_truncate = false);

#if 0
/*! @brief copy object contents to output and resize as needed
 *  @param object input python object (array, string. etc.)
 *  @param buffer reference to output buffer which will be resized as needed
 */
void py_copy_object_to_vector(const py::object &     object,
                              py::array_t<uint8_t> & buffer);
#endif

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

#endif // LIQUID_PYTHONLIB

