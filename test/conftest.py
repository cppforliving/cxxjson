from pytest import fixture
from itertools import combinations
import msgpack as pymsgpack

values = [
    42, 7, 3.14, 2.71, 'lorem', 'ipsum', True, False, None, b'lorem', b'ipsum', [], [
        'lorem', 42, 3.14, True, None, ['ipsum']], dict(), {
            'lorem': 'ipsum', 'dolor': 42, 'sit': 3.14, 'amet': [
                True, None], 'consectetur':{
                    'adipisicing': 'elit'}}]
pairs = tuple(combinations(values, 2))


@fixture
def cxxjson():
    from cxx import json
    return json


@fixture
def cxxmsgpack():
    from cxx import msgpack
    return msgpack
