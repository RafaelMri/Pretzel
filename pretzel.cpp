#include <sstream>
#include <string>

#include "algorithms.hpp"
#include "pretzel.hpp"

namespace
{
    // Note: Assuming contiguous encoding of letters :-(
    bool parse_letter(char c, long int * out = nullptr)
    {
        if ('A' <= c && c <=  'Z') { if (out) { *out = c - 'A' + 1; } return true; }
        if ('a' <= c && c <=  'z') { if (out) { *out = 'a' - c - 1; } return true; }
        return false;
    }

    // Add a twist in ordinary braid notation ("1 -2 1 -2" or "AbAb"); the
    // twisting number is always +/- 1.
    bool add_braid_twist(long int s, pretzel * out)
    {
        if      (s < 0) { out->emplace_back(-s, -1); return true; }
        else if (s > 0) { out->emplace_back(+s, +1); return true; }
        else            { return false;                           }
    }

    // Parse purely numeric input ("1 -2 1 -2", braid notation only).
    bool numeric(std::istream & iss, pretzel * out)
    {
        for (long int s; iss >> s; )
        {
            if (!add_braid_twist(s, out)) { return false; }
        }
        return true;
    }

    // Parse alphabetic input ("AbAb" or "A1A3a5", braid or pretzel notation).
    bool alphabetic(std::istream & iss, pretzel * out)
    {
        for (char c; iss >> c; )
        {
            long int tw, s;
            if (!parse_letter(c, &s)) { return false; }

            if (iss >> tw)
            {
                if (tw % 2 == 0) { return false;             }
                if (s < 0) { s *= -1; tw *= -1; }
                out->emplace_back(s, tw);
            }
            else
            {
                iss.clear();
                if (!add_braid_twist(s, out)) { return false; }
            }
        }
        return true;
    }

    template <bool (&Parser)(std::istream &, pretzel *)>
    bool parse(std::string const & in, pretzel * out)
    {
        pretzel pr;
        std::istringstream iss(in);

        if (!Parser(iss, &pr) || !iss.eof()) { return false; }

        out->swap(pr);
        return true;
    }
}

bool parse_string_as_pretzel(std::string const & in, pretzel * out)
{
    std::istringstream iss(in);
    char c;

    if (iss >> std::ws && iss.eof()) { pretzel().swap(*out); return true; }

    if (!(iss >> c >> std::ws)) { return false; }

    if (c == '+' || c == '-' || ('0' <= c && c <= '9')) { return parse<numeric>(in, out); }

    if (parse_letter(c)) { return parse<alphabetic>(in, out); }

    return false;
}

namespace
{
    void print_below(std::size_t i, std::size_t j, int twist, std::ostream & os)
    {
        bool overcross = twist > 0;
        if (!overcross) { twist = -twist; }

        char const * lines[] = { "_   _", " \\ / ", overcross ? "  \\  " : "  /  " };
        if (twist != 1 && i == 2)
        {
            if      (j == 3 && 10 <= twist && twist < 100) { os << static_cast<char>('0' + twist / 10); }
            else if (j == 4 && 10 <= twist && twist < 100) { os << static_cast<char>('0' + twist % 10); }
            else if (j == 4 && twist < 10)                 { os << static_cast<char>('0' + twist);      }
            else if ((j == 3 || j == 4) && 100 <= twist)   { os << '*';                                 }
            else                                           { os << lines[i][j];                         }
        }
        else
        {
            os << lines[i][j];
        }
    }

    void print_above(std::size_t i, std::size_t j, std::ostream & os)
    {
        if (i != 0) os << ' ';
        else        os << "_/ \\_"[j];
    }
}

void print_pretzel(pretzel const & pr, std::ostream & os, const char * prefix)
{
    std::size_t n = number_of_strands(pr);  // height
    std::size_t l = 5 * pr.size();          // width

    for (std::size_t i = 0; i != 3 * n - 2; ++i)
    {
        os << prefix;
        for (std::size_t j = 0; j != l; ++j)
        {
            std::size_t s = pr[j / 5].first - 1;
            int tw = pr[j / 5].second;

            if      (s     == i / 3) { print_below(i % 3, j % 5, tw, os); }
            else if (s + 1 == i / 3) { print_above(i % 3, j % 5, os);          }
            else if (0     == i % 3) { os << '_';                              }
            else                     { os << ' ';                              }
        }
        os << '\n';
    }

}
