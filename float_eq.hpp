#ifndef H_FLOAT_EQ
#define H_FLOAT_EQ

// Compare two floating point numbers; if they are not "obviously related" (i.e.
// are NaN, infinity, zero, or have opposite signs), check if they differ by
// fewer than "ulp" units in last place. If "ulpdiff" is non-null, also stores
// the difference in ULPs into *ulpdiff in that case.
bool float_eq(double a, double b, std::uint64_t ulp = 1, std::uint64_t * ulpdiff = nullptr);

#endif
