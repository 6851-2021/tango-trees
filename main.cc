#include "src/item.h"
#include "src/splay.hpp"
#include <iostream>


void splay_tree_test() {
	SplayTree<int, int> T = SplayTree<int, int>();
	T.insert(1, 1);
	T.insert(2, 2);
	T.insert(3, 3);
	T.insert(4, 4);
	T.insert(5, 5);
	T.find(1)->println();
    T.root->println();
    T.find(3)->println();
    T.root->println();
    T.remove(2);
    T.root->println();
    T.remove(4);
    T.root->println();
}

int main(int argc, char** argv) {
//  Item item1(5);
//  Item::dump_stats();
//  Item item2(6);
//  Item item3(7);
//  item2 < item1;
//  item3 < item1;
//  Item::dump_stats();
//  Item::reset_stats();
//  Item::dump_stats();
  splay_tree_test();
}
