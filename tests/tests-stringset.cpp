#include "catch.hpp"
#include <string>
#include <memory>
#include "../src/stringset.h"

TEST_CASE( "stringset works as specified", "[stringset]" ) {
  std::string buf = "test";
  std::auto_ptr<StringSet<char> > dict ( new StringSet<char>() );

  REQUIRE( dict->find(buf) == 0 );
  REQUIRE( dict->empty() == true );
  REQUIRE( dict->count() == 0 );
  
  dict->insert(buf);

  REQUIRE( dict->find(buf) != 0 );
  REQUIRE( dict->empty() == false);
  REQUIRE( dict->count() == 1);
}
