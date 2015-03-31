This project contains a program to compute link invariants.

A link is a mathematical structure studied in knot theory. Abstractly, it is an
embedding of a disjoint union of S<sup>1</sup>s into R<sup>3</sup>.

The input to this program may be provided in either braid or pretzel notation;
the latter is a generalisation of the former.
See [`pretzel.hpp`](https://github.com/SeifertForm/Pretzel/blob/master/pretzel.hpp) for details.

The computed invariants are:

* Number of components.
* Alexander polynomial.
* Genus of the Seifert surface.
* Seifert matrix.

### Example

Compile the program (e.g. using `make`) and run `main`. You will be prompted for input. Enter
a braid or pretzel in one of the supported notations. For example using numeric notation:

    Enter braid or pretzel (send EOF to quit): 1 -2 1 -2

Output:

    Pretzel component: [(1, 1), (2, -1), (1, 1), (2, -1)]
    The pretzel is a knot whose Seifert surface has genus 1.
    Seifert matrix: [[-1, 1], [0, 1]]
    Alexander polynomial: p(t) = -t^2 + 3 * t - 1

The program echoes back the input pretzel, in `(strand, twist)` notation, and prints the
result of the analysis. If the pretzel has multiple disconnected components, results are
printed for each component. For example, this time using alphabetic notation:

    Enter braid or pretzel (send EOF to quit): Ad3bE5AbF7

Here we specified two disconnected pretzels, `AbAb` and `d3E5F7`. The program separates
the two and analyses each one in turn:

    The pretzel is a disjoint union of unrelated sub-pretzels, and we have arranged it accordingly: [(1, 1), (2, -1), (1, 1), (2, -1), (4, -3), (5, 5), (6, 7)]
    Disjoint sub-pretzels:
       Pretzel component: [(1, 1), (2, -1), (1, 1), (2, -1)]
       The pretzel is a knot whose Seifert surface has genus 1.
       Seifert matrix: [[-1, 1], [0, 1]]
       Alexander polynomial: p(t) = -t^2 + 3 * t - 1

       Pretzel component: [(4, -3), (5, 5), (6, 7)]
       The pretzel is a knot whose Seifert surface has genus 0.
       Seifert matrix: []
       Alexander polynomial: p(t) = 1

### Requirements

The program is written in standard C++11. It has no external requirements.
A GNU makefile is provided for convenience.

Tested on Linux with GCC 4.9.2 and Clang 3.7 (with libstdc++ and libc++).

### Compile, test and run cheat sheet

* To compile only the main program with GCC:

        g++ -W -Wall -Wextra -pedantic -std=c++11 -O3 -s -o main main.cpp pretzel.cpp algorithms.cpp

* To run all the tests:

        make
        for i in *_test; do ./$i; done

 Tests should not produce errors, though they may occasionally print informative notes.

* To use Clang and `libc++`:

        CXX=clang++ CXXFLAGS="-stdlib=libc++ -I /usr/local/include/c++/v1" LDFLAGS="-stdlib=libc++ -L /usr/local/lib" make

 You may need to add a suitable version of `libstdc++.so` to the linker flags.

### Known issues

No known bugs at present. Please report issues and feature requests via GitHub or contact the people listed below.

The current input parser limits the number of strands in a braid or pretzel given in alphabetic notation to 27,
since each strand is labelled with an alphabetic letter. There is no intrinsic algorithmic or computational limit
on the number of strands. Get in touch if you require pretzels with a larger strand number.


### Acknowledgements:

* Algorithm developed by Julia Collins, 2007
* Original implementation in C++ by Thomas Köppe 2007, 2015
* Project curation and maintenance by Chris Palmer, 2015

[Original website](http://www.maths.ed.ac.uk/~jcollins/SeifertMatrix/)
