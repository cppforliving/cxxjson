# cython: c_string_type=str, c_string_encoding=ascii
# from cython.operator cimport dereference as deref
from libcpp.string cimport string
from libc.stdint cimport int64_t
from libcpp cimport bool as cppbool
from cxxjson cimport json as cppjson
from cxxjson cimport null_t
from cxxmsgpack cimport msgpack as cppmsgpack


cdef class json:
    cdef cppjson variant

    def __cinit__(self, value):
        if isinstance(value, int):
            self.variant = cppjson(<int64_t>value)
        elif isinstance(value, str):
            self.variant = cppjson(<string>value)
        elif isinstance(value, float):
            self.variant = cppjson(<double>value)
        elif isinstance(value, bool):
            self.variant = cppjson(<cppbool>value)
        elif value is None:
            self.variant = cppjson(null_t())


    def __eq__(self, value):
        if id(self) == id(value):
            return True
        if isinstance(value, json):
            return self.variant == (<json>value).variant
        if isinstance(value, int):
            return self.variant == <int64_t>value
        if isinstance(value, str):
            return self.variant == <string>value
        if isinstance(value, float):
            return self.variant == <double>value
        if isinstance(value, bool):
            return self.variant == <cppbool>value
        if value is None:
            return self.variant == null_t()
        return False

    def __ne__(self, value):
        return (self == value) != True

cdef class msgpack:
    cdef cppmsgpack object
