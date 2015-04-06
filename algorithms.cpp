#include <algorithm>
#include <map>

#include "algorithms.hpp"
#include "contract.hpp"

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
        unsigned int offset = std::min_element(it, last)->first - 1;
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
            // Self-linking
            if (i == j) { sm(i, j) = -(pr[i].second + pr[homology[i] - 1].second) / 2; }

            // See Section 3.3 case 1
            else if (homology[i] > homology[j])              { /* nothing */ }

            // See Section 3.3 case 2
            else if (homology[i] < j + 1)                    { /* nothing */ }

            // See Section 3.3 case 3
            else if (homology[i] == j + 1)
            {
                sm(i, j) = (pr[j].second - 1) / 2;
                sm(j, i) = (pr[j].second + 1) / 2;
            }

            // See Section 3.3 case 4
            else if (abs_diff(pr[i].first, pr[j].first) > 1) { /* nothing */ }

            // See Section 3.3 case 5
            else if (pr[i].first == 1 + pr[j].first) { sm(j, i) = -1; }
            else if (pr[i].first + 1 == pr[j].first) { sm(i, j) =  1; }

            else
            {
                CHECK(false, "Error in Seifert matrix algorithm");
            }
        }
    }

    // Remove zero rows and columns.
    for (std::size_t i = 0, N = sm.dim(); i != N; ++i)
    {
        std::size_t const ri = N - i - 1;
        if (homology[ri] == 0)
        {
            if (sm.rows() != 0) { sm.remove_row(ri); }
            if (sm.cols() != 0) { sm.remove_col(ri); }
        }
    }

    return sm;
}


// Pretzel simplification steps

namespace
{
    // "CA" => "AC""
    bool commute_distant_elements(pretzel * p)
    {
        auto it = std::adjacent_find(
            p->begin(), p->end(),
            [](twist const & a, twist const & b)
            { return a.first > b.first + 1; });
        if (it != p->end()) { std::iter_swap(it, std::next(it)); return true; }
        return false;
    }

    pretzel::iterator find_yb_triple(unsigned int st, int tw, int step, pretzel::iterator it, pretzel::iterator last);

    // Remove twists from the outside of the pretzel that do not affect the link
    // defined by the pretzel closure. Such twists are characterized by being
    // the unique twist to contain the lowest or highest strand number. If a
    // twist with the lowest strand number is removed, all other strand numbers
    // are decremented by one (otherwise the pretzel would gain a disconnected
    // unknot).
    bool trim_lone_twists(pretzel * p)
    {
        std::map<unsigned int, unsigned int> twistogram;
        for (twist const & tw : *p) { ++twistogram[tw.first]; }
        if (twistogram.empty()) { return false; }

        auto trim_finder = [](pretzel * q, unsigned int st) {
            return std::find_if(q->begin(), q->end(), [st](twist const & tw) { return tw.first == st; });
        };

        // Lowest twist is unique.
        if (twistogram.begin()->second == 1)
        {
            p->erase(trim_finder(p, twistogram.begin()->first));
            for (auto & tw : *p) { --tw.first; }
            return true;
        }

        // Highest twist is unique.
        if (twistogram.rbegin()->second == 1)
        {
            p->erase(trim_finder(p, twistogram.rbegin()->first));
            return true;
        }

        // Extreme strand occurs twice and is a braid twist; check whether there
        // is a YB relation that makes the strand unique (e.g. "ZYZ" => "YZY").
        if (twistogram.begin()->second == 2)
        {
            auto it = trim_finder(p, twistogram.begin()->first);
            if (it->second == 1 || it->second == -1)
            {
                auto kt = find_yb_triple(it->first + 1, it->second, -1, it, p->end());
                if (kt != p->end()) { return true; }
            }
        }
        if (twistogram.rbegin()->second == 2 && twistogram.rbegin()->first > 1)
        {
            auto it = trim_finder(p, twistogram.rbegin()->first);
            if (it->second == 1 || it->second == -1)
            {
                auto kt = find_yb_triple(it->first - 1, it->second, +1, it, p->end());
                if (kt != p->end()) { return true; }
            }
        }

        return false;
    }

    // Find a twist (st, tw) in the range [it, last) that can be commuted to the
    // beginning of the range; returns last if no such twist exists.
    pretzel::iterator find_distant(unsigned int st, int tw, pretzel::iterator it, pretzel::iterator last)
    {
        for (; it != last; ++it)
        {
            if (it->first == st && it->second == tw) { return it; }
            if (abs_diff(it->first, st) < 2) { return last; }
        }

        return last;
    }

    // Determine whether a twist (st, tw) can be obtained in a position that can
    // be commuted to the beginning of the range by applying YB relations. If no
    // such rearrangement can be performed, returns last; otherwise performs the
    // rearrangement and returns the iterator pointint to the newly produced ele-
    // ment (st, tw).
    pretzel::iterator produce_via_yb(unsigned int st, int tw, pretzel::iterator it, pretzel::iterator last)
    {
        // Base case
        {
            auto kt = find_distant(st, tw, it, last);
            if (kt != last) { return kt; }
        }

        // Try YB above ("C" searches for "DCD")
        {
            auto kt = find_yb_triple(st, tw, 1, it, last);
            if (kt != last) { return kt; }
        }

        // Try YB below ("C" searches for "BCB")
        if (st > 1)
        {
            auto kt = find_yb_triple(st, tw, -1, it, last);
            if (kt != last) { return kt; }
        }

        return last;
    }

    pretzel::iterator find_yb_triple(unsigned int st, int tw, int step, pretzel::iterator it, pretzel::iterator last)
    {
        auto kt1 = find_distant(st + step, tw, it, last);
        if (kt1 != last)
        {
            auto kt2 = find_distant(st, tw, std::next(kt1), last);
            if (kt2 != last)
            {
                auto kt3 = produce_via_yb(st + step, tw, std::next(kt2), last);
                if (kt3 != last)
                {
                    std::iter_swap(kt2, kt3);
                    *kt1 = *kt3;
                    return kt1;
                }
            }
        }
        return last;
    }

    bool cancel_inverses(pretzel * p)
    {
        for (auto it = p->begin(), e = p->end(); it != e; ++it)
        {
            if (it->second != 1 && it->second != -1) { continue; }

            // Find inverse that's adjacent after RM2 moves ("a...A")
            {
                auto kt = find_distant(it->first, -it->second, std::next(it), e);
                if (kt != e) { p->erase(kt); p->erase(it); return true; }
            }

            // Find inverse that's adjacent after RM3 moves
            {
                auto kt = produce_via_yb(it->first, -it->second, std::next(it), e);
                if (kt != e) { p->erase(kt); p->erase(it); return true; }

            }
        }

        return false;
    }
}

bool simplify(pretzel * p)
{
    bool progress = false;

    while (cancel_inverses(p)) { progress = true; }

    while (commute_distant_elements(p)) { progress = true; }

    while (trim_lone_twists(p)) { progress = true; }

    return progress;
}
