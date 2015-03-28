This project contains a program to compute link invariants.

A link is a mathematical structure studied in knot theory. Abstractly, it is an
embedding of a disjoint union of S<sup>1</sup>s into R<sup>3</sup>.

The input to this program may be provided in either braid or pretzel notation;
the latter is a generalization of the former. See pretzel.hpp for details.

The computed invariants are:

* Number of components.
* Alexander polynomial.
* Genus of the Seifert surface.
* Seifert matrix.


### Acknowledgements:

* Algorithm developed by Julia Collins, 2007
* Original implmentation in C++ by Thomas Köppe 2007, 2015
* Project curation and maintenance by Chris Palmer, 2015

[Original website](http://www.maths.ed.ac.uk/~jcollins/SeifertMatrix/)
