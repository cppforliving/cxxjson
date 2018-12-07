import pytest
from conftest import pymsgpack, values


@pytest.mark.parametrize('value', values)
def test_canImportCxxMsgpack(cxxmsgpack, value):
    assert value == pymsgpack.unpackb(cxxmsgpack.encode(value), raw=False)
