#include "algorithms.hpp"
#include "pretzel.hpp"
#include "testing.hpp"

void TestNumStrands()
{
    EXPECT_EQ(number_of_strands(pretzel()), 1u);

    pretzel pr = { {1, 1}, {3, -1} };
    EXPECT_EQ(number_of_strands(pr), 4u);
}

void TestMissingStrands()
{
    {
        pretzel pr = { {1, 1}, {2, -1}, {3, 3} };
        std::vector<std::size_t> expected = { };
        EXPECT_EQ(missing_strands(pr), expected);
    }

    {
        pretzel pr = { {6, 1}, {4, -1}, {3, 3} };
        std::vector<std::size_t> expected = { 1, 2, 5 };
        EXPECT_EQ(missing_strands(pr), expected);
    }
}

void TestPartitionTwists()
{
    pretzel pr = { {3, 1}, {1, 1}, {3, -1}, {4, 3}, {1, -3} };
    pretzel ex = { {1 ,1}, {1, -3}, {3, 1}, {3, -1}, {4, 3} };
    auto m = missing_strands(pr);
    partition_twists(m, &pr);
    EXPECT_EQ(pr, ex);
}

void TestStrandPermutations()
{
    pretzel pr = { {1, 1}, {1, 1}, {1, 1} };
    std::vector<std::size_t> expected = { 2, 1 };
    EXPECT_EQ(strand_permutations(pr), expected);
}

int main()
{
    TestNumStrands();
    TestMissingStrands();
    TestPartitionTwists();
    TestStrandPermutations();
}
