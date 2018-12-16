import pytest
from conftest import pymsgpack, values
from binascii import hexlify


@pytest.mark.parametrize('value', values)
def test_canTranscodeCxxMsgpackEncoder(cxxmsgpack, value):
    assert value == pymsgpack.unpackb(cxxmsgpack.encode(value), raw=False)


@pytest.mark.parametrize('value', values)
def test_canTranscodeCxxMsgpackDecoder(cxxmsgpack, cxxjson, value):
    data = pymsgpack.packb(value, use_bin_type=True)
    assert cxxmsgpack.decode(data) == value, f'failed to decode {hexlify(data)}'
