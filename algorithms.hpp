#ifndef H_ALGORITHMS
#define H_ALGORITHMS

#include <vector>

#include "matrix.hpp"
#include "pretzel.hpp"

// Returns the largest occurring strand number plus one; this is the number of
// strands in the pretzel. (E.g. the simple pretzel [(1, 1)] has two strands.)
std::size_t number_of_strands(pretzel const & pr);

// Return a list of all the inner strands that are not mentioned by the pretzel.
// If this list is non-empty, then the link decomposes into a disjoint union of
// links, since strands on either side of a "missing" strand cannot cross.
// (However, even if there are no missing strands, a pretzel may still have
// multiple components.)
std::vector<std::size_t> missing_strands(pretzel const & pr);

// Rearrange the twists in a pretzel into contiguous groups that contain no
// missing strands (e.g. "1 3 1 3" => "1 1 3 3"). The missing strands must be
// computed ahead of time and provided as input.
void partition_twists(std::vector<std::size_t> const & missing, pretzel * pr);

// Returns a list of sub-ranges of disconnected sub-pretzels of "pr". Requires
// that "pr" be partitioned according to "missing" as if by invocation of
// "partition_twists(missing, &pr)". Note that a sub-range does not constitute
// a pretzel properly, since it retains implicit initial strands. For example
// "BBB" is not the same as "AAA", but rather it is "AAA" plus an unknot. See
// "make_subpretzel()" below.
std::vector<std::pair<pretzel::const_iterator, pretzel::const_iterator>>
group_pretzel_components(std::vector<std::size_t> const & missing, pretzel const & pr);

// Turns a range of twists into a pretzel by moving all the strand numbers up so
// that the lowest strand number is 1. If the input range was obtained from a
// partition according to missing strands (as if by the above three algorithms),
// then the resulting pretzel is isomorphic to the connected component cut out
// by the range from the original pretzel.
//
// For example, "BBBDDD" has ranges [], [BBB], [], [DDD], but sub-pretzels [],
// [AAA], [], [AAA].
pretzel make_subpretzel(pretzel::const_iterator first, pretzel::const_iterator last);

// Given a braid or pretzel, computes its strand permutations. Let v denote the
// result. Then v.size() == number_of_strands(pr), and incoming strand i exits
// as strand v[i -1] (the "- 1" is because our strands are 1-based).
std::vector<std::size_t> strand_permutations(pretzel const & pr);

// Count the cycles in the given permutation. Permutations are 1-based.
std::size_t count_permutation_cycles(std::vector<std::size_t> const & permutation);

// Given a braid or pretzel, this function finds the homology generators:
// Let h = compute_homology(pr). Then the crossings pr[i] and pr[h[i] - 1]
// are adjacent, and h[i] = 0 means there is no adjacency.
std::vector<std::size_t> compute_homology(pretzel const & pr);

// Given a braid or pretzel, compute the link's Seifert matrix. The matrix is
// pruned, i.e. zero rows/columns have already been removed. The Seifert matrix
// is derived from the homology generators of "pr" that are obtained by calling
// compute_homology() above.
square_matrix<int> compute_seifert_matrix(pretzel const & pr);

#endif
