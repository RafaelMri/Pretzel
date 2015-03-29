#include <cassert>
#include <iostream>
#include <string>

#include "algorithms.hpp"
#include "pretzel.hpp"

// Compute and print analysis of a pretzel "pr".  Typically we preprocess a
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
    std::cout << " whose Seifert surface has genus " << genus << ".\n";
    std::cout << pre << "Seifert matrix: " << sm << "\n";
}

void analyse_pretzel(pretzel & pr)
{
    std::vector<std::size_t> missing = missing_strands(pr);
    partition_twists(missing, &pr);

    // Disjoint connected components of the pretzel.
    auto groups = group_pretzel_components(missing, pr);

    // We could also run "analyse_one(pr)" here to print the joint data for the
    // full pretzel, but this seems unnecessary. The interesting information
    // comes from individual connected components of of the Seifert surface.
    // (See comment above "analyse_one()".)

    char const * indent = "";

    if (!missing.empty())
    {
        std::cout << "The pretzel is a disjoint union of unrelated sub-pretzels, and we have arranged it accordingly: "
                  << pr << "\nDisjoint sub-pretzels:\n";
        indent = "   ";
    }

    for (auto const & g : groups)
    {
        std::cout << indent << "Pretzel component: ";
        print_range(std::cout, g.first, g.second);
        std::cout << '\n';
        analyse_one(make_subpretzel(g.first, g.second), indent);
        std::cout << '\n';
    }
}

int main()
{
    pretzel pr;

    for (std::string line;
         std::cerr << "Enter braid or pretzel (send EOF to quit): " && std::getline(std::cin, line); )
    {
        if (!parse_string_as_pretzel(line, &pr))
        {
            std::cerr << "Failed to parse input ('" << line << "') as pretzel; skipping.\n";
            continue;
        }

        analyse_pretzel(pr);
    }

    std::cerr << "Goodbye.\n";
}
