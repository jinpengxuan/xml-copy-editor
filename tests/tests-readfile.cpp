#include "catch.hpp"
#include <string>
#include "../src/readfile.h"

TEST_CASE( "readfile works as specified", "[readfile]" ) {
  std::string valid_path = "catch.hpp", invalid_path = "invalid.hpp";
  std::string buf;

  REQUIRE( ReadFile::run(valid_path, buf) == true );
  REQUIRE( buf.at(0) == '/' );

  buf.clear();
  REQUIRE( ReadFile::run(invalid_path, buf) == false );
  REQUIRE( buf.length() == 0 );
}
