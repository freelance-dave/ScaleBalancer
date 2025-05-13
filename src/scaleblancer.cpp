/**
 * @file scaleblancer.cpp
 * @brief ScaleBalancer - a balancing system for composite scales.
 *
 * This application models a recursive system of mechanical scales where each scale
 * can hold either a physical weight (Pan) or another scale on its left and right sides.
 *
 * The input is a series of CSV lines (from stdin) of the form:
 *     scale_name,left_side,right_side
 * where each side can either be a numeric weight or the name of another scale.
 *
 * The program parses this input into a tree of interconnected Scale and Pan objects.
 * It then computes the additional balancing mass needed on each side to ensure equilibrium.
 *
 * The results are output as CSV lines (to stdout) showing:
 *     scale_name,left_balance_mass,right_balance_mass
 */

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <ranges>

/**
 * @brief Represents a pan with weight and optional counterbalance.
 */
struct Pan {
    static constexpr int default_mass{0}; ///< Default mass for a pan (0).
    int mass{};           ///< Weight placed on the pan.
    int balance_mass{};   ///< Additional mass added for balancing.

    /**
     * @brief Constructs a Pan with optional initial weight.
     * @param kg Initial mass (default is 0).
     */
    explicit Pan(int kg = default_mass) : mass{kg} {}
};

/**
 * @brief A scale can can hold either a Pan or a weak reference to another Scale.
 */

struct Scale;
using scale_wrapper = std::shared_ptr<Scale>;
using pan_or_scale = std::variant<Pan, std::weak_ptr<Scale>>;

/**
 * @brief Represents a composite scale which can contain Pans or other Scales on each side.
 */
struct Scale final : Pan {
    static constexpr int default_mass{1}; ///< Default self-mass for a Scale.
    std::string name;                     ///< Identifier of the scale.
    pan_or_scale left;                    ///< Left side: Pan or linked Scale.
    pan_or_scale right;                   ///< Right side: Pan or linked Scale.

    /**
     * @brief Constructs a Scale with a name.
     * @param n The name of the scale.
     */
    explicit Scale(std::string n)
        : Pan{default_mass}, name{std::move(n)},
          left{std::in_place_type<Pan>},
          right{std::in_place_type<Pan>} {}

    /**
     * @brief Resolves a mutable pan_or_scale variant to a reference to the underlying Pan.
     * @param side A variant holding either a Pan or weak_ptr to Scale.
     * @return Reference to the resolved Pan.
     */
    static Pan& resolve_side(pan_or_scale& side) {
        return std::holds_alternative<Pan>(side)
            ? std::get<Pan>(side)
            : *std::get<std::weak_ptr<Scale>>(side).lock();
    }

    /**
     * @brief Resolves a const pan_or_scale variant to a reference to the underlying Pan.
     * @param side A variant holding either a Pan or weak_ptr to Scale.
     * @return Const reference to the resolved Pan.
     */
    static const Pan& resolve_side(const pan_or_scale& side) {
        return std::holds_alternative<Pan>(side)
            ? std::get<Pan>(side)
            : *std::get<std::weak_ptr<Scale>>(side).lock();
    }
};

/**
 * @brief Parses a CSV line of format "name,left,right" and returns trimmed tokens.
 * @param line The input line string.
 * @return Tuple containing name, left, and right strings.
 */
inline std::tuple<std::string, std::string, std::string> parse_line(const std::string& line) {
    auto parts = line
        | std::views::split(',')
        | std::views::transform([](auto&& r) {
            std::string token(&*r.begin(), std::ranges::distance(r));
            std::erase_if(token, ::isspace);
            return token;
        });

    auto it = parts.begin();
    const auto name  = it != parts.end() ? *it++ : "";
    const auto left  = it != parts.end() ? *it++ : "";
    const auto right = it != parts.end() ? *it++ : "";
    return {name, left, right};
}

/**
 * @brief Parses input stream to construct a list of interconnected scales.
 * @param infile Input stream containing scale definitions.
 * @param scales_list Output vector to hold the constructed scales.
 */
inline void parse_scales(std::istream& infile, std::vector<scale_wrapper>& scales_list) {
    scales_list.clear();
    std::unordered_map<std::string, scale_wrapper> known_scales;

    auto get_or_create_scale = [&](const std::string& name) -> scale_wrapper {
        if (auto it = known_scales.find(name); it != known_scales.end()) {
            return it->second;
        }
        auto scale = std::make_shared<Scale>(name);
        known_scales[name] = scale;
        scales_list.push_back(scale);
        return scale;
    };

    auto assign_side = [&](auto& side, const std::string& token) {
        if (!token.empty() && std::isdigit(token.front())) {
            side.template emplace<Pan>(std::stoi(token));
        } else if (!token.empty()) {
            side.template emplace<std::weak_ptr<Scale>>(get_or_create_scale(token));
        }
    };

    std::string line;
    for (int line_number = 0;std::getline(infile, line); ++line_number) {
        if (line.empty() || line.front() == '#') continue; // skip comment lines.

        const auto [name, left, right] = parse_line(line);

        // Valiate the parsed scales parameters
        if (name.empty() || left == name || right == name) {
            std::cerr << "Invalid line " << line_number << ": " << std::quoted(line) << '\n';
            continue;
        }

        // Add/update the referenced scale.
        auto scale = get_or_create_scale(name);
        assign_side(scale->left, left);
        assign_side(scale->right, right);
    }
}

/**
 * @brief Balances all scales by computing and assigning necessary counterweights.
 * @param scales_list A span of scales to balance.
 */
inline void balance_each_scale(std::span<scale_wrapper> scales_list) {
    for (const auto& scale : scales_list | std::views::reverse) {
        auto& left_pan = Scale::resolve_side(scale->left);
        auto& right_pan = Scale::resolve_side(scale->right);

        if (left_pan.mass > right_pan.mass)
            right_pan.balance_mass = left_pan.mass - right_pan.mass;
        else if (right_pan.mass > left_pan.mass)
            left_pan.balance_mass = right_pan.mass - left_pan.mass;

        scale->mass += left_pan.mass + right_pan.mass
                     + left_pan.balance_mass + right_pan.balance_mass;
    }
}

/**
 * @brief Outputs the balancing results for each scale to an output stream.
 * @param os The output stream.
 * @param scales_list The list of scales to report on.
 */
inline void report_changes(std::ostream& os, std::span<scale_wrapper> scales_list) {
    for (const auto& scale : scales_list) {
        const auto& left_pan = Scale::resolve_side(scale->left);
        const auto& right_pan = Scale::resolve_side(scale->right);
        os << scale->name << ',' << left_pan.balance_mass << ',' << right_pan.balance_mass << '\n';
    }
}

/**
 * @brief Entry point of the ScaleBalancer application.
 *
 * Reads input from standard input, constructs and balances a set of interconnected scales,
 * then writes the balancing results to standard output.
 */
int main()
{
    std::vector<scale_wrapper> scales_list;

    // Parse input lines to build the list of interconnected scales
    parse_scales(std::cin, scales_list);

    // Compute necessary balancing masses for each scale
    balance_each_scale(scales_list);

    // Output the balancing results to standard output
    report_changes(std::cout, scales_list);

    return 0;
}
