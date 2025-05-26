#include <concepts>
#include <iostream>

// Compare the first argument of the pack to all other args for same type
template<typename T, typename... Ts>
constexpr inline bool are_same_v = 
    std::conjunction_v<std::is_same<T, Ts>...>;

template<typename... Args>
requires are_same_v<Args...>
auto Add(Args&&... args) {
    return (... + args);
}

// Define new concept Multipliable
template<typename... Args>
concept Multipliable = requires(Args... args) {
    (... * args);
    requires are_same_v<Args...>;
    requires sizeof...(Args) > 1;
};

template<typename... Args>
requires Multipliable<Args...>
auto Multiply(Args&&... args) {
    return (... * args);
}

// Create a stub to test our Multipliable concept
template<bool nexcept, bool operatorStar, bool validReturnType>
struct Stub {
    Stub& operator*(const Stub& rhs) noexcept(nexcept)
        requires(operatorStar && validReturnType)
    { return *this; }

    int operator+(const Stub& rhs) noexcept(nexcept)
        requires(operatorStar && not validReturnType)
    { return {}; }
};

using NoMultiply = Stub<true, false, true>;
using ValidClass = Stub<true, true, true>;
using NotNoexcept = Stub<false, true, true>;
using DifferentReturnType = Stub<true, true, false>;

// Assert that mixed types are not allowed
static_assert(not Multipliable<int, double>);

// Assert that Add is used with at least two parameters
static_assert(not Multipliable<int>);

// Assert that type has operator*
static_assert(Multipliable<int, int>);
static_assert(Multipliable<ValidClass, ValidClass>);
static_assert(not Multipliable<NoMultiply, NoMultiply>);

// Assert that operator+ returns the same type
static_assert(
 not Multipliable<DifferentReturnType, DifferentReturnType>);

// Abbreviated Function Template
template<typename T>
concept Integral = std::integral<T>;
void Print(Integral auto value) {
    std::cout << value << std::endl;
}

int main() {
    auto result = Add(1, 2, 3, 4, 5);
    std::cout << result << std::endl;

    auto m = Multiply(2, 3, 4);
    std::cout << m << std::endl;

    Print(14);

    return 0;
}