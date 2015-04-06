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

    ./main
    Enter braid or pretzel (send EOF to quit): 1 -2 1 -2

Output:

    Pretzel: [(1, 1), (2, -1), (1, 1), (2, -1)]
    The pretzel is a knot whose Seifert surface has genus 1.
    Seifert matrix: [[-1, 1], [0, 1]]
    _   _______   ______
     \ /       \ /
      \         \
    _/ \__   __/ \__   _
          \ /       \ /
           /         /
    ______/ \_______/ \_

    Alexander polynomial: p(t) = -t^2 + 3 * t - 1

The program echoes back the input pretzel, in `(strand, twist)` notation, and prints the
result of the analysis, as well as a simple visualisation of the pretzel. (Non-braid twists
are shown with their twisting number next to the crossing, `**` is printed for twisting
numbers greater than 99.) If the pretzel has multiple disconnected components, results are
printed for each component. For example, this time using alphabetic notation:

    Enter braid or pretzel (send EOF to quit): Ad3bD5AbD7

Here we specified two disconnected pretzels, `AbAb` and `d3D5D7`. The program separates
the two and analyses each one in turn:

    The pretzel is a disjoint union of unrelated sub-pretzels, and we have arranged it accordingly.
    Input: [(1, 1), (2, -1), (1, 1), (2, -1), (4, -3), (5, 5), (6, 7)]
    _   _______   _____________________
     \ /       \ /
      \         \
    _/ \__   __/ \__   ________________
          \ /       \ /
           /         /
    ______/ \_______/ \________________


    _____________________   __   __   _
                         \ /  \ /  \ /
                          / 3  \ 5  \ 7
    _____________________/ \__/ \__/ \_

       Pretzel component: [(1, 1), (2, -1), (1, 1), (2, -1)]
       The pretzel is a knot whose Seifert surface has genus 1.
       Seifert matrix: [[-1, 1], [0, 1]]
       _   _______   ______
        \ /       \ /
         \         \
       _/ \__   __/ \__   _
             \ /       \ /
              /         /
       ______/ \_______/ \_

       Alexander polynomial: p(t) = -t^2 + 3 * t - 1

       Pretzel component: [(4, -3), (4, 5), (4, 7)]
       The pretzel is a knot whose Seifert surface has genus 1.
       Seifert matrix: [[-1, 2], [3, -6]]
       _   __   __   _
        \ /  \ /  \ /
         / 3  \ 5  \ 7
       _/ \__/ \__/ \_

       Alexander polynomial: p(t) = t

### Simplification

The program contains a algorithm that simplifies an input pretzel and puts it into
a canonical ordering. To apply this algorithm, launch the program with `-s`:

    ./main -s

    Enter braid or pretzel (send EOF to quit): BaCdceBdBe
    The pretzel has been simplified.
    Pretzel: [(1, 1), (3, -1), (2, -1), (1, 1), (1, 1), (3, -1)]
    The pretzel is a link with 2 components whose Seifert surface has genus 1.
    Seifert matrix: [[-1, 0, 0], [0, 1, 0], [1, 0, -1]]
    _   ____________   __   ______
     \ /            \ /  \ /
      \              \    \
    _/ \_______   __/ \__/ \______
               \ /
                /
    ______   __/ \____________   _
          \ /                 \ /
           /                   /
    ______/ \_________________/ \_

    Alexander polynomial: p(t) = t^3 - 2 * t^2 + 2 * t - 1

The braid `BaCdceBdBe` has been simplified to `AcbAAc`.

Simplification applies all of the following operations until no further such
applications are possible:

* Cancellation of adjacent, inverse braid elements (e.g. remove `Aa`). This applies
  only to braid twists, i.e. twists with twisting count +/-1. This is simply the
  group law of the braid group, and also the Reidemeister-2 move.
* Commuting distant twists (e.g. `D5A3` => `A3D5`). Commuting may enable one of the
  other operations. If no further commuting is required to enable other simplifications,
  all remaining ambiguities due to this commutativity are resolved by requiring the lowest
  strand number to appear first.
* Applying the Yang-Baxter braid relations (or Reidemeister-3 moves, e.g. `BCB` => `CBC`).
  This too only applies to braid twists.
* Removing isolated twists from the outside of the pretzel (e.g. `AAAB` => `AAA`),
  since they do not change the link isomorphism type. If an isolated twist is removed
  from the front, all remaining strands are shifted down by one: `ABBB` => `BBB` => `AAA`.
  (Without this shift, removing the isolated twist would introduce a new link component.)

Stepping through the above example `BaCdceBdBe`:

1. Commute the last two `B`s forward: `BaCdcBBede`
2. Apply Yang-Baxter relation: `BaCdcBBded`
3. Commute the last two `B`s backward: `BaCdcdedBB`
4. Apply Yang-Baxter relation: `BaCcdcedBB`
5. Cancel `Cc`: `BadcedBB`
6. Remove isolated twists `a` and `e`: `BdcdBB`
7. Shift all strands down by one: `AcbcAA`
8. Commute distant twists so that the smallest strand appears first: `AcbAAc`

**Caveat:** Applying simplifications may discard split unknots from the final link.
For example, `AaBBB` becomes `BBB`, which retains its two components, but `AAABb` becomes
just `AAA`, which loses one split, unknot component. This is because the number of desired
strands is implicit in the braid/pretzel notation and cannot be specified explicitly. For
example, there is no braid notation for a completely split link of unknots with multiple
components. Simplifying `AaBb` results in the empty pretzel, not in three unknots. If in
doubt, compare the number of pretzel and link components with and without simplifications.

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

Simplification does not currently attempt to apply YB relations to produce isolated twists (e.g. `AbAcbc` is not simplified to `AbAbb`).

The "caveat" note in the "Simplification" section describes circumstances under which simplification
discards disconnected pretzel components.

### Acknowledgements:

* Algorithm developed by Julia Collins, 2007
* Original implementation in C++ by Thomas Köppe 2007, 2015
* Project curation and maintenance by Chris Palmer, 2015

[Original website](http://www.maths.ed.ac.uk/~jcollins/SeifertMatrix/)
