from cxxjson cimport byte_stream
from cxxjson cimport json

cdef extern from "cxx/msgpack.hpp" namespace "cxx":
    cdef cppclass msgpack:
        @staticmethod
        byte_stream encode(const json&) except +

        @staticmethod
        json decode(const byte_stream&) except +
