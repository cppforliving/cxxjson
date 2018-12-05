from libcpp.string cimport string
from libc.stdint cimport int64_t, uint8_t
from libcpp cimport bool
from libcpp.vector cimport vector


cdef extern from "cstddef" namespace "std":
    cdef cppclass byte:
        byte()
        byte(uint8_t)


cdef extern from "cxx/json.hpp" namespace "cxx::json":
    cdef cppclass null_t:
        null_t()

        bool operator == (null_t)
        bool operator != (null_t)


cdef extern from "cxx/json.hpp" namespace "cxx":
    cdef cppclass json:
        json()
        json(bool)
        json(const string & ) except +
        json(double)
        json(int64_t)
        json(null_t)
        json(vector[byte])

        bool operator == (bool)
        bool operator == (const json&)
        bool operator == (const string&)
        bool operator == (double)
        bool operator == (int64_t)
        bool operator == (null_t)
        bool operator == (vector[byte])
