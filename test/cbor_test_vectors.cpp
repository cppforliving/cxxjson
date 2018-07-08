#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"
#include <cmath>

using namespace cxx::literals;
using namespace test::literals;

using cbor = cxx::cbor;

TEST_CASE("official cbor test vectors encoding")
{
  REQUIRE(cbor::encode(0) == "00"_hex);
  REQUIRE(cbor::encode(1) == "01"_hex);
  REQUIRE(cbor::encode(10) == "0a"_hex);
  REQUIRE(cbor::encode(23) == "17"_hex);
  REQUIRE(cbor::encode(24) == "1818"_hex);
  REQUIRE(cbor::encode(25) == "1819"_hex);
  REQUIRE(cbor::encode(100) == "1864"_hex);
  REQUIRE(cbor::encode(1000) == "1903e8"_hex);
  REQUIRE(cbor::encode(1000000) == "1a000f4240"_hex);
  REQUIRE(cbor::encode(1000000000000) == "1b000000e8d4a51000"_hex);
  // REQUIRE(cbor::encode(18446744073709551615) == "1bffffffffffff,
  // 0xffff"_hex);
  // error: integer constant is so large that it is unsigned
  // REQUIRE(cbor::encode(/*bignum*/18446744073709551616) ==
  //         "c249010000000000000000"_hex);
  // error: integer constant is too large for its type
  // REQUIRE(cbor::encode(-18446744073709551616) ==
  //         "3bffffffffffffffff"_hex);
  // error: integer constant is too large for its type
  // REQUIRE(cbor::encode(-18446744073709551617) ==
  //         "c349010000000000000000"_hex);
  // error: integer constant is too large for its type
  REQUIRE(cbor::encode(-1) == "20"_hex);
  REQUIRE(cbor::encode(-10) == "29"_hex);
  REQUIRE(cbor::encode(-100) == "3863"_hex);
  REQUIRE(cbor::encode(-1000) == "3903e7"_hex);
  // REQUIRE(cbor::encode(-0.0) == "f98000"_hex);
  // sizeof(double) == 8
  REQUIRE(cbor::encode(1.0e+300) == "fb7e37e43c8800759c"_hex);
  REQUIRE(cbor::encode(false) == "f4"_hex);
  REQUIRE(cbor::encode(true) == "f5"_hex);
  REQUIRE(cbor::encode(cxx::json::null) == "f6"_hex);
  REQUIRE(cbor::encode("") == "60"_hex);
  REQUIRE(cbor::encode("a") == "6161"_hex);
  REQUIRE(cbor::encode("IETF") == "6449455446"_hex);
  REQUIRE(cbor::encode("\"\\") == "62225c"_hex);
  REQUIRE(cbor::encode("ü") == "62c3bc"_hex);
  REQUIRE(cbor::encode("水") == "63e6b0b4"_hex);
  REQUIRE(cbor::encode("𐅑") == "64f0908591"_hex);
  REQUIRE(cbor::encode(cxx::json::array()) == "80"_hex);
  REQUIRE(cbor::encode({1, 2, 3}) == "83010203"_hex);
  REQUIRE(cbor::encode({1, {2, 3}, {4, 5}}) == "8301820203820405"_hex);
  REQUIRE(cbor::encode({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}) ==
          "98190102030405060708090a0b0c0d0e0f101112131415161718181819"_hex);
  REQUIRE(cbor::encode(cxx::json::dictionary()) == "a0"_hex);
  REQUIRE(cbor::encode({"a"_key >> 1, {"b"_key, {2, 3}}}) == "a26161016162820203"_hex);
  REQUIRE(cbor::encode({"a", {"b"_key >> "c"}}) == "826161a161626163"_hex);
  REQUIRE(cbor::encode({"a"_key >> "A", "b"_key >> "B", "c"_key >> "C", "d"_key >> "D",
                        "e"_key >> "E"}) == "a56161614161626142616361436164614461656145"_hex);
}

TEST_CASE("official cbor test vectors decoding")
{
  REQUIRE(cbor::decode("00"_hex) == 0);
  REQUIRE(cbor::decode("01"_hex) == 1);
  REQUIRE(cbor::decode("0a"_hex) == 10);
  REQUIRE(cbor::decode("17"_hex) == 23);
  REQUIRE(cbor::decode("1818"_hex) == 24);
  REQUIRE(cbor::decode("1819"_hex) == 25);
  REQUIRE(cbor::decode("1864"_hex) == 100);
  REQUIRE(cbor::decode("1903e8"_hex) == 1000);
  REQUIRE(cbor::decode("1a000f4240"_hex) == 1000000);
  REQUIRE(cbor::decode("1b000000e8d4a51000"_hex) == 1000000000000);
  REQUIRE_THROWS_AS(cbor::decode("1bffffffffffffffff"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("c249010000000000000000"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("3bffffffffffffffff"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("c349010000000000000000"_hex), cbor::unsupported);
  REQUIRE(cbor::decode("20"_hex) == -1);
  REQUIRE(cbor::decode("29"_hex) == -10);
  REQUIRE(cbor::decode("3863"_hex) == -100);
  REQUIRE(cbor::decode("3903e7"_hex) == -1000);
  REQUIRE_THROWS_AS(cbor::decode("f90000"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f98000"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f93c00"_hex), cbor::unsupported);
  REQUIRE(cbor::decode("fb3ff199999999999a"_hex) == 1.1);
  REQUIRE_THROWS_AS(cbor::decode("f93e00"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f97bff"_hex), cbor::unsupported);
  REQUIRE(cbor::decode("fa47c35000"_hex) == 100000.0);
  REQUIRE(cbor::decode("fa7f7fffff"_hex) == 3.4028234663852886e+38);
  REQUIRE(cbor::decode("fb7e37e43c8800759c"_hex) == 1.0e+300);
  REQUIRE_THROWS_AS(cbor::decode("f90001"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f90400"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f9c400"_hex), cbor::unsupported);
  REQUIRE(cbor::decode("fbc010666666666666"_hex) == -4.1);
  REQUIRE_THROWS_AS(cbor::decode("f97c00"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f97e00"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f9fc00"_hex), cbor::unsupported);
  REQUIRE(!std::isfinite(cxx::get<double>(cbor::decode("fa7f800000"_hex))));
  REQUIRE(std::isnan(cxx::get<double>(cbor::decode("fa7fc00000"_hex))));
  REQUIRE(!std::isfinite(cxx::get<double>(cbor::decode("faff800000"_hex))));
  REQUIRE(std::isnan(cxx::get<double>(cbor::decode("fb7ff8000000000000"_hex))));
  REQUIRE(!std::isfinite(cxx::get<double>(cbor::decode("fbfff0000000000000"_hex))));
  REQUIRE(cbor::decode("f4"_hex) == false);
  REQUIRE(cbor::decode("f5"_hex) == true);
  REQUIRE(cbor::decode("f6"_hex) == cxx::json::null);
  REQUIRE_THROWS_AS(cbor::decode("f7"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f0"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f818"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("f8ff"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("c074323031332d30332d32315432303a30343a30305a"_hex),
                    cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("c11a514b67b0"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("c1fb41d452d9ec200000"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("d74401020304"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("d818456449455446"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("d82076687474703a2f2f7777772e6578616d706c652e636f6d"_hex),
                    cbor::unsupported);
  REQUIRE(cbor::decode("40"_hex) == ""_hex);
  REQUIRE(cbor::decode("4401020304"_hex) == "01020304"_hex);
  REQUIRE(cbor::decode("60"_hex) == "");
  REQUIRE(cbor::decode("6161"_hex) == "a");
  REQUIRE(cbor::decode("6449455446"_hex) == "IETF");
  REQUIRE(cbor::decode("62225c"_hex) == "\"\\");
  REQUIRE(cbor::decode("62c3bc"_hex) == "ü");
  REQUIRE(cbor::decode("63e6b0b4"_hex) == "水");
  REQUIRE(cbor::decode("64f0908591"_hex) == "𐅑");
  REQUIRE(cbor::decode("80"_hex) == cxx::json::array());
  REQUIRE(cbor::decode("83010203"_hex) == cxx::json::array({1, 2, 3}));
  REQUIRE(cbor::decode("8301820203820405"_hex) ==
          cxx::json::array({1, cxx::json::array({2, 3}), cxx::json::array({4, 5})}));
  REQUIRE(cbor::decode("98190102030405060708090a0b0c0d0e0f101112131415161718181819"_hex) ==
          cxx::json::array({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                            14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}));
  REQUIRE(cbor::decode("a0"_hex) == cxx::json::dictionary());
  REQUIRE_THROWS_AS(cbor::decode("a201020304"_hex), cbor::unsupported);
  REQUIRE(cbor::decode("a26161016162820203"_hex) ==
          cxx::json::dictionary({{"a", 1}, {"b", cxx::json::array({2, 3})}}));
  REQUIRE(cbor::decode("826161a161626163"_hex) ==
          cxx::json::array({"a", cxx::json::dictionary({{"b", "c"}})}));
  REQUIRE(cbor::decode("a56161614161626142616361436164614461656145"_hex) ==
          cxx::json::dictionary({{"a", "A"}, {"b", "B"}, {"c", "C"}, {"d", "D"}, {"e", "E"}}));
  REQUIRE_THROWS_AS(cbor::decode("5f42010243030405ff"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("7f657374726561646d696e67ff"_hex), cbor::unsupported);
  REQUIRE(cbor::decode("9fff"_hex) == cxx::json::array());
  REQUIRE(cbor::decode("9f018202039f0405ffff"_hex) ==
          cxx::json::array({1, cxx::json::array({2, 3}), cxx::json::array({4, 5})}));
  REQUIRE(cbor::decode("9f01820203820405ff"_hex) ==
          cxx::json::array({1, cxx::json::array({2, 3}), cxx::json::array({4, 5})}));
  REQUIRE(cbor::decode("83018202039f0405ff"_hex) ==
          cxx::json::array({1, cxx::json::array({2, 3}), cxx::json::array({4, 5})}));
  REQUIRE(cbor::decode("83019f0203ff820405"_hex) ==
          cxx::json::array({1, cxx::json::array({2, 3}), cxx::json::array({4, 5})}));
  REQUIRE(cbor::decode("9f0102030405060708090a0b0c0d0e0f101112131415161718181819ff"_hex) ==
          cxx::json::array({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                            14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}));
  REQUIRE_THROWS_AS(cbor::decode("bf61610161629f0203ffff"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("826161bf61626163ff"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("bf6346756ef563416d7421ff"_hex), cbor::unsupported);
}
