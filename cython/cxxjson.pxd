from libcpp.string cimport string
from libc.stdint cimport int64_t


cdef extern from "cxx/json.hpp" namespace "cxx":
    cdef cppclass json:
        json()
        json(bool)
        json(int64_t)
        json(double)
        json(const string&) except +
