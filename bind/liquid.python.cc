#include "liquid.python.hh"

#include "firfilt.hh"
#include "firinterp.hh"
#include "firpfbch2.hh"
#include "fg64.hh"
#include "fs64.hh"
#include "msresamp.hh"
#include "spgram.hh"
#include "spwaterfall.hh"

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

PYBIND11_MODULE(liquid, m) {
    m.doc() = "software-defined radio signal processing library";

    // initialize objects
    liquid::init_firfilt    (m);
    liquid::init_firpfbch2a (m);
    liquid::init_firpfbch2s (m);
    liquid::init_firinterp  (m);
    liquid::init_fg64       (m);
    liquid::init_fs64       (m);
    liquid::init_msresamp   (m);
    liquid::init_spgram     (m);
    liquid::init_spwaterfall(m);
}

} // namespace liquid

