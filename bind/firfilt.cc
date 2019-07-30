
#include "firfilt.hh"

firfilt::~firfilt()
{
    firfilt_crcf_destroy(q);
}

#ifdef PYTHONLIB
void firfilt::py_execute(py::array_t<std::complex<float>> & _buf)
{
    // get buffer info
    py::buffer_info info = _buf.request();

    // verify input size and dimensions
    if (info.itemsize != sizeof(std::complex<float>))
        throw std::runtime_error("invalid input numpy size, use dtype=np.csingle");
    if (info.ndim != 1)
        throw std::runtime_error("invalid number of input dimensions, must be 1-D array");

    // comptue sample size, number of samples in buffer, and stride between samples
    size_t       s           = sizeof(std::complex<float>);
    unsigned int num_samples = info.shape[0];
    unsigned int stride      = info.strides[0]/s;

    // convert buffers to pointers
    std::complex<float> * ptr = (std::complex<float>*) info.ptr;

    // run filter (in place)
    for (auto i=0; i<num_samples; i++)
        ptr[i*stride] = execute(ptr[i*stride]);
}

void init_firfilt(py::module &m)
{
    py::class_<firfilt> f(m, "firfilt");
    f.def(py::init<unsigned int,float,float,float>(),
          py::arg("n")=51, py::arg("fc")=0.25, py::arg("As")=60, py::arg("mu")=0)
     .def("reset",   &firfilt::reset,      "reset object's internal state")
     .def("execute", &firfilt::py_execute, "execute on a block of samples")
     ;
}

#endif

