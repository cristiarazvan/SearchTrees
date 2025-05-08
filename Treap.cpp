#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

ifstream fin("abce.in");
ofstream fout("abce.out");

struct TreapNode {
    int key, priority;
    TreapNode *left, *right;
};

TreapNode* rightRotate(TreapNode* y) {
    TreapNode *x = y->left;
    TreapNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    return x;
}

TreapNode* leftRotate(TreapNode* x) {
    TreapNode *y = x->right;
    TreapNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    return y;
}

TreapNode* newNode(int key) {
    TreapNode* temp = new TreapNode;
    temp->key = key;
    temp->priority = rand();
    temp->left = temp->right = NULL;
    return temp;
}

TreapNode* search(TreapNode* root, int key) {
    if (!root || root->key == key) return root;
    if (key < root->key) return search(root->left, key);
    return search(root->right, key);
}

TreapNode* insert(TreapNode* root, int key) {
    if (!root) return newNode(key);
    if (key < root->key) {
        root->left = insert(root->left, key);
        if (root->left->priority > root->priority)
            root = rightRotate(root);
    } else {
        root->right = insert(root->right, key);
        if (root->right->priority > root->priority)
            root = leftRotate(root);
    }
    return root;
}

TreapNode* deleteNode(TreapNode* root, int key) {
    if (!root) return root;
    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        if (!root->left) {
            TreapNode* temp = root->right;
            delete root;
            return temp;
        } else if (!root->right) {
            TreapNode* temp = root->left;
            delete root;
            return temp;
        }
        if (root->left->priority < root->right->priority) {
            root = leftRotate(root);
            root->left = deleteNode(root->left, key);
        } else {
            root = rightRotate(root);
            root->right = deleteNode(root->right, key);
        }
    }
    return root;
}

int findMaxLE(TreapNode* root, int x) {
    int res = -1e9 - 5;
    while (root) {
        if (root->key <= x) {
            res = root->key;
            root = root->right;
        } else {
            root = root->left;
        }
    }
    return res;
}

int findMinGE(TreapNode* root, int x) {
    int res = 1e9 + 5;
    while (root) {
        if (root->key >= x) {
            res = root->key;
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return res;
}

void rangeQuery(TreapNode* root, int x, int y, vector<int>& result) {
    if (!root) return;
    if (x < root->key)
        rangeQuery(root->left, x, y, result);
    if (x <= root->key && root->key <= y)
        result.push_back(root->key);
    if (y > root->key)
        rangeQuery(root->right, x, y, result);
}

int getMaxElement(TreapNode* root) {
    if (!root) return -1e9 - 5;
    while (root->right) {
        root = root->right;
    }
    return root->key;
}

pair<TreapNode*, TreapNode*> split(TreapNode* root, int key) {
    if (!root) return {nullptr, nullptr};
    
    if (root->key <= key) {
        pair<TreapNode*, TreapNode*> rightSplit = split(root->right, key);
        root->right = rightSplit.first;
        return {root, rightSplit.second};
    } else {
        pair<TreapNode*, TreapNode*> leftSplit = split(root->left, key);
        root->left = leftSplit.second;
        return {leftSplit.first, root};
    }
}

TreapNode* mergeTreaps(TreapNode* t1, TreapNode* t2) {
    if (!t1) return t2;
    if (!t2) return t1;
    
    if (t1->priority < t2->priority) {
        swap(t1, t2);
    }
    
    pair<TreapNode*, TreapNode*> splitResult = split(t2, t1->key);
    
    t1->left = mergeTreaps(t1->left, splitResult.first);
    t1->right = mergeTreaps(t1->right, splitResult.second);
    
    return t1;
}

int main() {
    srand(time(NULL));
    TreapNode* root = NULL;
    TreapNode* root2 = NULL;

    int Q;
    fin >> Q;
    for (int i = 0; i < Q; i++) {
        int op, x, y;
        fin >> op >> x;
        if (op == 1) {
            root = insert(root, x);
        } else if (op == 2) {
            root = deleteNode(root, x);
        } else if (op == 3) {
            fout << (search(root, x) ? 1 : 0) << '\n';
        } else if (op == 4) {
            fout << findMaxLE(root, x) << '\n';
        } else if (op == 5) {
            fout << findMinGE(root, x) << '\n';
        } else if (op == 6) {
            fin >> y;
            vector<int> result;
            rangeQuery(root, x, y, result);
            for (int val : result) fout << val << " ";
            fout << '\n';
        }
    }

    return 0;
}