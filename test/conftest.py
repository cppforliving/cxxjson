from pytest import fixture


@fixture
def cxxjson():
    from cxx import json
    return json


@fixture
def cxxmsgpack():
    from cxx import msgpack
    return msgpack
