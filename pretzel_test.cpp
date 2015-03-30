#include <sstream>
#include <string>

#include "pretzel.hpp"
#include "testing.hpp"

void TestPrinting()
{
    pretzel pr { {1, -1}, {2, 1}, {1, 1} };
    std::ostringstream oss;
    oss << pr;
    EXPECT_EQ(oss.str(), "[(1, -1), (2, 1), (1, 1)]");
}

void TestEdgeCases()
{
    pretzel pr;

    // Unparsable.
    EXPECT_FALSE(parse_string_as_pretzel("Not 123 A 456 Pretzel", &pr));

    // Empty.
    EXPECT_TRUE(parse_string_as_pretzel("", &pr));
    EXPECT_EQ(pr, pretzel());
    EXPECT_TRUE(parse_string_as_pretzel("    \n   ", &pr));
    EXPECT_EQ(pr, pretzel());
}

void TestNumeric()
{
    pretzel pr, expected{{1, 1}, {2, 1}, {1, -1}, {51, 1}, {2, -1}};
    EXPECT_TRUE(parse_string_as_pretzel("1 2 -1 51 -2", &pr));
    EXPECT_EQ(pr, expected);
}

void TestBraid()
{
    pretzel pr, expected{{1, 1}, {2, -1}, {1, 1}, {2, -1}};
    EXPECT_TRUE(parse_string_as_pretzel("AbAb", &pr));
    EXPECT_EQ(pr, expected);
}

void TestPretzel()
{
    pretzel pr, expected{{1, 1}, {2, -3}, {2, 15}, {4, 1}, {1, 1}, {4, -1}, {1, -1}};
    //                     A1       b3      B15       D      a-1      d       a1
    EXPECT_TRUE(parse_string_as_pretzel("A1b3B15Da-1da1", &pr));
    EXPECT_EQ(pr, expected);
}

int main()
{
    TestPrinting();
    TestEdgeCases();
    TestNumeric();
    TestBraid();
    TestPretzel();
}
