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
    EXPECT_FALSE(parse_string_as_pretzel("foo", &pr));

    // Empty.
    EXPECT_TRUE(parse_string_as_pretzel("", &pr));
    EXPECT_EQ(pr, pretzel());
    EXPECT_TRUE(parse_string_as_pretzel("    \n   ", &pr));
    EXPECT_EQ(pr, pretzel());
}

void TestNumeric()
{
    pretzel pr, expected{{1, 1}, {2, 1}, {1, -1}, {3, 1}, {2, -1}};
    EXPECT_TRUE(parse_string_as_pretzel("1 2 -1 3 -2", &pr));
    EXPECT_EQ(pr, expected);
}

void TestPretzel()
{
    pretzel pr, expected{{1, 1}, {2, -3}, {2, 1}, {1, 1}, {1, -1}};
    EXPECT_TRUE(parse_string_as_pretzel("A1b3B1a-1a1", &pr));
    EXPECT_EQ(pr, expected);
}

int main()
{
    TestPrinting();
    TestEdgeCases();
    TestNumeric();
    TestPretzel();
}
