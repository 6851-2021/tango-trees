#include "bst.hpp"
#include <iostream>

void update_parent_ptr(BSTNode* node, BSTNode* new_node) {
	BSTNode* parent = node->parent;
	if (parent == nullptr) {
		return;
	}
	if (parent->left == node) {
		parent->left = new_node;
	} else {
		parent->right = new_node;
	}
}

BSTNode::BSTNode(K key, V val, BSTNode* parent) {
	this->key = key;
	this->val = val;
	this->parent = parent
	this->left = nullptr;
	this->right = nullptr;
}

BSTNode::rotate() {
	if (parent == nullptr) {
		return;
	}
	if (parent->left == this) {
		parent->left = right;
		right = parent;
	} else {
		parent->right = left;
		left = parent;
	}
	update_parent_ptr(parent, this);
}