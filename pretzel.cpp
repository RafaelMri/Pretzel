#include <cerrno>
#include <cstdio>
#include <regex>
#include <sstream>
#include <string>
#include "pretzel.hpp"

namespace
{
    std::regex braid_notation(R"(\s*(([\+\-]?\d+)\s*)*)");
    std::regex pretzel_notation(R"(\s*(([A-Za-z])\s*([\+\-]?\d+)\s*)+)");

    std::regex braid_item(R"(\s*([\+\-]?\d+))");
    std::regex pretzel_item(R"(\s*([A-Za-z]?)\s*([\+\-]?\d+))");

    bool parse_int(std::string const & s, long int * out)
    {
        auto old_errno = errno;
        char * e;
        errno = 0;
        long int result = std::strtol(s.c_str(), &e, 10);
        if (*e != '\0' && errno != 0) { errno = old_errno; return false; }
        *out = result;
        errno = old_errno;
        return true;
    }

    bool parse_letter(std::string const & s, long int * out)
    {
        // Turn [A, Z] into [1, 26] and [a, z] into [-1, -26].

        if (s.size() != 1) { return false; }
        char c = s[0];

        // Assuming contiguous letters in the system's encoding.
        if ('A' <= c && c <=  'Z')  { *out = c - 'A' + 1;  return true; }
        if ('a' <= c && c <=  'z')  { *out = 'a' - c - 1;  return true; }
        return false;
    }
}

bool parse_string_as_pretzel(std::string const & in, pretzel * out)
{
    pretzel result;

    if (std::regex_match(in, braid_notation))
    {
        for (std::sregex_iterator it(in.begin(), in.end(), braid_item), e; it != e; ++it)
        {
            long int n;
            if (!parse_int(it->str(1), &n)) { return false; }

            if      (n < 0) { result.emplace_back(-n, -1); }
            else if (n > 0) { result.emplace_back(+n, +1); }
            else            { return false;                }
        }
    }
    else if (std::regex_match(in, pretzel_notation))
    {
        for (std::sregex_iterator it(in.begin(), in.end(), pretzel_item), e; it != e; ++it)
        {
            long int n, k;
            if (!parse_letter(it->str(1), &n)) { return false; }
            if (!parse_int(it->str(2), &k)) { return false; }

            // Twists must be odd.
            if (k % 2 == 0) { return false; }

            if      (n < 0) { result.emplace_back(-n, -k); }
            else if (n > 0) { result.emplace_back(+n, +k); }
            else            { return false;                }
        }
    }
    else
    {
        return false;
    }

    result.swap(*out);
    return true;
}
