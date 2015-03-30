#include <sstream>
#include <string>

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
        for (long int s; iss >> std::ws >> s; )
        {
            if (!add_braid_twist(s, out)) { return false; }
        }
        return true;
    }

    // Parse alphabetic input ("AbAb" or "A1A3a5", braid or pretzel notation).
    bool alphabetic(std::istream & iss, pretzel * out)
    {
        for (char c; iss >> std::ws >> c; )
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
