from libcpp.string cimport string
from libc.stdint cimport int64_t
from libcpp cimport bool


cdef extern from "cxx/json.hpp" namespace "cxx":
    cdef cppclass json:
        json()
        json(bool)
        json(int64_t)
        json(double)
        json(const string&) except +

        bool operator==(int64_t)
        bool operator==(double)
        bool operator==(string)
        bool operator==(json)
