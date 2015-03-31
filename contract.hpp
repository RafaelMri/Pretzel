// Macros for contract enforcement.
//
// The CHECK* macros are essentially assertions, but will always take effect,
// no matter the compiler settings. This is unlike the "assert" macro in the
// <cassert> header, which has no effect when NDEBUG is defined.
//
// To decide when to use "assert" and when to use a CHECK macro, consider these
// scenarios:
//
//    * Array (vector) subscripting: clearly a case for "assert", since an
//      invalid subscript is always a logic error and a programming bug.
//
//    * A function taking a pointer as an out-parameter: here a CHECK for
//      non-nullness is appropriate. The non-nullness is a precondition, but
//      there is nothing in the way functions are called that would naturally
//      require a pointer argument to be non-null. By contrast, in the previous
//      example array subscripts are usually derived from array sizes and not
//      completely arbitrary.
//
//    * A final "else" block or "default" label in an algorithm that should not
//      be reached. Here a CHECK(false) is appropriate, since the code is cold
//      anyway and there is no harm in having an unconditional abort there. An
//      "assert" would also be appropriate, though.
//
// The macros are inspired by GoogleTest and google-glog.

#ifndef H_CONTRACT
#define H_CONTRACT

#include <cstdlib>
#include <iostream>

#define CHECK(c, msg) do {                                          \
    bool v = static_cast<bool>(c);                                  \
    if (!v) {                                                       \
        std::cerr << "Error in " __FILE__ ":" << __LINE__           \
                  << " when evaluating '" #c "'. " << msg << "\n";  \
        std::abort();                                               \
    }                                                               \
} while (false)

#define CHECK_OP(op, A, B, msg) do {                                \
    auto && a = (A);                                                \
    auto && b = (B);                                                \
    if (!(a op b)) {                                                \
        std::cerr << "Error in " __FILE__ ":" << __LINE__           \
                  << " when evaluating '" #A " " #op " " #B         \
                     "'. Got '" << a << "' vs. '" << b << "'. "     \
                  << (msg) << "\n";                                 \
        std::abort();                                               \
    }                                                               \
} while (false)

#define CHECK_EQ(A, B, msg) CHECK_OP(==, A, B, msg)
#define CHECK_NE(A, B, msg) CHECK_OP(==, A, B, msg)

#endif
