#include "bst.hpp"

#ifndef TANGO_TREES_SPLAY_H
#define TANGO_TREES_SPLAY_H

struct None {
};
None none = None();

template<typename K, typename V>
using SplayNode = BSTNode<K, V, None>;

template<typename K, typename V>
class SplayTree : public BST<K, V, None> {
public:
  SplayTree() : BST<K, V, None>() {}

  void insert(K key, V val);

  void remove(K key);

  BSTNode<K, V, None> *find(K key);
};

template<typename K, typename V>
void splay(SplayNode<K, V> *node) {
  if (node->parent == nullptr) {
    return;
  }
  SplayNode<K, V> *p = node->parent;
  if (p->parent == nullptr) {
    // zig
    node->rotate();
    return;
  }
  if (node->is_left_child() == p->is_left_child()) {
    // zig-zig
    p->rotate();
    node->rotate();
  } else {
    // zig-zag
    node->rotate();
    node->rotate();
  }
  splay(node);
}

template<typename K, typename V>
SplayNode<K, V> *join(SplayNode<K, V> *left, SplayNode<K, V> *right) {
  if (left == nullptr) {
    return right;
  }
  SplayNode<K, V> *left_largest = left->max();
  splay(left_largest);
  left_largest->right = right;
  if (right != nullptr) {
    right->parent = left_largest;
  }
  return left_largest;
}

template<typename K, typename V>
void SplayTree<K, V>::insert(K key, V val) {
  if (this->root == nullptr) {
    this->root = new SplayNode<K, V>(key, val);
    return;
  }
  SplayNode<K, V> *node = this->root->search(key);
  SplayNode<K, V> *new_node;
  if (key == node->key) {
    node->val = val;
    new_node = node;
  } else {
    new_node = new SplayNode<K, V>(key, val);
    new_node->parent = node;
    if (key < node->key) {
      node->left = new_node;
    } else {
      node->right = new_node;
    }
  }
  splay(new_node);
  this->root = new_node;
}

template<typename K, typename V>
void SplayTree<K, V>::remove(K key) {
  if (this->root == nullptr) {
    return;
  }
  SplayNode<K, V> *node = this->root->search(key);
  if (node->key == key) {
    splay(node);
    this->root = join(node->left, node->right);
    this->root->parent = nullptr;
    // TODO: deallocate node?
  }
}

template<typename K, typename V>
SplayNode<K, V> *SplayTree<K, V>::find(K key) {
  if (this->root == nullptr) {
    return nullptr;
  }
  SplayNode<K, V> *node = this->root->search(key);
  splay(node);
  this->root = node;
  if (node->key == key) {
    return node;
  }
  return nullptr;
}

#endif
