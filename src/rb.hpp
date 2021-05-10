#include "bst.hpp"
#include <vector>
#include <iostream>
using std::vector;

#ifndef TANGO_TREES_RB_H
#define TANGO_TREES_RB_H

struct TInfo {
	// "In addition to storing the key value and depth, each node
	// stores the minimum and maximum depth over the nodes in its subtree."
	unsigned int black_height;
	unsigned int perfect_depth;
	unsigned int min_depth;
	unsigned int max_depth;
	bool red;
	bool deleted;
	bool marked;

	TInfo() {
		this->black_height = 0;
		this->perfect_depth = 0;
		this->min_depth = 0;
		this->max_depth = 0;
		this->red = true;
		this->marked = false;
		this->deleted = false;
	}
};

template<typename K, typename V>
using TangoNode = BSTNode<K, V, TInfo>;

template<typename K, typename V>
class TangoTree : public BST<K, V, TInfo> {
private:
    void split(K key);
    void rebuild();
    void left_rotate(TangoNode<K, V> *x);
    void right_rotate(TangoNode<K, V> *x);
    void fixup(TangoNode<K, V> *x);
    bool locked = false;
    unsigned long size = 0;
    unsigned long deleted = 0;
public:
    TangoTree() : BST<K, V, TInfo>() {}
    void insert(K key, V val);
    void remove(K key);
    void lock();
    void unlock();
    TangoNode<K, V> *find(K key);
};


template <typename K, typename V>
bool is_black(TangoNode<K, V> *node) {
	// Recommended by Wein 2001 in lieu of a sentinel leaf node.
	return node == nullptr || !(node->info.red);
}

template <typename K, typename V>
bool is_red(TangoNode<K, V> *node) {
	return node != nullptr && node->info.red;
}


template <typename K, typename V>
void TangoTree<K, V>::left_rotate(TangoNode<K, V> *x) {
	auto y = x->right;
	x->right = y->left;
	if (y->left != nullptr) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == nullptr) {
		this->root = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}


template <typename K, typename V>
void TangoTree<K, V>::right_rotate(TangoNode<K, V> *x) {
	auto y = x->left;
	x->left = y->right;
	if (y->right != nullptr) {
		y->right->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == nullptr) {
		this->root = y;
	} else if (x == x->parent->right) {
		x->parent->right = y;
	} else {
		x->parent->left = y;
	}
	y->right = x;
	x->parent = y;
}


template <typename K, typename V>
void TangoTree<K, V>::insert(K key, V val) {
	if (this->locked) {
		throw;  // TODO: better error here
	}
	TangoNode<K, V> *z = new TangoNode<K, V>(key, val, TInfo());
	TangoNode<K, V> *y = nullptr;
	TangoNode<K, V> *x = this->root;
	while (x != nullptr) {
		std::cout << "key of x is " << x->key << std::endl;
		y = x;
		if (z->key < x->key) {
			x = x->left;
		} else {
			x = x->right;
		}
	}
	if (x != nullptr && x->key == key) {
		throw; // TODO: better error here
	}
	z->parent = y;
	if (y == nullptr) {
		this->root = z;
	} else if (z->key < y->key) {
		y->left = z;
	} else {
		y->right = z;
	}
	this->size++;
	this->fixup(z);
}


template <typename K, typename V>
void TangoTree<K, V>::fixup(TangoNode<K, V> *z) {
	if (z == nullptr ||
	    z->parent == nullptr ||
	    z->parent->parent == nullptr) {
		// No need to fix up small trees.
		return;
	}
	while (is_red(z->parent)) {
		if (z->parent == z->parent->parent->left) {
			auto y = z->parent->parent->right;
			if (is_red(y)) {
				// CLRS case 1
				z->parent->info.red = false;
				y->info.red = false;
				z->parent->parent->info.red = true;
				z = z->parent->parent;
			} else {
				if (z == z->parent->right) {
					// CLRS case 2
					z = z->parent;
					this->left_rotate(z);
				}
				// CLRS case 3
				z->parent->info.red = false;
				z->parent->parent->info.red = true;
				this->right_rotate(z->parent->parent);
			}
		} else {
			auto y = z->parent->parent->left;
			if (is_red(y)) {
				// symmetric with CLRS case 1
				z->parent->info.red = false;
				z->parent->info.red = false;
				y->info.red = false;
				z->parent->parent->info.red = true;
				z = z->parent->parent;
			} else {
				if (z == z->parent->left) {
					// symmetric with CLRS case 2
					z = z->parent;
					this->right_rotate(z);
				}
				// symmetric with CLRS case 3
				z->parent->info.red = false;
				z->parent->info.red = false;
				z->parent->parent->info.red = true;
				this->left_rotate(z->parent->parent);
			}
		}
	}
	this->root->info.red = false;
}

template <typename K, typename V>
void TangoTree<K, V>::remove(K key) {
	// For simplicity, we delete lazily and amortize.
	// TODO: are we also assuming unique keys?
	if (this->locked) {
		throw;
	}
	if (this->root == nullptr) {
		return;
	}
	auto node = this->root->search(key);
	if (node == nullptr || node->info.deleted) {
		return;
	}
	node->info.deleted = true;
	this->deleted++;
	if (this->deleted > this->size / 2) {
		this->rebuild();
	}
}

template<typename K, typename V>
TangoNode<K, V> *TangoTree<K, V>::find(K key) {
	// TODO
	if (!this->locked) {
		// In the initial stage, the tree is write-only.
		throw;
	}
	return nullptr;
}

template <typename K, typename V>
void TangoTree<K, V>::rebuild() {
	// Builds a perfect BST from the non-deleted nodes in the tree.
	if (this->root == nullptr) {
		return;
	}
	vector<TangoNode<K, V> *> in_order;
	in_order_traverse(this->root, in_order);
	this->root = build_perfect(in_order, 0, 0, in_order.size() - 1);
	this->size -= this->deleted;
	this->deleted = 0;
}

template <typename K, typename V>
void in_order_traverse(TangoNode<K, V> *root,
		       vector<TangoNode<K, V> *> &nodes) {
	if (root->left != nullptr) {
		in_order_traverse(root->left, nodes);
	}
	if (!root->info.deleted) {
		nodes.push_back(root);
	}
	if (root->right != nullptr) {
		in_order_traverse(root->right, nodes);
	}
}

template <typename K, typename V>
TangoNode<K, V> *build_perfect(vector<TangoNode<K, V> *> &nodes,
			       int depth,
			       long start,
			       long end) {
       //std::cout << "start: " << start << "\tend: " << end << std::endl;
       if (start > end) {
	       return nullptr;
       }
       auto mid = (start + end) / 2;
       auto root = nodes[mid];
       std::cout << "key: " << root->key << std::endl;
       root->info.perfect_depth = depth;
       root->info.red = depth % 2 == 1;
       if (end > start) {
	       root->left = build_perfect(nodes, depth + 1, 0, mid - 1);
	       root->right = build_perfect(nodes, depth + 1, mid + 1, end);
       } else {
	       root->left = nullptr;
	       root->right = nullptr;
       }
       return root;
}

template <typename K, typename V>
void TangoTree<K, V>::lock() {
	// For now, trees should be made read-only after the initial insertions
	// and deletions.
	this->locked = true;
	this->rebuild();
}

template <typename K, typename V>
void TangoTree<K, V>::unlock() {
	this->rebuild();
	this->locked = false;
}

template <typename K, typename V>
void TangoTree<K, V>::split(K key) {
}
#endif

