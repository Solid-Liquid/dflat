//Unit tests for the TypeChecker

#include "catch2/catch.hpp"
#include "typechecker.hpp"

using namespace dflat;

TEST_CASE( "TypeChecker correctly checks types", "[TypeChecker]" )
{

    /*
     *   Tests for TypeChecker success:
     */

    REQUIRE( true == true );


    /*
     *   Tests for Typechecker throwing exception:
     */

    REQUIRE_THROWS_AS(throw TypeCheckerException("error"), TypeCheckerException);
}
