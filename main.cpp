#include <iostream>
#include <string>

#include "algorithms.hpp"
#include "pretzel.hpp"

void print_sub_pretzels(std::vector<std::size_t> const & missing, pretzel const & pr,
                        std::ostream & os, char const * pre)
{
    auto mt = missing.begin(), me = missing.end();
    auto pt1 = pr.begin(), pt2 = pt1, pe = pr.end();
    do
    {
        if (mt == me) { pt2 = pe; }
        while (pt2 != pe && pt2->first < *mt) { ++pt2; }

        os << pre;
        print_range(os, pt1, pt2);
        os << '\n';

        pt1 = pt2;
        ++mt;
    } while (pt2 != pe);
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

        std::cout << "Input pretzel: " << pr << "\n";
        std::vector<std::size_t> missing = missing_strands(pr);
        if (!missing.empty())
        {
            partition_twists(missing, &pr);

            std::cout << "The pretzel is a disjoint union of unrelated sub-pretzels:\n";
            print_sub_pretzels(missing, pr, std::cout, "   ");
            std::cout << "We have rearranged your pretzel accordingly: " << pr << "\n";
        }

        std::vector<size_t> homology = compute_homology(pr);
        auto sm = compute_seifert_matrix(pr, homology);

        std::cout << "Seifert matrix: " << sm << "\n";
    }

    std::cerr << "Goodbye.\n";
}