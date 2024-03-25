# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "1.6.0"

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

ext_modules = [
    Pybind11Extension(
        name="liquid",
        sources=["bind/liquid.python.cc", ],
        libraries=["m","c","fftw3f"],
        include_dirs=['.','./include','./bind'],
        define_macros=[("VERSION_INFO", __version__),
                       ("LIQUID_PYTHONLIB", 1)],
        extra_compile_args=["-Wall",
                            "-Wno-deprecated",
                            "-Wno-deprecated-declarations",
                            "-Wno-return-type-c-linkage"],
        extra_objects=["libliquid.ar"],
    ),
]

setup(
    name="liquid",
    version=__version__,
    author="Joseph D. Gaeddert",
    author_email="joseph@liquidsdr.org",
    url="https://liquidsdr.org",
    description="Software-Defined Radio Digital Signal Processing Library",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)

