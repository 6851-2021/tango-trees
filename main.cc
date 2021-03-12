#include "src/item.h"
#include <iostream>

int main(int argc, char** argv) {
  Item item1(5);
  Item::dump_stats();
  Item item2(6);
  Item item3(7);
  item2 < item1;
  item3 < item1;
  Item::dump_stats();
  Item::reset_stats();
  Item::dump_stats();
}
