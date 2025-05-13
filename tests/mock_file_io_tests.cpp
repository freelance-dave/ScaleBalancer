/***
 * @brief
 */

#define main __main__
#include "scaleblancer.cpp"
#undef main 

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <sstream>

TEST_CASE("Mock file I/O: read and report correct balance", "[mock_io]") {
    std::string mock_input = R"(# Comment
Main,Left,Right
Left,3,1
Right,2,2
)";

    std::istringstream mock_istream(mock_input);
    std::ostringstream mock_ostream;

    std::vector<scale_wrapper> scales;
    parse_scales(mock_istream, scales);
    balance_each_scale(scales);
    report_changes(mock_ostream, scales);

    std::string output = mock_ostream.str();
    REQUIRE_THAT(output, Catch::Matchers::ContainsSubstring("Main,0,2"));
    REQUIRE_THAT(output, Catch::Matchers::ContainsSubstring("Left,0,2"));
    REQUIRE_THAT(output, Catch::Matchers::ContainsSubstring("Right,0,0"));
}

TEST_CASE("Mock file I/O: handles invalid and blank lines", "[mock_io]") {
    std::string mock_input = R"(# valid
Invalid,,Invalid
S1,5,S2
S2,5,5
)";

    std::istringstream mock_istream(mock_input);
    std::ostringstream mock_ostream;

    std::vector<scale_wrapper> scales;
    parse_scales(mock_istream, scales);
    balance_each_scale(scales);
    report_changes(mock_ostream, scales);

    std::string output = mock_ostream.str();
    REQUIRE_THAT(output, Catch::Matchers::ContainsSubstring("S1,6,0"));
    REQUIRE_THAT(output, Catch::Matchers::ContainsSubstring("S2,0,0"));
}

TEST_CASE("Mock file I/O: deeply nested scales", "[mock_io]") {
    std::string mock_input = R"(
A,B,1
B,C,2
C,3,4
)";

    std::istringstream mock_istream(mock_input);
    std::ostringstream mock_ostream;

    std::vector<scale_wrapper> scales;
    parse_scales(mock_istream, scales);
    balance_each_scale(scales);
    report_changes(mock_ostream, scales);

    std::string out = mock_ostream.str();
    REQUIRE_THAT(out, Catch::Matchers::ContainsSubstring("A,0,18"));
    REQUIRE_THAT(out, Catch::Matchers::ContainsSubstring("B,0,7"));
    REQUIRE_THAT(out, Catch::Matchers::ContainsSubstring("C,1,0"));
}
