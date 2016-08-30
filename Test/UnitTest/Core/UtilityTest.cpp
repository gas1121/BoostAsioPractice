#include "Core/Utility.hpp"

#include <catch.hpp>

using namespace BoostAsioPractice;

TEST_CASE("RandomNumber test", "[Core]")
{
    REQUIRE(RandomNumber<int>(7, 7) == 7);
    int testResult = RandomNumber<int>(0, 7);
    REQUIRE(testResult < 8);
    REQUIRE(testResult > -1);
}