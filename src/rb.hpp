#include "bst.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include <utility>

using std::vector;
using std::pair, std::make_pair;

#ifndef TANGO_TREES_RB_H
#define TANGO_TREES_RB_H

struct TInfo {
        // "In addition to storing the key value and depth, each node
        // stores the minimum and maximum depth over the nodes in its subtree."
        int black_height;
        int perfect_depth;
        int min_depth;
        int max_depth;
        bool red;
        bool deleted;
        bool marked;

        TInfo()
        {
                this->black_height = -1;
                this->perfect_depth = -1;
                this->min_depth = -1;
                this->max_depth = -1;
                this->red = true;
                this->marked = false;
                this->deleted = false;
        }
};

template <typename K, typename V> using TangoNode = BSTNode<K, V, TInfo>;

template <typename K, typename V> class TangoTree : public BST<K, V, TInfo> {
    private:
        void rebuild();
        void left_rotate(TangoNode<K, V> *x);
        void right_rotate(TangoNode<K, V> *x);
        void fixup(TangoNode<K, V> *x);
        TangoNode<K, V> *concatenate_left(TangoNode<K, V> *left,
                                          TangoNode<K, V> *pivot,
                                          TangoNode<K, V> *right);
        TangoNode<K, V> *concatenate_right(TangoNode<K, V> *left,
                                           TangoNode<K, V> *pivot,
                                           TangoNode<K, V> *right);
        TangoNode<K, V> *concatenate(TangoNode<K, V> *root);
	TangoNode<K, V> *join_helper(TangoNode<K, V> *left,
				     TangoNode<K, V> *root,
				     TangoNode<K, V> *right);
	std::pair<TangoNode<K, V> *, TangoNode<K, V> *> split_helper(TangoNode<K, V> *root, K key);

        void split(TangoNode<K, V> *root, TangoNode<K, V> *pivot);
        bool locked = false;
        long size = 0;
        long deleted = 0;

    public:
        TangoTree() : BST<K, V, TInfo>()
        {
        }
        void print();
        void insert(K key, V val);
        void remove(K key);
        void lock();
        void unlock();
	std::pair<TangoNode<K, V> *, TangoNode<K, V> *> split_at(K key);
  void cut(TangoNode<K, V> * root, int depth);
  void join(TangoNode<K, V> * root, TangoNode<K, V> * other);
        TangoNode<K, V> *find(K key);
};


template<typename K, typename V>
void print_with_depth(TangoNode<K, V>* node, int depth) {
  if (node == nullptr) {
    return;
  }
  for (int i=0; i < depth; i++) {
    std::cout << "  ";
  }
  std::cout << "(" << node->val << ", [l: ";
  if (node->left != nullptr) { std::cout << node->left->val; }
  std::cout << "], [r: ";
  if (node->right != nullptr) { std::cout << node->right->val; }
  std::cout << "], [p: ";
  if (node->parent != nullptr) { std::cout << node->parent->val; }
  std::cout << "], [mk: " << node->info.marked << "], [mind: " << node->info.min_depth;
  std::cout << "], [maxd: " << node->info.max_depth << "], [depth: ";
  std::cout << node->info.perfect_depth << "])\n";
  print_with_depth(node->left, depth + 1);
  print_with_depth(node->right, depth + 1);
}

template <typename K, typename V>
void TangoTree<K, V>::print() {
  print_with_depth(this->root, 0);
}

template <typename K, typename V>
void print_node(TangoNode<K, V>* node) {
  print_with_depth(node, 0);
}

template <typename K, typename V> bool is_black(TangoNode<K, V> *node)
{
        // Recommended by Wein 2001 in lieu of a sentinel leaf node.
        return node == nullptr || !(node->info.red);
}

template <typename K, typename V> bool is_red(TangoNode<K, V> *node)
{
        return node != nullptr && node->info.red;
}

template <typename K, typename V> int black_height(TangoNode<K, V> *node)
{
        // Computes the black height starting from a node.
        int height = 0;
        while (node != nullptr && !node->info.marked) {
                height += !(node->info.red);
                node = node->left;
        }
        return height;
}

template <typename K, typename V>
std::pair<TangoNode<K, V> *, TangoNode<K, V> *>
TangoTree<K, V>::split_at(K key) {
	auto split_node = this->root->search(key);
	this->split(this->root, split_node);
	return make_pair<>(this->root->left, this->root->right);
}

template <typename K, typename V>
void recompute(TangoNode<K, V>* node) {
  auto mindepth = node->info.perfect_depth;
  if (node->left != nullptr && !node->left->info.marked && node->left->info.min_depth < mindepth) {
    mindepth = node->left->info.min_depth;
  }
  if (node->right != nullptr && !node->right->info.marked && node->right->info.min_depth < mindepth) {
    mindepth = node->right->info.min_depth;
  }
  node->info.min_depth = mindepth;

  auto maxdepth = node->info.perfect_depth;
  if (node->left != nullptr && !node->left->info.marked && node->left->info.max_depth > maxdepth) {
    maxdepth = node->left->info.max_depth;
  }
  if (node->right != nullptr && !node->right->info.marked && node->right->info.max_depth < maxdepth) {
    maxdepth = node->right->info.max_depth;
  }
  node->info.max_depth = maxdepth;
}

template <typename K, typename V>
void recompute_all(TangoNode<K, V>* node) {
	// Recomputes min/max depth augmentation all the way up the tree.
	if (node != nullptr) {
		recompute(node);
		if (!node->info.marked) {
			recompute_all(node->parent);
		}
	}
}

template <typename K, typename V>
void TangoTree<K, V>::left_rotate(TangoNode<K, V> *x)
{
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
	if (x->info.marked) {
		x->info.marked = false;
		y->info.marked = true;
	}
        y->left = x;
        x->parent = y;
	y->info.min_depth = x->info.min_depth;
	y->info.max_depth = x->info.max_depth;
	recompute(x);
}

template <typename K, typename V>
void TangoTree<K, V>::right_rotate(TangoNode<K, V> *x)
{
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
	if (x->info.marked) {
		x->info.marked = false;
		y->info.marked = true;
	}
        y->right = x;
        x->parent = y;
	y->info.min_depth = x->info.min_depth;
	y->info.max_depth = x->info.max_depth;
	recompute(x);
}

template <typename K, typename V> void TangoTree<K, V>::insert(K key, V val)
{
        if (this->locked) {
                throw; // TODO: better error here
        }
        TangoNode<K, V> *z = new TangoNode<K, V>(key, val, TInfo());
        TangoNode<K, V> *y = nullptr;
        TangoNode<K, V> *x = this->root;
        while (x != nullptr) {
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
void TangoTree<K, V>::fixup(TangoNode<K, V> *z)
{
        if (z == nullptr || z->parent == nullptr ||
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

template <typename K, typename V> void TangoTree<K, V>::remove(K key)
{
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

template <typename K, typename V> TangoNode<K, V> *TangoTree<K, V>::find(K key)
{
        // TODO
        if (!this->locked) {
                // In the initial stage, the tree is write-only.
                throw;
        }
	
}

template <typename K, typename V> void TangoTree<K, V>::rebuild()
{
        // Builds a perfect BST from the non-deleted nodes in the tree.
        if (this->root == nullptr) {
                return;
        }
        vector<TangoNode<K, V> *> in_order;
        in_order_traverse(this->root, in_order);
        this->root = build_perfect(in_order, 0, 0, in_order.size(), (TangoNode<K, V>*) nullptr);
	this->root->info.marked = true;
        this->size -= this->deleted;
        this->deleted = 0;
}

template <typename K, typename V>
void in_order_traverse(TangoNode<K, V> *root, vector<TangoNode<K, V> *> &nodes)
{
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
TangoNode<K, V> *build_perfect(vector<TangoNode<K, V> *> &nodes, int depth,
                               long start, long end, TangoNode<K, V>* parent)
{
        /**
	 * Builds a perfect red-black tree from an arbitrary tango tree.
	 *
	 * Used for locking and unlocking operations.
	 *
	 * @param nodes The nodes to rebalance, in key order.
	 * @param depth The current depth (used internally; should start at 0).
	 * @param left The leftmost index for the current range, inclusive.
	 *   (used internally; should start at 0).
	 * @param right The rightmost index for the current range, exclusive.
	 *   (used internally; should start at `nodes.size()`.)
	 * @return The new root node.
	 */
        if (start >= end) {
                return nullptr;
        }
        auto mid = (start + end) / 2;
        auto root = nodes[mid];
        root->info.marked = false;
        root->info.perfect_depth = depth;
        root->info.red = depth % 2 == 1;
	root->parent = parent;
        root->left = build_perfect(nodes, depth + 1, start, mid, root);
        root->right = build_perfect(nodes, depth + 1, mid + 1, end, root);
	root->info.min_depth = depth;
	root->info.max_depth = depth;

	// Hack to compute depths for perfect bst
	if (root->left != nullptr) {
	  root->info.max_depth = root->left->info.max_depth;
	}
	if (root->right != nullptr) {
	  root->info.max_depth = root->right->info.max_depth;
	}
	
        // if (root->left != nullptr && root->right != nullptr) {
        //         root->info.min_depth = std::min(root->right->info.min_depth,
        //                                         root->left->info.min_depth) +
        //                                1;
	        
        //         root->info.max_depth = std::max(root->right->info.max_depth,
        //                                         root->left->info.max_depth) +
        //                                1;
        // } else if (root->left != nullptr) {
        //         root->info.max_depth = root->left->info.max_depth;
        // } else if (root->right != nullptr) {
        //         root->info.max_depth = root->right->info.max_depth;
        // } else {
        //         root->info.max_depth = 0;
        // } 
	// TODO: update depths when doing tango operations.
        return root;
}

// template <typename K, typename V>
// void TangoTree<K, V>::split(TangoNode<K, V> *root, TangoNode<K, V> *s) {
// 	/**
// 	 * Splits a red-black tree, forming two separate red-black trees in the
// 	 * tree of trees.
// 	 *
// 	 * In the tree of trees, `root` is replaced with `s`. The children of
// 	 * `s` become a red-black tree with keys under `root` less than the
// 	 * key of `s` (left) and a separate red-black tree, possibly of
// 	 * different black height, with keys under `root` greater than the key
// 	 * of `s` (right)
// 	 *
// 	 * @param root The root of the red-black tree (within the tree of trees)
// 	 *     to split.
// 	 * @param s The node to split on.
// 	 * @return nothing -- the tree of trees is modified in place.
// 	 */
// 	if (root == nullptr || s == nullptr || root == s) {
// 		// Note: splitting on a null pointer and splitting an empty tree
// 		// are undefined behavior. More properly, we should throw an
// 		// exception in those cases.
// 		//
// 		// The *real* base case is when the split node is equal to the
// 		// root node, in which case there's no work to do.
// 		return;
// 	}
// 	if (s->key < root->key) {
// 		// The non-in-place version of this code (from Wikipedia) is
// 		// something to the effect of:
// 		//      (L',b,R') = split(L, k)
// 		//      return (L',b,join(R',m,R))
// 		// Instead of *returning* a modified version of root->left,
// 		// we *replace* root in the tree of trees with a modified
// 		// version of root->left.
// 		auto parent = root->parent;
// 		this->split(root->left, s);
// 		// TODO: this function doesn't actually exist yet.
// 		root->left->right = this->join(root->left->right,
// 					       root,
// 					       root->right);
// 		root->left->parent = parent;
// 		// TODO: Christian recommends we turn this into a helper.
// 		if (parent == nullptr) {
// 			this->root = root->left;
// 		} else if (parent->left == root) {
// 			parent->left = root->left;
// 		} else {
// 			parent->right = root->left;
// 		}
// 	} else {
// 		// The s->key > root->key case is symmetric to above.
// 	}

// }

template <typename K, typename V>
void set_children(TangoNode<K, V> *left, TangoNode<K, V> *root,
                  TangoNode<K, V> *right)
{
        root->left = left;
        root->right = right;
        if (left != nullptr) {
                left->parent = root;
        }
        if (right != nullptr) {
                right->parent = root;
        }
        recompute_all(root);
}

template <typename K, typename V>
TangoNode<K, V> *TangoTree<K, V>::concatenate_right(TangoNode<K, V> *left,
                                                    TangoNode<K, V> *pivot,
                                                    TangoNode<K, V> *right)
{
        assert(pivot != nullptr);
        int bh_left = black_height(left);
        int bh_right = black_height(right);
        if (bh_left == bh_right) {
		set_children(left, pivot, right);
                pivot->info.red = true;
                return pivot;
        }
        assert(left != nullptr);
        auto t = concatenate_right(left->right, pivot, right);
        set_children(left->left, left, t);
        assert(t != nullptr);
        if (is_black(left) && is_red(t) && is_red(t->right)) {
                t->right->info.red = false;
                this->left_rotate(left);
                return left->parent;
        }
        return left;
}

template <typename K, typename V>
TangoNode<K, V> *TangoTree<K, V>::concatenate_left(TangoNode<K, V> *left,
                                                   TangoNode<K, V> *pivot,
                                                   TangoNode<K, V> *right)
{
        assert(pivot != nullptr);
        int bh_left = black_height(left);
        int bh_right = black_height(right);
        if (bh_left == bh_right) {
		set_children(left, pivot, right);
                pivot->info.red = true;
                return pivot;
        }
        assert(right != nullptr);
        auto t = concatenate_left(left, pivot, right->left);
        set_children(t, right, right->right);
        if (is_black(right) && is_red(t) && is_red(t->left)) {
                t->left->info.red = false;
                this->right_rotate(right);
                return right->parent;
        }
        return right;
}

template <typename K, typename V>
TangoNode<K, V> *TangoTree<K, V>::concatenate(TangoNode<K, V> *root)
{
        assert(root != nullptr);
        int bh_left = black_height(root->left);
        int bh_right = black_height(root->right);
        if (bh_left > bh_right) {
                auto t = concatenate_right(root->left, root, root->right);
                if (is_red(t) && is_red(t->right)) {
                        t->info.red = false;
                }
                return t;
        } else if (bh_left < bh_right) {
                auto t = concatenate_left(root->left, root, root->right);
                if (is_red(t) && is_red(t->left)) {
                        t->info.red = false;
                }
                return t;
        } else if (is_black(root->left) && is_black(root->right)) {
                root->info.red = true;
        } else {
                root->info.red = false;
        }
        return root;
}


template <typename K, typename V>
void replace_child(TangoNode<K, V> *old_child, TangoNode<K, V> *new_child)
{
        if (old_child != nullptr && old_child->parent != nullptr) {
                if (old_child->parent->left == old_child) {
                        old_child->parent->left = new_child;
                }
                if (old_child->parent->right == old_child) {
                        old_child->parent->right = new_child;
                }
		if (new_child != nullptr) {
			new_child->parent = old_child->parent;
		}
		recompute_all(old_child->parent);
        }
}

template <typename K, typename V>
TangoNode<K, V> *TangoTree<K, V>::join_helper(TangoNode<K, V> *left,
					      TangoNode<K, V> *root,
					      TangoNode<K, V> *right)
{
        // if (left != nullptr) {left->println(); }
	// root->println();
        // if (right != nullptr) { right->println(); }
	replace_child(left,  (TangoNode<K, V> *) nullptr);
	replace_child(right, (TangoNode<K, V> *) nullptr);
	root->left = left;
        root->right = right;
        recompute_all(root);

	if (left != nullptr) {
		left->parent = root;
	}
	if (right != nullptr) {
		right->parent = root;
	}
	// std::cout << "Join helper before concat:\n";
	// if (left != nullptr) {left->println(); }
	// root->println();
        // if (right != nullptr) { right->println(); }
        auto r = this->concatenate(root);
	// std::cout << "Join helper after concat:\n";
	// if (r != nullptr) {r->println(); }
	return r;
}

template <typename K, typename V>
std::pair<TangoNode<K, V> *, TangoNode<K, V> *>
TangoTree<K, V>::split_helper(TangoNode<K, V> *root, K key)
{
        // pivot must a node be inside root  
        assert(root != nullptr);
	// std::cout << "splitting at " << root->key << std::endl;
        if (root->key == key) {
                return make_pair<>(root->left, root->right);
        }
        if (key < root->key) {
                auto t = split_helper(root->left, key);
		// std::cout << "split result left :\n";
		// this->print();
		auto right_side = join_helper(t.second, root, root->right);
                return make_pair<>(t.first, right_side);
        } else {
                auto t = split_helper(root->right, key);
		// std::cout << "split result right:\n";
		// this->print();
                auto left_side = join_helper(root->left, root, t.first);
                return make_pair<>(left_side, t.second);
        }
}

template <typename K, typename V>
void TangoTree<K, V>::split(TangoNode<K, V> *root, TangoNode<K, V> *pivot)
{
        auto root_parent = root->parent;
        auto result = split_helper(root, pivot->key);
        set_children(result.first, pivot, result.second);
	// std::cout << "root: " << root->val;
	// this->print();
	if (root_parent == nullptr) { this->root = pivot; }
        if (root_parent != nullptr) {
                if (root_parent->left == root) {
                        root_parent->left = pivot;
                } else if (root_parent->right == root) {
                        root_parent->right = pivot;
                }
        }
        pivot->parent = root_parent;
}


template <typename K, typename V>
TangoNode<K, V> * leftmost(TangoNode<K, V> *node, int depth) {
  if (node == nullptr || node->info.marked || node->info.max_depth <= depth) {
    return nullptr;
  }
  auto left = leftmost(node->left, depth);
  if (left != nullptr) { return left; }
  if (node->info.depth > depth) { return node; }
  return leftmost(node->right, depth);
}


template <typename K, typename V>
TangoNode<K, V> * rightmost(TangoNode<K, V> *node, int depth) {
  if (node == nullptr || node->info.marked || node->info.max_depth <= depth) {
    return nullptr;
  }
  auto right = rightmost(node->right, depth);
  if (right != nullptr) { return right; }
  if (node->info.depth > depth) { return node; }
  return rightmost(node->left, depth);
}

template <typename K, typename V>
TangoNode<K, V> * predecessor(TangoNode<K, V> *node) {
  if (node == nullptr) {
    return nullptr;
  }
  if (node->left != nullptr) {
    auto curr = node->left;
    while (curr != nullptr && !curr->info.marked &&
	   curr->right != nullptr && !curr->right->info.marked) {
      curr = curr->right;
    }
    return curr;
  } else {
    auto curr = node;
    while (curr->parent != nullptr && !curr->info.marked) {
      if (curr->parent->right == curr) {
	return curr->parent;
      }
      curr = curr->parent;
    }
    return nullptr;
  }
}

template <typename K, typename V>
TangoNode<K, V> * successor(TangoNode<K, V> *node) {
  if (node == nullptr) {
    return nullptr;
  }
  if (node->right != nullptr) {
    auto curr = node->right;
    while (curr != nullptr && curr->left != nullptr &&
	   !curr->info.marked && !curr->left->info.marked) {
      curr = curr->left;
    }
    return curr;
  } else {
    auto curr = node;
    while (curr->parent != nullptr && !curr->info.marked) {
      if (curr->parent->left == curr) {
	return curr->parent;
      }
      curr = curr->parent;
    }
    return nullptr;
  }
}

template <typename K, typename V>
void TangoTree<K, V>::cut(TangoNode<K, V> *root, int depth) {
  assert(root->info.marked);
  auto l = predecessor(leftmost(root)); // l'
  auto r = successor(rightmost(root));  // r'

  if (l != nullptr && r != nullptr) {
	  // Range is (l, r).
	  this->split(root, l);
	  this->split(l->right, r);
	  if (r->left != nullptr) {
		  r->left->info.marked = true;
	  }
	  recompute_all(r);
	  recompute_all(l);
	  set_children(l->left, l, concatenate(r)); // right concat
	  replace_child(l, concatenate(l)); // left concat

  } else if (l != nullptr) {
	  // Range is (l, ∞) -- left split only.
	  this->split(root, l);
	  if (l->right != nullptr) {
		  l->right->info.marked = true;
	  }
	  recompute_all(l);
	  replace_child(l, concatenate(l)); // left concat
  } else if (r != nullptr) {
	  // Range is (-∞, r) -- right split only.
	  this->split(root, r);
	  if (r->left != nullptr) {
		  r->left->info.marked = true;
	  }
	  recompute_all(r);
	  replace_child(r, concatenate(r)); // right concat
  }
  // For (-∞, ∞), we do nothing.
}

template <typename K, typename V>
void TangoTree<K, V>::join(TangoNode<K, V> *root, TangoNode<K, V> *other) {




}

// template <typename K, typename V>
// void TangoTree<K, V>::concatenate(TangoNode<K, V> *root) {
// 	// These operations are O(log n) -- we can afford them once per split.
// 	// According to CLRS(?), the black height of a regular red-black tree
// 	// can be stored in O(1) space as global tree metadata. However, things
// 	// get more complicated in our implicit tree-of-trees representation---
// 	// it seems that we would need to store black height at each marked
// 	// node.
// 	if (root == nullptr) {
// 		return;
// 	}
// 	int bh_left = black_height(root->left);
// 	int bh_right = black_height(root->right);
// 	TangoNode<K, V> *new_root = nullptr;
// 	if (height_l > height_r) {
// 		new_root = this->concatenate_left_bigger(
// 			root, root->left, root->right
// 		);
// 		if (is_red(new_root) && is_red(new_root->right)) {
// 			new_root->info.red = false;
// 		}
// 	} else if (height_r > height_l) {
// 		new_root = this->concatenate_right_bigger(
// 			root, root->left, root->right
// 		);
// 		if (is_red(new_root) && is_red(new_root->left)) {
// 			new_root->info.red = false;
// 		}
// 	} else if (is_black(root->left) && is_black(root->right)) {
// 		// TODO: is this even necessary for our purposes?
// 		root->info.red = true;
// 		return;
// 	} else {
// 		root->info.red = false;
// 		return;
// 	}
// 	new_root->parent = root->parent;
// 	if (root == this->root) {
// 		this->root = new_root;
// 	} else if (root->parent->left == root) {
// 		root->parent->left = new_root;
// 	} else {
// 		root->parent->right = new_root;
// 	}
// }

// template <typename K, typename V>
// TangoNode<K, V> *
// TangoTree<K, V>::concatenate_left_bigger(TangoNode<K, V> *pivot,
// 					 TangoNode<K, V> *left,
// 					 TangoNode<K, V> *right) {
// 	// TODO: (!!!) Querying the black height at each step leads to
// 	// O(log^2 n) runtime.
// 	int bh_left = black_height(left);
// 	int bh_right = black_height(right);
// 	if (bh_left == bh_right) {
// 		pivot->left = left;
// 		pivot->right = right;
// 		pivot->info.color = red;
// 		return pivot;
// 	}
// 	// TODO: decrement black heights as we go down the trees.
// 	left->right = concatenate_left_bigger(pivot, left->right, right);
// 	if (is_black(left) &&
// 	    is_black(left->right) &&
// 	    is_black(left->right->right)) {
// 		left->right->right->info.red = false;
// 		return this->rotate_left(left);
// 	}
// 	return left;
// }

// template <typename K, typename V>
// TangoNode<K, V> *
// TangoTree<K, V>::concatenate_right_bigger(TangoNode<K, V> *pivot,
// 				  	  TangoNode<K, V> *left,
// 				  	  TangoNode<K, V> *right) {
// 	// TODO: (!!!) Querying the black height at each step leads to
// 	// O(log^2 n) runtime.
// 	int bh_left = black_height(left);
// 	int bh_right = black_height(right);
// 	if (bh_left == bh_right) {
// 		pivot->left = left;
// 		pivot->right = right;
// 		pivot->info.color = red;
// 		return pivot;
// 	}
// 	// TODO: decrement black heights as we go down the trees.
// 	right->left = concatenate_right_bigger(pivot, left, right->left);
// 	if (is_black(right) &&
// 	    is_black(right->left) &&
// 	    is_black(right->left->left)) {
// 		right->left->left->info.red = false;
// 		return this->rotate_right(right);
// 	}
// 	return right;
// }

template <typename K, typename V> void TangoTree<K, V>::lock()
{
        // For now, trees should be made read-only after the initial insertions
        // and deletions.
        this->locked = true;
        this->rebuild();
}

template <typename K, typename V> void TangoTree<K, V>::unlock()
{
        this->rebuild();
        this->locked = false;
}

#endif
