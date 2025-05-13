/***
 * @brief
 */

#define main __main__
#include "scaleblancer.cpp"
#undef main 

#include <catch2/catch_test_macros.hpp>
#include <sstream>

TEST_CASE("Integration: simple input produces correct output", "[integration]") {
    std::string input =
        "# Comment line\n"
        "A,2,B\n"
        "B,1,3\n";

    std::istringstream in(input);
    std::ostringstream out;

    std::vector<scale_wrapper> scales;
    parse_scales(in, scales);
    balance_each_scale(scales);
    report_changes(out, scales);

    std::string expected =
        "A,5,0\n"
        "B,2,0\n";

    REQUIRE(out.str() == expected);
}

TEST_CASE("Integration: balanced scale needs no adjustment", "[integration]") {
    std::string input = "S,5,5\n";
    std::istringstream in(input);
    std::ostringstream out;

    std::vector<scale_wrapper> scales;
    parse_scales(in, scales);
    balance_each_scale(scales);
    report_changes(out, scales);

    REQUIRE(out.str() == "S,0,0\n");
}

TEST_CASE("Integration: nested scale balancing", "[integration]") {
    std::string input =
        "Main,Sub,6\n"
        "Sub,4,4\n";

    std::istringstream in(input);
    std::ostringstream out;

    std::vector<scale_wrapper> scales;
    parse_scales(in, scales);
    balance_each_scale(scales);
    report_changes(out, scales);

    std::string expected =
        "Main,0,3\n"
        "Sub,0,0\n";

    REQUIRE(out.str() == expected);
}
