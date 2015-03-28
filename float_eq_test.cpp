#include <limits>

#include "testing.hpp"

static_assert(std::numeric_limits<double>::has_signaling_NaN, "No signaling NaN.");
static_assert(std::numeric_limits<double>::has_quiet_NaN, "No quiet NaN.");
static_assert(std::numeric_limits<double>::has_infinity, "No infinity.");

constexpr double fl_snan = std::numeric_limits<double>::signaling_NaN();
constexpr double fl_qnan = std::numeric_limits<double>::quiet_NaN();
constexpr double fl_inf = std::numeric_limits<double>::infinity();

void TestNaN()
{
    EXPECT_FALSE(float_eq(fl_snan, fl_snan));
    EXPECT_FALSE(float_eq(fl_qnan, fl_qnan));
    EXPECT_FALSE(float_eq(fl_snan, fl_qnan));
    EXPECT_FALSE(float_eq(fl_qnan, 12.5));
}

void TestInf()
{
    EXPECT_TRUE(float_eq(+fl_inf, +fl_inf));
    EXPECT_TRUE(float_eq(-fl_inf, -fl_inf));
    EXPECT_FALSE(float_eq(+fl_inf, -fl_inf));
    EXPECT_FALSE(float_eq(fl_inf, 12.5));
    EXPECT_FALSE(float_eq(fl_inf, fl_qnan));
}

void TestNum()
{
    // Any non-NaN value is equal to itself.
    EXPECT_TRUE(float_eq(0.2, 0.2));

    // ...but the result of a computation may not be what you expect.
    std::uint64_t n;
    EXPECT_TRUE(float_eq(0.1 + 0.1 + 0.1, 0.3, 2, &n));
    std::printf("INFO: 0.1 + 0.1 + 0.1 and 0.3 differ by %" PRIu64 " ULP.\n", n);
}

int main()
{
    TestNaN();
    TestInf();
    TestNum();
}
