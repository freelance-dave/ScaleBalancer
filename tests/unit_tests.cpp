/***
 * @brief
 */

#define main __main__
#include "scaleblancer.cpp"
#undef main 

#include <catch2/catch_test_macros.hpp>
#include <sstream>

TEST_CASE("Pan initializes correctly", "[Pan]") {
    Pan p1;
    REQUIRE(p1.mass == 0);
    REQUIRE(p1.balance_mass == 0);

    Pan p2(5);
    REQUIRE(p2.mass == 5);
}

TEST_CASE("Scale initializes with default pan values", "[Scale]") {
    Scale scale("TestScale");
    REQUIRE(scale.name == "TestScale");

    REQUIRE(std::holds_alternative<Pan>(scale.left));
    REQUIRE(std::holds_alternative<Pan>(scale.right));
}

TEST_CASE("parse_line extracts trimmed tokens", "[parse_line]") {
    auto [name, left, right] = parse_line("  Scale1 , 3 , Scale2  ");
    REQUIRE(name == "Scale1");
    REQUIRE(left == "3");
    REQUIRE(right == "Scale2");
}

TEST_CASE("parse_scales builds structure from input", "[parse_scales]") {
    std::string input =
        "A,2,B\n"
        "B,1,3\n";
    std::istringstream iss(input);
    std::vector<scale_wrapper> scales;
    parse_scales(iss, scales);

    REQUIRE(scales.size() == 2);
    REQUIRE(scales[0]->name == "A");
    REQUIRE(scales[1]->name == "B");
}

TEST_CASE("balance_each_scale assigns correct counterweights", "[balance]") {
    auto a = std::make_shared<Scale>("A");
    a->left = Pan(4);
    a->right = Pan(2);

    std::vector<scale_wrapper> list = { a };
    balance_each_scale(list);

    const auto& l = std::get<Pan>(a->left);
    const auto& r = std::get<Pan>(a->right);
    REQUIRE(l.balance_mass == 0);
    REQUIRE(r.balance_mass == 2);
    REQUIRE(a->mass == 8 + Scale::default_mass); // 4 + 2 + 2 + scale_mass (balance)
}

/**
 * Edge Case Tests
 */

TEST_CASE("Empty and comment lines are ignored", "[parse_scales][edge]") {
    std::string input = "\n# This is a comment\n\n";
    std::istringstream iss(input);
    std::vector<scale_wrapper> scales;
    parse_scales(iss, scales);

    REQUIRE(scales.empty());
}

TEST_CASE("Scales with self-reference are rejected", "[parse_scales][edge]") {
    std::string input = "X,X,2\nY,3,Y\n";
    std::istringstream iss(input);
    std::vector<scale_wrapper> scales;
    parse_scales(iss, scales);

    REQUIRE(scales.empty());
}

TEST_CASE("Unbalanced scale requires maximum counterweight", "[balance][edge]") {
    auto scale = std::make_shared<Scale>("HeavyRight");
    scale->left = Pan(1);
    scale->right = Pan(1000);

    std::vector<scale_wrapper> list = { scale };
    balance_each_scale(list);

    const auto& l = std::get<Pan>(scale->left);
    const auto& r = std::get<Pan>(scale->right);
    REQUIRE(l.balance_mass == 999);
    REQUIRE(r.balance_mass == 0);
    REQUIRE(scale->mass == 1001 + 1 + 999);
}

TEST_CASE("Recursive scale dependencies resolve correctly", "[parse_scales][balance][complex]") {
    std::string input =
        "Top,Mid,1\n"
        "Mid,2,3\n";
    std::istringstream iss(input);
    std::vector<scale_wrapper> scales;
    parse_scales(iss, scales);
    balance_each_scale(scales);

    auto top = scales[0];
    const auto& mid = std::get<std::weak_ptr<Scale>>(top->left).lock();
    const auto& left_mid = std::get<Pan>(mid->left);
    const auto& right_mid = std::get<Pan>(mid->right);

    REQUIRE(left_mid.mass == 2);
    REQUIRE(right_mid.mass == 3);
    REQUIRE(left_mid.balance_mass == 1);
    REQUIRE(right_mid.balance_mass == 0);
    REQUIRE(mid->mass == 6 + Scale::default_mass); // 2 + 3 + 1 + scale_mass
}
