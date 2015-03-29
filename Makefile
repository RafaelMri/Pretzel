BINS := main float_eq_test matrix_test pretzel_test algorithms_test polynomial_format_test
SRCS := main.cpp float_eq_test.cpp float_eq.cpp matrix_test.cpp pretzel_test.cpp pretzel.cpp algorithms_test.cpp algorithms.cpp polynomial_format_test.cpp
OBJS :=  $(SRCS:%.cpp=%.o)

CXXFLAGS := $(CFLAGS) $(CXXFLAGS) -W -Wall -Wextra -pedantic -std=c++11 -O3
LDFLAGS := $(LDFLAGS) -O3 -s

.phony: all clean
.default: all

all: $(BINS)

clean:
	$(RM) $(BINS) $(OBJS)

%: %.o
	$(CXX) $(LDFLAGS) -o $@ $+


algorithms_test.o: algorithms.hpp pretzel.hpp testing.hpp
algorithms_test: algorithms.o
algorithms.o: algorithms.hpp matrix.hpp pretzel.hpp

float_eq_test.o: testing.hpp
float_eq_test: float_eq.o
float_eq.o: float_eq.hpp

matrix_test.o: matrix.hpp testing.hpp
matrix_test: float_eq.o

pretzel_test.o: pretzel.hpp testing.hpp
pretzel_test: pretzel.o
pretzel.o: pretzel.hpp

polynomial_format_test.o: polynomial_format.hpp testing.hpp

main: pretzel.o algorithms.o
