from cxxjson cimport json as cppjson

cdef class json:
    cdef cppjson object
