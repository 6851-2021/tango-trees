#include <iostream>

#ifndef TANGO_TREES_BST_H
#define TANGO_TREES_BST_H
template <typename K, typename V>
class BSTNode {
public:
    K key;
    V val;
    BSTNode* left;
    BSTNode* right;
    BSTNode* parent;

	BSTNode(K key, V val, BSTNode* parent);
    void print();
    void println();
    bool is_left_child();
	void update_parent_ptr(BSTNode* new_node);

    // Simple tree traversal: returns node with key new_key
    // or leaf node where new_key would be inserted
    BSTNode* find(K new_key);

    // Simple rotation of node with its parent
    void rotate();

    // Minimum element in subtree
    BSTNode* min();

    // Maximum element in subtree
    BSTNode* max();
};

template <typename K, typename V>
class BST {
public:
	BST() : root(nullptr) {}
    virtual void insert(K key, V val) = 0;
    virtual void remove(K key) = 0;
    virtual BSTNode<K, V>* find(K key) = 0;
	BSTNode<K, V>* root;
};

template <typename K, typename V>
BSTNode<K, V>::BSTNode(K key, V val, BSTNode<K, V>* parent) {
    this->key = key;
    this->val = val;
    this->parent = parent;
    this->left = nullptr;
    this->right = nullptr;
}

template <typename K, typename V>
void BSTNode<K, V>::print() {
    std::cout << "(" << val << ", [l: ";
    if (left != nullptr) {
        left->print();
    }
    std::cout << "], [r: ";
    if (right != nullptr) {
        right->print();
    }
    std::cout << "], [p: ";
    if (parent != nullptr) {
        std::cout << parent->val;
    } else {
        std::cout << "null";
    }
    std::cout << "])";
}

template <typename K, typename V>
void BSTNode<K, V>::println() {
    print();
    std::cout << std::endl;
}

template <typename K, typename V>
bool BSTNode<K, V>::is_left_child() {
    return (parent != nullptr) && (parent->left == this);
}

template <typename K, typename V>
void BSTNode<K, V>::update_parent_ptr(BSTNode<K, V>* new_node) {
    if (parent == nullptr) {
        return;
    }
    if (parent->left == this) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }
}

template <typename K, typename V>
void BSTNode<K, V>::rotate() {
    auto p = parent;
    if (p == nullptr) {
        return;
    }
    p->update_parent_ptr(this);
    auto g = p->parent;
    if (p->left == this) {
        if (right != nullptr) {
            right->parent = p;
        }
        p->left = right;
        right = p;
    } else {
        if (left != nullptr) {
            left->parent = p;
        }
        p->right = left;
        left = p;
    }
    p->parent = this;
    parent = g;
}

template <typename K, typename V>
BSTNode<K, V>* BSTNode<K, V>::find(K new_key) {
    if (new_key == this->key) {
        return this;
    } else if (new_key < this->key) {
        if (this->left != nullptr) {
            return this->left->find(new_key);
        }
        return this;
    } else {
        if (this->right != nullptr) {
            return this->right->find(new_key);
        }
        return this;
    }
}

template <typename K, typename V>
BSTNode<K, V>* BSTNode<K, V>::min() {
    BSTNode<K, V>* current = this;
    while (this->left != nullptr) {
        current = current->left;
    }
    return current;
}

template <typename K, typename V>
BSTNode<K, V>* BSTNode<K, V>::max() {
    BSTNode<K, V>* current = this;
    while (this->right != nullptr) {
        current = current->right;
    }
    return current;
}

#endif