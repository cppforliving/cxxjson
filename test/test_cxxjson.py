def test_canImportCxxJson(cxxjson):
    json = cxxjson(42)
    assert json == json
    assert json == 42
    assert json != "lorem ipsum"


def test_canImportCxxMsgpack(cxxmsgpack):
    pass
