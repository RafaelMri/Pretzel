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

std::vector<std::size_t> missing_strands(pretzel const & pr)
{
    std::size_t num_strands = number_of_strands(pr);

    std::vector<std::size_t> result;
    std::vector<bool> have_strand(num_strands);
    for (auto const & tw : pr) { have_strand[tw.first - 1] = true; }
    for (std::size_t i = 0; i != num_strands - 1; ++i)
    {
        if (!have_strand[i]) { result.push_back(i + 1); }
    }
    return result;
}

void partition_twists(std::vector<std::size_t> const & missing, pretzel * pr)
{
    auto it = pr->begin();
    for (std::size_t n : missing)
    {
        it = std::stable_partition(it, pr->end(),
                                   [n](twist const & tw) { return tw.first < n; });
    }
}

std::vector<std::pair<pretzel::const_iterator, pretzel::const_iterator>>
group_pretzel_components(std::vector<std::size_t> const & missing, pretzel const & pr)
{
    std::vector<std::pair<pretzel::const_iterator, pretzel::const_iterator>> result;

    auto mt = missing.begin(), me = missing.end();
    auto pt1 = pr.begin(), pt2 = pt1, pe = pr.end();

    do
    {
        if (mt == me) { pt2 = pe; }
        while (pt2 != pe && pt2->first < *mt) { ++pt2; }

        result.emplace_back(pt1, pt2);

        pt1 = pt2;
        ++mt;
    } while (pt2 != pe);

    return result;
}

pretzel make_subpretzel(pretzel::const_iterator it, pretzel::const_iterator last)
{
    pretzel pr;
    if (it != last)
    {
        unsigned int offset = it->first - 1;
        pr.reserve(std::distance(it, last));
        for (; it != last; ++it) { pr.emplace_back(it->first - offset, it->second); }
    }
    return pr;
}

std::vector<std::size_t> strand_permutations(pretzel const & pr)
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

std::size_t count_permutation_cycles(std::vector<std::size_t> const & permutation)
{
    std::vector<bool> visited(permutation.size(), false);
    std::size_t count = 0;

    for (std::size_t i = 0; i != permutation.size(); ++i)
    {
        if (visited[i]) { continue; }

        for (std::size_t k = permutation[i] - 1; ;k = permutation[k] - 1)
        {
            visited[k] = true;
            if (k == i) { break; }
        }

        ++count;
  }

  return count;
}

std::vector<std::size_t> compute_homology(pretzel const & pr)
// The algorithm takes each crossing in turn and looks through the braid to find
// the next crossing with the same modulus. This is because the modulus of the
// crossing tells us between which strands it lies.
{
    std::vector<std::size_t> homology;

    if (pr.empty()) { return homology; }

    homology.resize(pr.size() - 1, 0);

    for (std::size_t i = 0; i != homology.size(); ++i)
    {
        for (std::size_t j = i; homology[i] == 0 && j < homology.size(); )
        {
            ++j;
            if (pr[i].first == pr[j].first) { homology[i] = j + 1; }
        }
    }

    return homology;
}

square_matrix<int> compute_seifert_matrix(pretzel const & pr)
// The algorithm follows the paper by Julia Collins ("An algorithm for computing
// the Seifert matrix of a link from a braid representation", section 3).
{
    std::vector<size_t> homology = compute_homology(pr);

    square_matrix<int> sm(homology.size(), 0);

    for (std::size_t i = 0; i != homology.size(); ++i)
    {
        if (!homology[i]) { continue; }

        for (std::size_t j = i; j != homology.size(); ++j)
        {
            /* Self-linking */
            if (i == j) { sm(i, j) = -(pr[i].second + pr[homology[i] - 1].second) / 2; }

            /* See Section 3.3 case 1 */
            else if (homology[i] > homology[j])              { /* nothing */ }

            /* See Section 3.3 case 2 */
            else if (homology[i] < j + 1)                    { /* nothing */ }

            /* See Section 3.3 case 3 */
            else if (homology[i] == j + 1)
            {
                sm(i, j) = (pr[j].second - 1) / 2;
                sm(j, i) = (pr[j].second + 1) / 2;
            }

            /* See Section 3.3 case 4 */
            else if (abs_diff(pr[i].first, pr[j].first) > 1) { /* nothing */ }

            /* See Section 3.3 case 5 */
            else if (pr[i].first == 1 + pr[j].first) { sm(j, i) = -1; }
            else if (pr[i].first + 1 == pr[j].first) { sm(i, j) =  1; }

            else
            {
                throw "Error in Seifert matrix algorithm";
            }
        }
    }

    // Remove zero rows and columns.
    for (std::size_t i = 0, N = sm.dim(); i != N; ++i)
    {
        std::size_t const ri = N - i - 1;
        if (homology[ri] == 0) { sm.remove_row(ri); sm.remove_col(ri); }
    }

    return sm;
}
