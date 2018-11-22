# cython: c_string_type=str, c_string_encoding=ascii
# from cython.operator cimport dereference as deref
from libcpp.string cimport string
from cxxjson cimport json as cppjson
from cxxmsgpack cimport msgpack as cppmsgpack

cdef class json:
    cdef cppjson variant

    def __cinit__(self, value):
        if isinstance(value, int):
            self.variant = cppjson(<int>value)
        self.variant = cppjson()

cdef class msgpack:
    cdef cppmsgpack object
