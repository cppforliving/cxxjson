# distutils: language = c++
# cython: c_string_type=str, c_string_encoding=ascii
# from cython.operator cimport dereference as deref
from cxxjson cimport byte
from cxxjson cimport json as cppjson
from cxxjson cimport byte_stream
from cxxjson cimport null_t
from cxxmsgpack cimport msgpack as cppmsgpack
from libc.stdint cimport int64_t, uint8_t
cimport libcpp
from libcpp.map cimport map
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef class json:
    cdef cppjson variant

    def __cinit__(self, value):
        class converter:
            @staticmethod
            def from_int(input):
                self.variant = cppjson( < int64_t > input)

            @staticmethod
            def from_float(input):
                self.variant = cppjson( < double > input)

            @staticmethod
            def from_bool(input):
                self.variant = cppjson( < libcpp.bool > input)

            @staticmethod
            def from_str(input):
                self.variant = cppjson( < string > input)

            @staticmethod
            def from_NoneType(input):
                self.variant = cppjson(null_t())

            @staticmethod
            def from_bytes(input):
                cdef byte_stream data
                data.reserve(len(input))
                for x in input:
                    data.push_back(byte( < uint8_t > x))
                self.variant = cppjson(data)

            @staticmethod
            def from_list(input):
                cdef vector[cppjson] array
                array.reserve(len(input))
                for x in input:
                    array.push_back(json(x).variant)
                self.variant = cppjson(array)

            @staticmethod
            def from_dict(input):
                cdef map[string, cppjson] dictionary
                for k, v in input.items():
                    dictionary[ < string > k] = json(v).variant
                self.variant = cppjson(dictionary)

            @staticmethod
            def unknown(input):
                raise TypeError(f'unsupported type: from_{type(input).__name__}')
        conv = getattr(converter, f'from_{type(value).__name__}', converter.unknown)
        conv(value)

    def __eq__(self, value):
        if id(self) == id(value):
            return True
        if isinstance(value, json):
            return self.variant == ( < json > value).variant
        return self.variant == json(value).variant

    def __ne__(self, value):
        return (self == value) != True

cdef class msgpack:
    cdef cppmsgpack object

    @staticmethod
    def encode(value):
        cdef byte_stream data = cppmsgpack.encode(json(value).variant)
        return bytes([ < int > x for x in data])

    @staticmethod
    def decode(value):
        cdef byte_stream data
        data.reserve(len(value))
        for x in value:
            data.push_back(byte( < uint8_t > x))
        ret = json(None)
        ret.variant = cppmsgpack.decode(data)
        return ret
