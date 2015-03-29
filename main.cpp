#include <cassert>
#include <iostream>
#include <string>

#include "algorithms.hpp"
#include "pretzel.hpp"

void analyze_one(pretzel const & pr, std::size_t k, char const * pre = "")
{
   // Seifert matrix.
    square_matrix<int> sm = compute_seifert_matrix(pr);

    // Number of connected components of the link.
    std::size_t components = count_permutation_cycles(strand_permutations(pr));

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

    if (components == 1) { std::cout << pre << "The pretzel is a knot.\n"; }
    else                 { std::cout << pre << "The pretzel is a link with " << components << " components.\n"; }

    std::cout << pre << "Seifert matrix: " << sm << "\n"
              << pre << "Genus: " << genus << "\n";
}

void analyze_pretzel(pretzel & pr)
{
    std::vector<std::size_t> missing = missing_strands(pr);
    partition_twists(missing, &pr);

    // Disjoint connected components of the pretzel.
    auto groups = group_pretzel_components(missing, pr);

    // We could also run "analyze_one(pr, groups.size())" here to print the
    // joint data for the full pretzel, but this seems unnecessary. The
    // interesting information comes from individual connected components of
    // of the Seifert surface.

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
        pretzel sub_pr;
        if (g.first != g.second)
        {
            std::size_t offset = g.first->first - 1;
            for (auto it = g.first; it != g.second; ++it)
                sub_pr.emplace_back(it->first - offset, it->second);
        }
        analyze_one(sub_pr, 1, indent);
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

        analyze_pretzel(pr);
    }

    std::cerr << "Goodbye.\n";
}
