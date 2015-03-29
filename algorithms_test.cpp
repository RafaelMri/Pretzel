#include "algorithms.hpp"
#include "pretzel.hpp"
#include "testing.hpp"

void TestNumStrands()
{
    EXPECT_EQ(number_of_strands(pretzel()), 1u);

    pretzel pr = { {1, 1}, {3, -1} };
    EXPECT_EQ(number_of_strands(pr), 4u);
}

void TestStrandPermutations()
{
    pretzel pr = { {1, 1}, {1, 1}, {1, 1} };
    std::vector<std::size_t> expected = { 2, 1 };
    EXPECT_EQ(compute_strand_permutations(pr), expected);
}

int main()
{
    TestNumStrands();
    TestStrandPermutations();
}
