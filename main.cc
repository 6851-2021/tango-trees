#include "src/item.h"
#include "src/splay.hpp"
#include "src/rbtree.hpp"
#include "src/bbtree.hpp"
#include <vector>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <time.h>
#include <assert.h> 


void splay_tree_test() {
  srand (time(NULL));
  SplayTree<int, int> T = SplayTree<int, int>();
  std::set<int> S; 
  for (int i=0; i < 100; i++) {
    int x = rand() % 50;
    S.insert(x);
    T.insert(x, x);
  }
  for (int i=0; i < 20; i++) {
    int x = rand() % 50;
    S.erase(x);
    T.remove(x);
  }
  for (int i=0; i < 50; i++) {
    auto t = T.find(i);
    if (S.find(i) != S.end()) {
      assert(t != nullptr);
    } else {
      assert(t == nullptr);
    }
  }  
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
