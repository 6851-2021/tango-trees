#include "bst.hpp"

#ifndef TANGO_TREES_SPLAY_H
#define TANGO_TREES_SPLAY_H
template<typename K, typename V>
class SplayTree : public BST<K, V> {
public:
    SplayTree() : BST<K, V>() {}
    void insert(K key, V val);
    void remove(K key);
    BSTNode<K, V>* find(K key);
};

template <typename K, typename V>
void splay(BSTNode<K, V>* node) {
    if (node->parent == nullptr) {
        return;
    }
    BSTNode<K, V>* p = node->parent;
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

template <typename K, typename V>
BSTNode<K, V>* join(BSTNode<K, V>* left, BSTNode<K, V>* right) {
    if (left == nullptr) {
        return right;
    }
    BSTNode<K, V>* left_largest = left->max();
    splay(left_largest);
    left_largest->right = right;
    if (right != nullptr) {
        right->parent = left_largest;
    }
    return left_largest;
}

template <typename K, typename V>
void SplayTree<K, V>::insert(K key, V val) {
    if (this->root == nullptr) {
        this->root = new BSTNode<K, V>(key, val, nullptr);
        return;
    }
    BSTNode<K, V>* node = this->root->find(key);
    BSTNode<K, V>* new_node;
    if (key == node->key) {
        node->val = val;
        new_node = node;
    } else {
        new_node = new BSTNode<K, V>(key, val, node);
        if (key < node->key) {
            node->left = new_node;
        } else {
            node->right = new_node;
        }
    }
    splay(new_node);
    this->root = new_node;
}

template <typename K, typename V>
void SplayTree<K, V>::remove(K key) {
    BSTNode<K, V>* node = this->root->find(key);
    if (node->key == key) {
        splay(node);
        this->root = join(node->left, node->right);
    }
}

template <typename K, typename V>
BSTNode<K, V>* SplayTree<K, V>::find(K key) {
    BSTNode<K, V>* node = this->root->find(key);
    if (node->key == key) {
        splay(node);
        this->root = node;
        return node;
    }
    return nullptr;
}

#endif