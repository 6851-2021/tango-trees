# Tango Trees in C++

In this project, we seek to create a generic, reasonably performant implementation of the tango tree data structure in C++. 

## Goals
We have three (tentative) goals:
1. Build an optimized implementation of the tango tree data structure.
2. Benchmark this implementation against other reference tree implementations. (Consider both the number of comparisons and the wall-clock time.)
3. Compile the tree to WASM using Emscripten and create a TypeScript wrapper, with the possibility of creating a visualization.

## TODO
- [ ] Define tree interface
- [ ] Define `Item` interface (think about global comparison counter)
- [ ] Implement balanced BST (splay?) under the hood (this should be generic enough to swap with an AVL/BB(⍺)/...)
- [ ] Create basic unit tests
_(more to come!)_