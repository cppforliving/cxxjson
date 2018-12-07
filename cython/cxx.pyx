# distutils: language = c++
# cython: c_string_type=str, c_string_encoding=ascii
# from cython.operator cimport dereference as deref
from cxxjson cimport byte
from cxxjson cimport json as cppjson
from cxxjson cimport byte_stream
from cxxjson cimport null_t
from cxxmsgpack cimport msgpack as cppmsgpack
from libc.stdint cimport int64_t, uint8_t
from libcpp cimport bool as cppbool
from libcpp.map cimport map
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef class json:
    cdef cppjson variant

    def __cinit__(self, value):
        cdef byte_stream data
        cdef vector[cppjson] array
        cdef map[string, cppjson] dictionary
        if isinstance(value, int):
            self.variant = cppjson(<int64_t>value)
        elif isinstance(value, str):
            self.variant = cppjson(<string>value)
        elif isinstance(value, float):
            self.variant = cppjson(<double>value)
        elif isinstance(value, bool):
            self.variant = cppjson(<cppbool>value)
        elif isinstance(value, bytes):
            data.reserve(len(value))
            for x in value:
                data.push_back(byte(<uint8_t>x))
            self.variant = cppjson(data)
        elif isinstance(value, list):
            array.reserve(len(value))
            for x in value:
                array.push_back(json(x).variant)
            self.variant = cppjson(array)
        elif isinstance(value, dict):
            for k, v in value.items():
                dictionary[<string>k] = json(v).variant
            self.variant = cppjson(dictionary)
        elif value is None:
            self.variant = cppjson(null_t())
        else:
            raise TypeError(f'unsupported type')


    def __eq__(self, value):
        if id(self) == id(value):
            return True
        if isinstance(value, json):
            return self.variant == (<json>value).variant
        return self.variant == json(value).variant


    def __ne__(self, value):
        return (self == value) != True

cdef class msgpack:
    cdef cppmsgpack object

    @staticmethod
    def encode(value):
        cdef byte_stream data = cppmsgpack.encode(json(value).variant)
        return bytes([<int>x for x in data])
