#ifndef _ITEM_H
#define _ITEM_H

class Item {
public:
	Item(int value) : value(value) {}

	int get_value() const;
	bool operator<(const Item& other);

  static void dump_stats();
  static void reset_stats();

private:
	static int n_comparisons;
	int value;
};

#endif  // _ITEM_H
