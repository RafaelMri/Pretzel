BINS := float_eq_test matrix_test pretzel_test
SRCS := float_eq_test.cpp matrix_test.cpp pretzel_test.cpp pretzel.cpp
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

float_eq_test.o: testing.hpp
matrix_test.o: matrix.hpp testing.hpp
pretzel_test.o: testing.hpp
pretzel.o: pretzel.hpp
pretzel_test: pretzel.o
