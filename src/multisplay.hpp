#include "splay.hpp"

#ifndef TANGO_TREES_MULTISPLAY_H
#define TANGO_TREES_MULTISPLAY_H

struct Info {
  int depth;
  int mindepth;
  bool isroot;
};

template<typename K, typename V>
using cNode = BSTNode<K, V, Info>;

template<typename K, typename V>
class MultiSplayTree : public BST<K, V, Info> {
public:
  MultiSplayTree() : BST<K, V, Info>() {}

  void Splay(cNode<K, V>* node);
  void Splay(cNode<K, V>* node, cNode<K, V>* ancestor);

  void Left2Right(cNode<K, V>* y);

  void Right2Left(cNode<K, V>* y);

  cNode<K, V>* Query(K key, cNode<K, V>* cp, cNode<K, V>* lower, cNode<K, V>* upper);

  void insert(K key, V val) {};

  void remove(K key) {};

  cNode<K, V> *find(K key);
};

void print(cNode<int, int>* node) {
  if (node == nullptr) { return; }
  print(node->left);
  std::cout << node->val << ": (" << "l: ";
  if (node->left != nullptr) {
    std::cout << node->left->key;
    if (!node->left->info.isroot) {
      std::cout << " (preferred)";
    }
  }
  std::cout << ", r: ";
  if (node->right != nullptr) {
    std::cout << node->right->key;
    if (!node->right->info.isroot) {
      std::cout << " (preferred)";
    }
  }
  std::cout << ", p: ";
  if (node->parent != nullptr) {
    std::cout << node->parent->key;
  }
  std::cout << ", depth: " << node->info.depth;
  std::cout << ", mindepth: " << node->info.mindepth;
  std::cout << ", isroot: " << node->info.isroot << ")\n";
  
  print(node->right);
}

int rotations2 = 0;

template<typename K, typename V>
void rotate(cNode<K, V>* node) {
  rotations2 += 1;
  if (node->parent == nullptr) {
    return;
  }
  auto p = node->parent;
  node->rotate();
  node->info.mindepth = p->info.mindepth;
  p->info.mindepth = p->info.depth;
  if (p->left != nullptr && p->left->info.mindepth < p->info.mindepth) {
    p->info.mindepth = p->left->info.mindepth;
  }
  if (p->right != nullptr && p->right->info.mindepth < p->info.mindepth) {
    p->info.mindepth = p->right->info.mindepth;
  }
}


template<typename K, typename V>
bool SplayStep(cNode<K, V>* node, cNode<K, V>* ancestor) {
  auto p = node->parent;
  if (p == ancestor) {
    return true;
  }
  if (p->parent == ancestor) {
    // zig
    rotate(node);
    return true;
  }
  if (node->is_left_child() == p->is_left_child()) {
    // zig-zig
    rotate(p);
    rotate(node);
  } else {
    // zig-zag
    rotate(node);
    rotate(node);
  }
  return false;
}

template<typename K, typename V>
bool SplayStep(cNode<K, V>* node) {
  auto p = node->parent;
  if (p == nullptr || node->info.isroot) {
    return true;
  }
  if (p->info.isroot) {
    // zig
    rotate(node);
    p->info.isroot = false;
    node->info.isroot = true;
    return true;
  }
  auto g = p->parent;
  if (node->is_left_child() == p->is_left_child()) {
    // zig-zig
    rotate(p);
    rotate(node);
  } else {
    // zig-zag
    rotate(node);
    rotate(node);
  }
  if (g->info.isroot) {
    g->info.isroot = false;
    node->info.isroot = true;
    return true;
  }
  return false;
}

template<typename K, typename V>
void MultiSplayTree<K, V>::Splay(cNode<K, V> *node, cNode<K, V>* ancestor) {
  while (true) {
    if (SplayStep(node, ancestor)) { break; }
  }
  if (node->parent == nullptr) {
    this->root = node;
    node->info.isroot = true;
  }
}

template<typename K, typename V>
void MultiSplayTree<K, V>::Splay(cNode<K, V> *node) {
  while (true) {
    if (SplayStep(node)) { break; }
  }
  if (node->parent == nullptr) {
    this->root = node;
    node->info.isroot = true;
  }
}

template<typename K, typename V>
cNode<K, V>* rightmost(cNode<K, V>* node, int depth) {
  if (node == nullptr || node->info.isroot || node->info.mindepth >= depth) {
    return nullptr;
  }
  auto right = rightmost(node->right, depth);
  if (right != nullptr) { return right; }
  if (node->info.depth < depth) return node;
  return rightmost(node->left, depth);
}

template<typename K, typename V>
cNode<K, V>* leftmost(cNode<K, V>* node, int depth) {
  if (node == nullptr || node->info.isroot || node->info.mindepth >= depth) {
    return nullptr;
  }
  auto left = leftmost(node->left, depth);
  if (left != nullptr) { return left; }
  if (node->info.depth < depth) return node;
  return leftmost(node->right, depth);
}

template<typename K, typename V>
void recompute(cNode<K, V>* node) {
  auto mindepth = node->info.depth;
  if (node->left != nullptr && !node->left->info.isroot && node->left->info.mindepth < mindepth) {
    mindepth = node->left->info.mindepth;
  }
  if (node->right != nullptr && !node->right->info.isroot && node->right->info.mindepth < mindepth) {
    mindepth = node->right->info.mindepth;
  }
  node->info.mindepth = mindepth;
}

template<typename K, typename V>
void MultiSplayTree<K, V>::Left2Right(cNode<K, V>* y){
  Splay(y);
  auto lower = rightmost(y->left, y->info.depth);
  auto upper = leftmost(y->right, y->info.depth);
  if (lower != NULL) {
    Splay(lower, y);
    lower->right->info.isroot = true;
    recompute(lower);
  }
  else y->left->info.isroot = true;
  if (upper != nullptr) {
    Splay(upper, y);
    upper->left->info.isroot = false;
    recompute(upper);
  }
  else y->right->info.isroot = false;
  recompute(y);
}

template<typename K, typename V>
void MultiSplayTree<K, V>::Right2Left(cNode<K, V>* y){
  Splay(y);
  auto lower = rightmost(y->left, y->info.depth);
  auto upper = leftmost(y->right, y->info.depth);
  if (upper != NULL) {
    Splay(upper, y);
    upper->left->info.isroot = true;
    recompute(upper);
  }
  else y->right->info.isroot = true;
  if (lower != nullptr) {
    Splay(lower, y);
    lower->right->info.isroot = false;
    recompute(lower);
  }
  else y->left->info.isroot = false;
  recompute(y);
}

template<typename K, typename V>
cNode<K, V>* MultiSplayTree<K, V>::Query(K key, cNode<K, V>* cp, cNode<K, V>* lower, cNode<K, V>* upper) {
  if (cp->info.isroot) {lower = nullptr; upper = nullptr;}
  if (cp->key == key) {return cp; }
  if (key > cp->key) {lower = cp; cp = cp->right;}
  else if (key < cp->key) {upper = cp; cp = cp->left;}
  int lower_depth = 0, upper_depth = 0;
  if (lower != nullptr) { lower_depth = lower->info.depth; }
  if (upper != nullptr) { upper_depth = upper->info.depth; }
  if (cp->info.isroot){
    if (lower_depth > upper_depth) Left2Right(lower);
    if (upper_depth > lower_depth) Right2Left(upper);
  }
  cNode<K, V>* result = Query(key, cp, lower, upper);
  return result;
}

template<typename K, typename V>
cNode<K, V>* MultiSplayTree<K, V>::find(K key) {
  if (this->root == nullptr) {
    return nullptr;
  }
  cNode<K, V>* result = Query(key, this->root, nullptr, nullptr);
  Splay(result);
  return result;
}




#endif
