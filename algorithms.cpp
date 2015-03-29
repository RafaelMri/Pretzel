#include <algorithm>

#include "algorithms.hpp"

namespace
{
    unsigned int abs_diff(unsigned int a, unsigned int b)
    {
        return a < b ? b - a : a - b;
    }
}

std::size_t number_of_strands(pretzel const & pr)
{
    if (pr.empty()) { return 1; }

    return 1 + std::max_element(pr.begin(), pr.end())->first;
}

std::vector<std::size_t> compute_strand_permutations(pretzel const & pr)
// The algorithm follows each strand in turn through the braid to see where its
// final position is. For example, if we are following strand 2 and we see a
// crossing labelled '1', then we know that strand 2 will switch places with
// strand 1. If the crossing is labelled '2' then strand 2 will switch places
// with strand 3. Any other number will leave strand 2 as strand 2.
{
    std::size_t num_strands = number_of_strands(pr);

    std::vector<std::size_t> strand_permutation;
    strand_permutation.reserve(num_strands);

    for (std::size_t n = 1; n <= num_strands; ++n)
    {
        std::size_t m = n;
        for (auto const & tw : pr)
        {
            if      (tw.first     == m) { ++m; }
            else if (tw.first + 1 == m) { --m; }
        }
        strand_permutation.push_back(m);
    }

    return strand_permutation;
}
