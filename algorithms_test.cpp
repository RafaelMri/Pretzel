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

    pretzel pr_orig = pr;
    partition_twists({}, &pr);
    EXPECT_EQ(pr, pr_orig);

    auto m = missing_strands(pr);
    partition_twists(m, &pr);
    EXPECT_EQ(pr, ex);
}

void TestGroupPretzelComponents()
{
    using V = std::vector<std::pair<pretzel::const_iterator, pretzel::const_iterator>>;

    {
        // One group
        pretzel pr = { {1, 1}, {3, 1}, {2, 1} };
        std::vector<std::size_t> missing = missing_strands(pr);
        partition_twists(missing, &pr);

        V groups = group_pretzel_components(missing, pr), expected({{pr.cbegin(), pr.cend()}});
        EXPECT_TRUE(groups == expected);
    }

    {
        // Two groups: [(1, 1), (2, 1)] and [(4, 1)]
        pretzel pr = { {1, 1}, {4, 1}, {2, 1} };
        std::vector<std::size_t> missing = missing_strands(pr);
        partition_twists(missing, &pr);

        auto it = pr.cbegin();
        V groups = group_pretzel_components(missing, pr), expected({{it, it + 2}, {it + 2, it + 3}});
        EXPECT_TRUE(groups == expected);
    }
}

void TestMakeSubPretzel()
{
    pretzel pr = { {5, 1}, {3, 1}, {4, 1} }, expected = { {3, 1}, {1, 1}, {2, 1} };
    EXPECT_EQ(make_subpretzel(pr.begin(), pr.end()), expected);
}

void TestStrandPermutations()
{
    pretzel pr = { {1, 1}, {1, 1}, {1, 1} };
    std::vector<std::size_t> expected = { 2, 1 };
    EXPECT_EQ(strand_permutations(pr), expected);
}

void TestCountPermutationCycles()
{
    {
        std::vector<std::size_t> perm = { 2, 4, 3, 1 }; // Cycles: [2, 4, 1], [3]
        EXPECT_EQ(count_permutation_cycles(perm), 2u);
    }

    {
        std::vector<std::size_t> perm = { 1 };          // Cycles: [1]
        EXPECT_EQ(count_permutation_cycles(perm), 1u);
    }
}

void TestSimplify()
{
    {
        //                 d        C       D       B       C       B
        pretzel pr =  { {4, -1}, {3, 1}, {4, 1}, {2, 1}, {3, 1}, {2, 1} };
        // => dDCDBC (YB twice) => CDBC (cancel inverses) => BB (trim)
        pretzel expected = { {2, 1}, {2, 1} };
        EXPECT_TRUE(simplify(&pr));
        EXPECT_EQ(pr, expected);
    }

    {
        pretzel pr = { { 2, 3}, {3, 1}, {4, -5}, {3, 1}, {3, 1} };
        pretzel expected = { {2, 1}, {2, 1}, {2, 1} };
        EXPECT_TRUE(simplify(&pr));
        EXPECT_EQ(pr, expected);
    }

    {
        pretzel pr = { { 1, 1}, {2, -1}, {1, 1}, {3, -1}, {2, -1}, {3, -1} };
        pretzel expected = { {1, 1}, {2, -1}, {1, 1}, {2, -1}, {2, -1} };
        EXPECT_TRUE(simplify(&pr));
        EXPECT_EQ(pr, expected);
    }
}

void TestNonSimplify()
{
    {
        pretzel pr =  { {1, 1}, {2, -1}, {1, 1}, {2, -1} }, expected = pr;
        EXPECT_FALSE(simplify(&pr));
        EXPECT_EQ(pr, expected);
    }

    {
        pretzel pr =  { {1, 3}, {1, -3} }, expected = pr;
        EXPECT_FALSE(simplify(&pr));
        EXPECT_EQ(pr, expected);
    }
}

int main()
{
    TestNumStrands();
    TestMissingStrands();
    TestPartitionTwists();
    TestGroupPretzelComponents();
    TestMakeSubPretzel();
    TestStrandPermutations();
    TestCountPermutationCycles();
    TestSimplify();
    TestNonSimplify();
}
