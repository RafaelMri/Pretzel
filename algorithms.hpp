#ifndef H_ALGORITHMS
#define H_ALGORITHMS

#include <vector>

#include "matrix.hpp"
#include "pretzel.hpp"

// Returns the largest occurring strand number plus one; this is the number of
// strands in the pretzel. (E.g. the simple pretzel [(1, 1)] has two strands.)
std::size_t number_of_strands(pretzel const & pr);

// Given a braid, computes its strand permutations. Let v denote the result.
// Then v.size() == number_of_strands(pr), and incoming strand i exits as strand
// v[i -1] (the "- 1" is because our strands are 1-based).
std::vector<std::size_t> compute_strand_permutations(pretzel const & pr);

#endif
