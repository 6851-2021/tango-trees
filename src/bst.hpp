#include <iostream>

#ifndef TANGO_TREES_BST_H
#define TANGO_TREES_BST_H

template<typename K, typename V, typename A>
class BSTNode {
public:
  K key;
  V val;
  A info;
  BSTNode *left;
  BSTNode *right;
  BSTNode *parent;

  BSTNode(K key, V val);

  BSTNode(K key, V val, A info);

  void print(int depth);
  
  void print();

  void println();

  bool is_left_child();

  BSTNode *sibling();

  // Replaces this with node by updating only pointers
  // between this and this->parent
  void replace(BSTNode *node);

  // Simple tree traversal: returns node with key new_key
  // or leaf node where new_key would be inserted
  BSTNode *search(K new_key);

  // Simple rotation of node with its parent
  void rotate();

  // Minimum element in subtree
  BSTNode *min();

  // Maximum element in subtree
  BSTNode *max();

  // Predecessor
  BSTNode *pred();

  // Successor
  BSTNode *succ();
};

template<typename K, typename V, typename A>
class BST {
public:
  BST() : root(nullptr) {}

  virtual void rotate(BSTNode<K, V, A> *node);

  void replace(BSTNode<K, V, A>* first, BSTNode<K, V, A>* second);

  virtual void insert(K key, V val) = 0;

  virtual void remove(K key) = 0;

  virtual BSTNode<K, V, A> *find(K key) = 0;

  BSTNode<K, V, A> *root;
};

template<typename K, typename V, typename A>
void BST<K, V, A>::rotate(BSTNode<K, V, A> *node) {
  node->rotate();
  if (node->parent == nullptr) {
    root = node;
  }
}

template<typename K, typename V, typename A>
void BST<K, V, A>::replace(BSTNode<K, V, A> *first, BSTNode<K, V, A> *second) {
  if (first->parent != nullptr) {
    if (first->parent->left == first) {
      first->parent->left = second;
    } else {
      first->parent->right = second;
    }
  } else {
    root = second;
  }
  if (second != nullptr) {
    second->parent = first->parent;
  }
}


template<typename K, typename V, typename A>
BSTNode<K, V, A>::BSTNode(K key, V val) {
  this->key = key;
  this->val = val;
  this->left = nullptr;
  this->right = nullptr;
}

template<typename K, typename V, typename A>
BSTNode<K, V, A>::BSTNode(K key, V val, A info) {
  this->key = key;
  this->val = val;
  this->info = info;
  this->left = nullptr;
  this->right = nullptr;
}

template<typename K, typename V, typename A>
void BSTNode<K, V, A>::print(int depth) {
  auto node = this;
  for (int i=0; i < depth; i++) {
    std::cout << "  ";
  }
  std::cout << "(" << node->val << ", [l: ";
  if (node->left != nullptr) {
    std::cout << node->left->val;
  }
  std::cout << "], [r: ";
  if (node->right != nullptr) {
    std::cout << node->right->val;
  }
  std::cout << "], [p: ";
  if (node->parent != nullptr) {
    std::cout << node->parent->val;
  }
  std::cout << "])";
  std::cout << "\n";
  if (node->left != nullptr) { node->left->print(depth + 1); }
  if (node->right != nullptr) { node->right->print(depth + 1); }
}

template<typename K, typename V, typename A>
void BSTNode<K, V, A>::print() {
  print(0);
}

template<typename K, typename V, typename A>
void BSTNode<K, V, A>::println() {
  print();
  std::cout << std::endl;
}


template<typename K, typename V, typename A>
void BSTNode<K, V, A>::replace(BSTNode<K, V, A>* node) {
  if (parent != nullptr) {
    if (parent->left == this) {
      parent->left = node;
    } else {
      parent->right = node;
    }
  }
  node->parent = parent;
}

template<typename K, typename V, typename A>
bool BSTNode<K, V, A>::is_left_child() {
  return (parent != nullptr) && (parent->left == this);
}

template<typename K, typename V, typename A>
BSTNode<K, V, A> *BSTNode<K, V, A>::sibling() {
  if (parent == nullptr) {
    return this;
  }
  if (parent->left == this) {
    return parent->right;
  } else {
    return parent->left;
  }
}

template<typename K, typename V, typename A>
void BSTNode<K, V, A>::rotate() {
  auto p = parent;
  if (p == nullptr) {
    return;
  }
  // update parent pointers
  parent->replace(this);
  if (p->left == this) {
    // left rotation
    if (right != nullptr) {
      right->parent = p;
    }
    p->left = right;
    right = p;
  } else {
    // right rotation
    if (left != nullptr) {
      left->parent = p;
    }
    p->right = left;
    left = p;
  }
  // update remaining pointers
  p->parent = this;
}

template<typename K, typename V, typename A>
BSTNode<K, V, A> *BSTNode<K, V, A>::search(K new_key) {
  auto curr = this;
  while (true) {
    if (new_key == curr->key) {
      return curr;
    }
    if (new_key < curr->key) {
      if (curr->left == nullptr) {
	return curr;
      }
      curr = curr->left;
    } else {
      if (curr->right == nullptr) {
	return curr;
      }
      curr = curr->right;
    }
  }
}

template<typename K, typename V, typename A>
BSTNode<K, V, A> *BSTNode<K, V, A>::min() {
  BSTNode<K, V, A> *current = this;
  while (current->left != nullptr) {
    current = current->left;
  }
  return current;
}

template<typename K, typename V, typename A>
BSTNode<K, V, A> *BSTNode<K, V, A>::max() {
  BSTNode<K, V, A> *current = this;
  while (current->right != nullptr) {
    current = current->right;
  }
  return current;
}

template<typename K, typename V, typename A>
BSTNode<K, V, A> *BSTNode<K, V, A>::pred() {
  if (left != nullptr) {
    return left->max();
  }
  auto curr = this;
  while (curr->parent != nullptr) {
    if (curr->parent->right == curr) {
      return curr->parent;
    }
  }
  return nullptr;
}

template<typename K, typename V, typename A>
BSTNode<K, V, A> *BSTNode<K, V, A>::succ() {
  if (right != nullptr) {
    return right->min();
  }
  auto curr = this;
  while (curr->parent != nullptr) {
    if (curr->parent->left == curr) {
      return curr->parent;
    }
  }
  return nullptr;
}

#endif
