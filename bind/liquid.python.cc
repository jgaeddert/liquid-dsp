#include "liquid.python.hh"

#include "firfilt.hh"
#include "firinterp.hh"
#include "fg64.hh"
#include "fs64.hh"
#include "msresamp.hh"
#include "spwaterfall.hh"

namespace liquid {

void print_object(py::object o)
{
    if (py::isinstance<py::dict>(o)) {
        size_t i=0, n=py::dict(o).size();
        printf("{");
        for (auto p: py::dict(o)) {
            std::string k = py::cast<std::string>(p.first);
            auto        v = py::cast<py::object> (p.second);
            printf("\"%s\":", k.c_str());
            print_object(py::cast<py::object>(v));
            printf("%s", ++i==n ? "" : ",");
        }
        printf("}");
    } else if (py::isinstance<py::list>(o) || py::isinstance<py::tuple>(o)) {
        size_t i=0, n=py::list(o).size();
        printf("[");
        for (auto v: py::list(o)) {
            print_object(py::cast<py::object>(v));
            printf("%s", ++i==n ? "" : ",");
        }
        printf("]");
    } else if (py::isinstance<py::bool_>(o)) {
        printf("%s", bool(py::bool_(o)) ? "true" : "false");
    } else if (py::isinstance<py::int_>(o)) {
        printf("%d", int(py::int_(o)));
    } else if (py::isinstance<py::float_>(o)) {
        printf("%g", float(py::float_(o)));
    } else if (py::isinstance<py::none>(o)) {
        printf("null");
    } else if (py::isinstance<py::str>(o)) {
        printf("\"%s\"", std::string(py::str(o)).c_str());
    } else {
        printf("?\n");
    }
}

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

PYBIND11_MODULE(liquid, m) {
    m.doc() = "software-defined radio signal processing library";
    m.def("print_object", &print_object, "a function to recursively print a python object");

    // initialize objects
    liquid::init_firinterp  (m);
    liquid::init_firfilt    (m);
    liquid::init_fg64       (m);
    liquid::init_fs64       (m);
    liquid::init_msresamp   (m);
    liquid::init_spwaterfall(m);
}

} // namespace liquid

