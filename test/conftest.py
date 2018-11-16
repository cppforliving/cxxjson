from pytest import fixture


@fixture
def cxxjson():
    from cxx import json
    return json
