# cython: c_string_type=str, c_string_encoding=ascii
# from cython.operator cimport dereference as deref
from libcpp.string cimport string
from libc.stdint cimport int64_t
from cxxjson cimport json as cppjson
from cxxmsgpack cimport msgpack as cppmsgpack


cdef class json:
    cdef cppjson variant

    def __cinit__(self, value):
        if isinstance(value, int):
            self.variant = cppjson(<int64_t>value)
        elif isinstance(value, str):
            self.variant = cppjson(<string>value)

    def __eq__(self, value):
        if id(self) == id(value): return True
        if isinstance(value, json):
            return self.variant == (<json>value).variant
        if isinstance(value, int):
            return self.variant == <int64_t>value
        if isinstance(value, str):
            return self.variant == <string>value
        return False

    def __ne__(self, value):
        return (self == value) != True

cdef class msgpack:
    cdef cppmsgpack object
