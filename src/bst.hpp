template <typename K, typename V>
class BSTNode {
public:
	BSTNode(K key, V val, BSTNode* parent);
	void rotate();
	K key;
	V val;
	BSTNode* left;
	BSTNode* right;
	BSTNode* parent;
};

template <typename K, typename V>
class BST {
public:
	BST() : root(nullptr) {}
    void insert(K key, V val) = 0;
    void remove(K key) = 0;
    V find(K key) = 0;
	BSTNode<K, V>* root;
};