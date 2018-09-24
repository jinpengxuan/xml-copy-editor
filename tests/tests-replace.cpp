#include "catch.hpp"
#include <string>
#include "../src/replace.h"

TEST_CASE( "replace works as specified", "[replace]" ) {
  std::string orig = "when shall we three meet again";
  std::string find = "THREE";
  std::string replace = "four";

  // case insensitive
  std::string buf = orig;
  REQUIRE( Replace::run(buf, find, replace, false) == 1 );
  REQUIRE( buf == "when shall we four meet again" );

  // case sensitive
  buf = orig;
  REQUIRE( Replace::run(buf, find, replace, true) == 0 );
  REQUIRE( buf == "when shall we three meet again" );
}
