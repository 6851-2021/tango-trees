#include "src/item.h"
#include "src/multisplay.hpp"
#include "src/rb.hpp"
#include <vector>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

bool DEBUG = false;

cNode<int, int> *make_perfect_bst(int n, int offset, int d)
{
        if (n == 0) {
                return nullptr;
        }
        auto pivot = (1 << (n - 1)) - 1;
        auto left = make_perfect_bst(n - 1, offset, d);
        auto right = make_perfect_bst(n - 1, offset + (1 << (n - 1)), d);
        auto node = new cNode<int, int>(pivot + offset, pivot + offset);
        node->left = left;
        node->right = right;
        if (left != nullptr) {
                left->parent = node;
        }
        if (right != nullptr) {
                right->parent = node;
        }
        if (left != nullptr) {
                node->info.depth = left->info.depth - 1;
                node->info.mindepth = node->info.depth;
        } else {
                node->info.depth = d;
                node->info.mindepth = d;
        }
        if (right != nullptr) {
                right->parent = node;
                right->info.isroot = true;
        }
        if (left != nullptr) {
                left->parent = node;
                left->info.isroot = false;
        }
        return node;
}

MultiSplayTree<int, int> multi_splay_tree_init(int N)
{
        auto root = make_perfect_bst(N, 0, N);
        root->parent = nullptr;
        root->info.isroot = true;
        auto T = MultiSplayTree<int, int>();
        T.root = root;
        return T;
}

void multi_splay_tree_test(int n)
{
        int N = (1 << n) - 1;
        auto T = multi_splay_tree_init(n);
        srand(time(NULL));

        std::vector<int> queries;

        for (int i = 0; i < N; i++) {
                auto x = rand() % N;
                queries.push_back(x);
        }
        auto start = clock();
        for (int i = 0; i < N; i++) {
                T.find(queries[i]);
                if (DEBUG) {
                        std::cout << "==============\n";
                        std::cout << "root: " << T.root->val << std::endl;
                        print(T.root);
                        std::cout << std::endl;
                }
        }
        auto elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        std::cout << "MultiSplay tree find queries took " << elapsed
                  << " seconds" << std::endl;
        //print(T.root);
}

void tango_tree_test(int n)
{
        TangoTree<int, int> T = TangoTree<int, int>();
        for (int i = 0; i < n; i++) {
                std::cout << "inserting " << i << std::endl;
                T.insert(i, i);
        }
        T.lock();
        T.split_at(300);
        /*
	T.unlock();
	for (int i = 0; i < n; i++) {
		std::cout << "removing " << i << std::endl;
		T.remove(i);
	}
	*/
}

void splay_tree_test(int N)
{
        srand(time(NULL));

        SplayTree<int, int> T = SplayTree<int, int>();
        std::set<int> S;
        // Insertions and deletions
        std::vector<int> I, D;
        for (int i = 0; i < N; i++) {
                int x = rand() % N;
                I.push_back(i);
        }
        for (int i = 0; i < N / 2; i++) {
                int x = rand() % N;
                D.push_back(i);
        }

        if (DEBUG) {
                std::cout << "I: ";
                for (auto i : I) {
                        std::cout << i << ", ";
                }
                std::cout << "\n";
                std::cout << "D: ";
                for (auto i : D) {
                        std::cout << i << ", ";
                }
                std::cout << "\n";
        }
        auto start = clock();
        for (int i = 0; i < N; i++) {
                S.insert(I[i]);
        }
        // for (int i=0; i < N/2; i++) {
        //   S.erase(D[i]);
        // }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        std::cout << "C++ std set insertion took " << elapsed << " seconds"
                  << std::endl;
        start = clock();
        for (int i = 0; i < N; i++) {
                T.insert(I[i], I[i]);
        }
        // for (int i=0; i < N/2; i++) {
        //   T.remove(D[i]);
        // }
        elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        std::cout << "Splay tree insertion took " << elapsed << " seconds"
                  << std::endl;

        for (int i = 0; i < N; i++) {
                assert((S.find(i) != S.end()) == (T.find(i) != nullptr));
        }

        start = clock();
        for (int i = 0; i < N; i++) {
                int x = rand() % N;
                S.find(x);
        }
        elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        std::cout << "C++ std set find queries took " << elapsed << " seconds"
                  << std::endl;
        rotations1 = 0;
        start = clock();
        for (int i = 0; i < N; i++) {
                int x = rand() % N;
                T.find(x);
        }
        elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        std::cout << "Splay tree find queries took " << elapsed << " seconds"
                  << std::endl;
}

int main(int argc, char **argv)
{
        tango_tree_test(1000);
        /*
  int n = 20;
  splay_tree_test((1 << n) - 1);
  multi_splay_tree_test(n);
  std::cout << "Number of nodes: " << (1 << n) - 1 << std::endl;
  std::cout << "Splay trees:  " << rotations1 << "\nMultiSplay trees: " << rotations2 << std::endl;
  */
}
