def test_canImportCxxJson(cxxjson):
    json = cxxjson(42)
    same = cxxjson(42)
    other = cxxjson("lorem ipsum")
    assert json == json
    assert json == 42
    assert same == 42
    assert json == same
    assert json != other
    assert json != cxxjson(7)
    assert json != "lorem ipsum"


def test_canCreateCxxJsonFromInt(cxxjson):
    json = cxxjson(42)
    assert json == 42


def test_canCompateCxxJsonToItSelf(cxxjson):
    json = cxxjson(42)
    assert json == json
    assert (json != json) is False


def test_canCompareCxxJsonToInt(cxxjson):
    assert cxxjson(42) == 42
    assert (cxxjson(42) != 42) is False
    assert (cxxjson(42) == 77) is False
    assert cxxjson(42) != 77


def test_canImportCxxMsgpack(cxxmsgpack):
    pass
