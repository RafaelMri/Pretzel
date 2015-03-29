// Pretzel types and I/O.

#ifndef H_PRETZEL
#define H_PRETZEL

#include <iosfwd>
#include <utility>
#include <vector>

// A pretzel is a representation of a link, given as a sequence of twists of N
// strands. We represent the "pretzel" type as a sequence of twists, where a
// "twist" is represented as a pair of a strand number and a twisting count. For
// a positive number k, a twist (n, +k) means that strand n twists k times
// around strand (n + 1) starting with an over-crossing, and similarly (n, -k)
// means the twist starts with an under-crossing:
/*                                                      1 ___   ___
//                                                           \ /
//                                                            \
// 1 ___   ___         1 ___   ___                           / \
//      \ /                 \ /                              \ /
//       \                   /                                \
// 2 ___/ \___         2 ___/ \___                           / \
//                                                           \ /
//                                                            \
//                                                      2 ___/ \___
//
//    (1, +1)             (1, -1)                          (1, +3)
*/
// A consistent pretzel contains only twists of the form (positive int, odd
// int), since strands are labelled 1, 2, ..., and twists have to exit on the
// opposite side from where they entered.
//
// The total number of strands in a pretzel configuration is one plus the
// maximal strand number that occurs in any of its twists.
// TODO(chris): Say something about strand numbers that don't appear in the
//     pretzel, e.g. "1 3 1 1 3" (no "2").
//
// The pretzel representation is a generalisation of the so-called braid
// representation. A braid is a pretzel where all twists have twisting number
// +/-1, i.e. are simple over- or under-crossings. White it is true that every
// link has a braid representation and thus every pretzel has a corresponding
// braid representation that determines the same link, a pretzel representation
// can be more compact than an equivalent braid representation.
//
// See http://www.maths.ed.ac.uk/~jcollins/SeifertMatrix/ for details.

using twist = std::pair<unsigned int, int>;
using pretzel = std::vector<twist>;

// Formatted input. If parsing succeeds, returns true and overwrites *out with
// the parsed pretzel data. If parsing fails, returns false and *out is not
// modified. Requires that out be dereferenceable.
//
// The following string representations are recognised:
//
// * Braid notation: This is just a sequence of whitespace-separated, signed
//   strand numbers, for example "2 1 5 -1 1 -2". Each number is understood as a
//   twist by +/-1 of that strand, so the example represents the pretzel [(2, 1),
//   (1, 1), (5, 1), (1, -1), (1, 1), (2, -1)].
//
// * Pretzel notation: Each strand is labelled with a letter A-Z, and a twist is
//   given by a strand letter followed by a twisting count, e.g. "A3B1c3".
//   Whitespace is optional. If the strand letter is lower-case, the twisting
//   number is taken with its negated meaning, e.g. "A3" and "a-3" are the same
//   twist "(1, 3)". (Conventionally one would only use positive numbers and use
//   upper- and lower-case letters to indicate twist direction, but we accept
//   any combination of case and sign.)
//
// * Simple pretzel notation, a.k.a. alphabetic braid notation: Like pretzel
//   notation, but without twisting number. The twisting number is implied to be
//   one, so "A" is "(1, 1)" and "a" is "(1, -1)". Simple pretzel notation and
//   full pretzel notation cannot be mixed, so if any of the twists are not 1,
//   you must specify all twists explicitly. TODO(tkoeppe): Unify the two.
//
// Pretzel notation is naturally limited to 27 strands (i.e. twists starting at
// strand 1 (= A) up to 26 (= Z).
//
// TODO(chris): investigate if there's need to support more strands, and propose
//     a new notation.

bool parse_string_as_pretzel(std::string const & in, pretzel * out);

// Formatted output.

template <typename CharT, typename Traits, typename T, typename U>
std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> & os,
                                               std::pair<T, U> const & p)
{
    return os << '(' << p.first << ", " << p.second << ')';
}

template <typename CharT, typename Traits, typename Iter>
void print_range(std::basic_ostream<CharT, Traits> & os, Iter it, Iter last)
{
    os << '[';
    if (it != last) { os << *it; ++it; }
    for (; it != last; ++it) { os << ", " << *it; }
    os << ']';
}

template <typename CharT, typename Traits, typename T, typename A>
std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> & os,
                                               std::vector<T, A> const & v)
{
    print_range(os, v.begin(), v.end());
    return os;
}

#endif
