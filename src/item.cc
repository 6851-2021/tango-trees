#include "item.h"
#include <iostream>

int Item::get_value() const {
	return value;
}

bool Item::operator<(const Item& other) {
	n_comparisons++;
	return value < other.get_value();
}

void Item::dump_stats() {
	std::cout << n_comparisons << std::endl;
}

void Item::reset_stats() {
	n_comparisons = 0;
}

int Item::n_comparisons = 0;
