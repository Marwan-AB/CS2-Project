#pragma once
#include "AVLTree.h"
template<typename T>
AVLTree<T>::AVLTree() : root(nullptr) {}

template<typename T>
AVLTree<T>::~AVLTree() {
    destroy(root);
}

template<typename T>
void AVLTree<T>::destroy(Node* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

template<typename T>
int AVLTree<T>::height(Node* node) const {
    return node ? node->height : 0;
}

template<typename T>
int AVLTree<T>::getBalance(Node* node) const {
    return node ? height(node->left) - height(node->right) : 0;
}

template<typename T>
typename AVLTree<T>::Node* AVLTree<T>::rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

template<typename T>
typename AVLTree<T>::Node* AVLTree<T>::rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

template<typename T>
void AVLTree<T>::insert(const T& key) {
    root = insert(root, key);
}

template<typename T>
typename AVLTree<T>::Node* AVLTree<T>::insert(Node* node, const T& key) {
    if (!node) return new Node(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    if (balance > 1 && key < node->left->key) return rotateRight(node);
    if (balance < -1 && key > node->right->key) return rotateLeft(node);
    if (balance > 1 && key > node->left->key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && key < node->right->key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

template<typename T>
void AVLTree<T>::remove(const T& key) {
    root = remove(root, key);
}

template<typename T>
typename AVLTree<T>::Node* AVLTree<T>::minValueNode(Node* node) {
    Node* current = node;
    while (current->left) current = current->left;
    return current;
}

template<typename T>
typename AVLTree<T>::Node* AVLTree<T>::remove(Node* root, const T& key) {
    if (!root) return root;

    if (key < root->key)
        root->left = remove(root->left, key);
    else if (key > root->key)
        root->right = remove(root->right, key);
    else {
        if (!root->left || !root->right) {
            Node* temp = root->left ? root->left : root->right;
            delete root;
            return temp;
        }

        Node* temp = minValueNode(root->right);
        root->key = temp->key;
        root->right = remove(root->right, temp->key);
    }

    root->height = 1 + max(height(root->left), height(root->right));
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0) return rotateRight(root);
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0) return rotateLeft(root);
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

template<typename T>
bool AVLTree<T>::contains(const T& key) const {
    return contains(root, key);
}

template<typename T>
bool AVLTree<T>::contains(Node* node, const T& key) const {
    if (!node) return false;
    if (key < node->key) return contains(node->left, key);
    if (key > node->key) return contains(node->right, key);
    return true;
}

template<typename T>
vector<T> AVLTree<T>::inOrderTraversal() const {
    vector<T> result;
    inOrder(root, result);
    return result;
}

template<typename T>
void AVLTree<T>::inOrder(Node* node, vector<T>& result) const {
    if (!node) return;
    inOrder(node->left, result);
    result.push_back(node->key);
    inOrder(node->right, result);
}

template<typename T>
vector<T> AVLTree<T>::levelOrderTraversal() const {
    vector<T> result;
    levelOrder(root, result);
    return result;
}

template<typename T>
void AVLTree<T>::levelOrder(Node* root, vector<T>& result) const {
    if (!root) return;
    queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        Node* n = q.front(); q.pop();
        result.push_back(n->key);
        if (n->left) q.push(n->left);
        if (n->right) q.push(n->right);
    }
}

template <typename T>
void AVLTree<T>::clear() {
    clear(root);
    root = nullptr;
}

template <typename T>
void AVLTree<T>::clear(Node* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

