#include "polynomial_format.hpp"
#include "testing.hpp"

namespace
{
    std::initializer_list<int> wrap(std::initializer_list<int> il)
    {
        return il;
    }
}

void TestZero()
{
    EXPECT_EQ(polynomial_to_string("t", wrap({})), "0");
}

void TestConstant()
{
    EXPECT_EQ(polynomial_to_string("t", wrap({ 1})),  "1");
    EXPECT_EQ(polynomial_to_string("t", wrap({-1})), "-1");
    EXPECT_EQ(polynomial_to_string("t", wrap({15})), "15");
}

void TestMonic()
{
    EXPECT_EQ(polynomial_to_string("t", wrap({0, 1})),  "t");
    EXPECT_EQ(polynomial_to_string("t", wrap({-3, 2, 1})),  "t^2 + 2 * t - 3");
    EXPECT_EQ(polynomial_to_string("t", wrap({3, -2, 1})),  "t^2 - 2 * t + 3");
}

void TestOther()
{
    EXPECT_EQ(polynomial_to_string("t", wrap({0, 0, 8, -3})),  "-3 * t^3 + 8 * t^2");
    EXPECT_EQ(polynomial_to_string("t", wrap({-2, 1, -7, 4})),  "4 * t^3 - 7 * t^2 + t - 2");
    EXPECT_EQ(polynomial_to_string("t", wrap({2, -3})),  "-3 * t + 2");
}

int main()
{
    TestZero();
    TestConstant();
    TestMonic();
    TestOther();
}
