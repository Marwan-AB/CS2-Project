#pragma once
#include <iostream>
#include <vector>

using namespace std;

template<typename T>
class AVLTree {
private:
    struct Node {
        T key;
        int height;
        Node* left;
        Node* right;

        Node(const T& k) : key(k), height(1), left(nullptr), right(nullptr) {}
    };

    Node* root;

    void destroy(Node* node);
    Node* insert(Node* node, const T& key);
    Node* remove(Node* node, const T& key);
    bool contains(Node* node, const T& key) const;
    int height(Node* node) const;
    int getBalance(Node* node) const;
    Node* rotateLeft(Node* x);
    Node* rotateRight(Node* y);
    Node* minValueNode(Node* node);
    void inOrder(Node* node, vector<T>& result) const;
    void levelOrder(Node* node, vector<T>& result) const;

public:
    AVLTree();
    ~AVLTree();

    void insert(const T& key);
    void remove(const T& key);
    bool contains(const T& key) const;
    vector<T> inOrderTraversal() const;
    vector<T> levelOrderTraversal() const;
    void clear();
    void clear(Node* node);    
};

#include "AVLTree.tpp"