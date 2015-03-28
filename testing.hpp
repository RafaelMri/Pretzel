// A set of simple testing macros:
//
//    EXPECT_EQ(fingers_per_hand(), toes_per_foot());
//    EXPECT_FALSE(v.empty());
//    EXPECT_FLOAT_EQ(0.1 + 0.1, 0.2);
//    etc.

#ifndef H_TESTING
#define H_TESTING

#include <cstdint>
#include <iostream>

#include "float_eq.hpp"

#define EXPECT_OP(op, A, B) do { \
        auto && a = (A);                                              \
    auto && b = (B);                                                  \
    if (!(a op b)) {                                                  \
        std::cout << "Expected " #A " " #op " " #B " but got: '"      \
                  << std::boolalpha << a << "' vs '" << b << "'.\n";  \
       std::cout << "Line: " __FILE__ ":" << __LINE__ << "\n";        \
    }                                                                 \
} while (false)

#define EXPECT_EQ(A, B) EXPECT_OP(==, A, B)
#define EXPECT_NE(A, B) EXPECT_OP(==, A, B)
#define EXPECT_TRUE(A) EXPECT_OP(==, A, true)
#define EXPECT_FALSE(A) EXPECT_OP(==, A, false)

// Floating point comparison tolerates one ULP difference.
#define EXPECT_FLOAT_EQ(A, B) EXPECT_TRUE(float_eq(A, B, 2))


#endif
