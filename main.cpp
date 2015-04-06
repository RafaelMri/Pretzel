#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <numeric>
#include <string>

#include "algorithms.hpp"
#include "matrix_format.hpp"
#include "polynomial_format.hpp"
#include "pretzel.hpp"

// Compute an Alexander polynomial from a Seifert matrix. Returns the list of
// coefficients, starting at degree zero.
std::vector<long int> alexander_poly(square_matrix<int> const & sm)
// We compute the coefficients of the Alexander polynomial by evaluating it on
// d + 1 points, where d == sm.dim() is its degree. Solving for the coefficients
// can be achieved by augmenting a Vandermonde matrix of d + 1 points with a
// column of the values at those points and performing Gauss-Jordan elimination
// on the augmented matrix.
//
// Note that even though all the input values are integers, we have to perform
// the matrix elimination with floating point numbers and round the result back
// to the nearest integer.
{
    // Step 1: Set up the Vandermonde matrix (at points 0, 1, ..., d).
    std::vector<double> points(sm.dim() + 1);
    std::iota(points.begin(), points.end(), 0);
    matrix<double> augmented_vandermonde = vandermonde<double>(sm.dim() + 2, points);

    // Step 2: Fill in the result p(t) = det(M - t M*) at those points.
    std::size_t last_col = augmented_vandermonde.cols() - 1;
    square_matrix<double> am(sm); // cast to double
    for (std::size_t i = 0; i != augmented_vandermonde.rows(); ++i)
    {
        augmented_vandermonde(i, last_col) = (am + am.transpose() * (-points[i])).determinant();
    }

    // Step 3: Solve the linear system by Gauss-Jordan elimination.
    matrix<double> solution = augmented_vandermonde.gauss_jordan();

    // Step 4: Obtain the resulting polynomial coefficients by rounding.
    std::vector<long int> coeffs;
    coeffs.reserve(sm.dim() + 1);
    for (std::size_t i = 0; i != sm.dim() + 1; ++i)
    {
        std::size_t const ri = sm.dim() - i;
        coeffs.push_back(std::lround(solution(ri, last_col)));
    }

    // Step 5: Profit.
    return coeffs;
}

// Compute and print analysis of a pretzel "pr". Typically we preprocess a
// given pretzel and analyse it component by component, but it is equally
// possible to analyse a complete, multi-component pretzel. The genus is
// additive and the Seifert matrix is block-additive under disjoint unions.
void analyse_one(pretzel const & pr, char const * pre = "")
{
    // Seifert matrix.
    square_matrix<int> sm = compute_seifert_matrix(pr);

    // Number of connected components of the link.
    std::size_t components = count_permutation_cycles(strand_permutations(pr));

    // Number of connected components of the Seifert surface.
    std::size_t k = missing_strands(pr).size() + 1;

    // Genus of the Seifert surface.
    //
    // There are several equivalent expressions for the genus of the Seifert
    // surface, see Corollary 2.7 and Equation (3) in the paper:
    //
    //    g = k - (s - c      + n) / 2  // s = number of Seifert circles, c = number of crossings
    //      = k - (m - l      + n) / 2  // m = number of strands = s, l = pr.size() = c
    //      = k - (k - dim(M) + n) / 2  // using dim(M) = rk H_1 = k - (m - l)
    //      = (k + dim(m) - n) / 2      // rearranged
    //
    // where n is the number of components of the link, k is the number of
    // components of the Seifert surface, c is the number of crossings, s is
    // the number of strands, and M is the Seifert matrix.
    //
    // We use the final expression to compute the genus.
    //
    // TODO(tkoeppe): Move this code into algorithms?

    assert((k + sm.dim() - components) % 2 == 0);
    std::size_t genus = (k + sm.dim() - components) / 2;

    std::cout << pre << "The pretzel is a ";
    if (components == 1) { std::cout << "knot"; }
    else                 { std::cout << "link with " << components << " components"; }
    std::cout << " whose Seifert surface has genus " << genus << ".\n"
              << pre << "Seifert matrix: " << print_inline(sm) << "\n";

    if (!pr.empty()) { print_pretzel(pr, std::cout, pre); std::cout << '\n'; }

    if (k > 1)
    {
        std::cout << pre << "Not computing Alexander polynomial because the link "
                            "is splittable (the Seifert surface is not connected).\n";
    }
    else
    {
        std::vector<long int> ap_coeffs = alexander_poly(sm);
        std::cout << pre << "Alexander polynomial: p(t) = "
                  << polynomial_to_string("t", ap_coeffs.begin(), ap_coeffs.end())
                  << "\n";
    }
}

void analyse_pretzel(pretzel pr, bool do_simplify)
{
    bool all_simplified = do_simplify && simplify(&pr);

    std::vector<std::size_t> missing = missing_strands(pr);
    partition_twists(missing, &pr);

    // Disjoint connected components of the pretzel.
    auto groups = group_pretzel_components(missing, pr);

    char const * indent = "";

    if (all_simplified)
    {
        std::cout << "The pretzel has been simplified.\n";
    }
    if (groups.size() > 1)
    {
        std::cout << "The pretzel is a disjoint union of unrelated sub-pretzels";
        if (do_simplify) { std::cout << ".\n"; }
        else             { std::cout << ", and we have arranged it accordingly.\n"; }
        indent = "   ";
    }
    if (!pr.empty() && groups.size() > 1)
    {
        std::cout << "Input: " << pr << '\n';
        print_pretzel(pr, std::cout);
        std::cout << '\n';
    }

    for (auto const & g : groups)
    {
        auto spr = make_subpretzel(g.first, g.second);

        bool sub_simplified = do_simplify && simplify(&spr);

        std::cout << indent << "Pretzel" << (groups.size() > 1 ? " component" : "") << ": ";

        print_range(std::cout, g.first, g.second);
        if (sub_simplified) { std::cout << " Simplified: " << spr; }
        std::cout << '\n';

        analyse_one(spr, indent);
        std::cout << '\n';
    }
}

int main(int argc, char * argv[])
{
    bool do_simplify = argc == 2 && std::strcmp(argv[1], "-s") == 0;

    pretzel pr;

    for (std::string line;
         std::cerr << "Enter braid or pretzel (send EOF to quit): " && std::getline(std::cin, line); )
    {
        if (!parse_string_as_pretzel(line, &pr))
        {
            std::cerr << "Failed to parse input ('" << line << "') as pretzel; skipping.\n";
            continue;
        }

        analyse_pretzel(std::move(pr), do_simplify);
    }

    std::cerr << "Goodbye.\n";
}
