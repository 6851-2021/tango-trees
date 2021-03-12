template <typename K, typename V>
class BST {
    public:
    void insert(K key, V val) = 0;
    void remove(K key) = 0;
    V find(K key) = 0;
};