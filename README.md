# Multi-Splay Trees and Tango Trees in C++

6.851 spring 2021 final project by Parker J. Rule [(@pjrule)](https://github.com/pjrule) and Christian Altamirano [(@bdiehs)](https://github.com/bdiehs).

We implement splay trees [(Sleator and Tarjan 1985)](https://dl.acm.org/doi/pdf/10.1145/3828.3835) and multi-splay trees [(Wang, Derryberry, and Sleator 2006)](https://www.cs.cmu.edu/~jonderry/multi-splay.pdf). We also provide a half-finished implementation of tango trees [(Demaine et al. 2005)](http://erikdemaine.org/papers/Tango_SICOMP/). Notes on what's left to be done to complete the tango tree implementation are in the `writeup` directory.

## Building and running
To build and run the testing/benchmarking code, simply run `make` and execute `./main`.

## Reusing
We implement our trees in headers. The `SplayTree` class in `src/splay.hpp` and the `MultiSplayTree` class in `src/multisplay.hpp` implement the common BST interface in `src/bst.hpp`. The half-finished implementation of tango trees in `src/rb.hpp` also implements this interface.
