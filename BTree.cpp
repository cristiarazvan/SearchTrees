#include <climits>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

ifstream fin("abce.in");
ofstream fout("abce.out");

struct Node {
    int t;
    bool is_leaf;
    vector<int> keys;
    vector<Node*> children;

    Node(int _t, bool _leaf) : t(_t), is_leaf(_leaf) {
        keys.reserve(2 * t - 1);
        children.reserve(2 * t);
    }

    // traverse the subtree in sorted order
    void traverse() {
        for (int i = 0; i < keys.size(); i++) {
            if (!is_leaf) children[i]->traverse();
            cout << keys[i] << " ";
        }
        if (!is_leaf) children[keys.size()]->traverse();
    }
    // search for a key in the subtree
    Node* search(int val) {
        int i = 0;
        while (i < keys.size() && val > keys[i]) i++;
        if (i < keys.size() && keys[i] == val) return this;
        if (is_leaf) return nullptr;
        return children[i]->search(val);
    }

    // split full child
    void splitChild(int i, Node* fullChild) {
        Node* newChild = new Node(t, fullChild->is_leaf);
        for (int j = 0; j < t - 1; ++j)
            newChild->keys.push_back(fullChild->keys[j + t]);

        if (!fullChild->is_leaf) {
            for (int j = 0; j < t; ++j)
                newChild->children.push_back(fullChild->children[j + t]);
        }

        int median = fullChild->keys[t - 1];

        fullChild->keys.resize(t - 1);
        if (!fullChild->is_leaf) fullChild->children.resize(t);
        children.insert(children.begin() + i + 1, newChild);
        keys.insert(keys.begin() + i, median);
    }

    // insert val into a non-full node
    void insert(int val) {
        int i = keys.size() - 1;
        if (is_leaf) {
            keys.push_back(0);
            while (i >= 0 && keys[i] > val) {
                keys[i + 1] = keys[i];
                i--;
            }
            keys[i + 1] = val;
        } else {
            while (i >= 0 && keys[i] > val) --i;
            i++;
            if (children[i]->keys.size() == (t << 1) - 1) {
                splitChild(i, children[i]);
                if (keys[i] < val) i++;
            }
            children[i]->insert(val);
        }
    }

    // find the index of val in the node
    int findKeyIdx(int val) {
        /// Change to binary search maybe?
        int idx = 0;
        while (idx < keys.size() && keys[idx] < val) idx++;
        return idx;
    }

    // merge child[idx] with child[idx + 1]
    void merge(int idx) {
        Node* st = children[idx];
        Node* dr = children[idx + 1];
        st->keys.push_back(keys[idx]);
        for (int val : dr->keys) st->keys.push_back(val);
        if (!st->is_leaf)
            for (Node* nod : dr->children) st->children.push_back(nod);
        keys.erase(keys.begin() + idx);
        children.erase(children.begin() + idx + 1);
        delete dr;
    }

    // remove key val
    void remove(int val) {
        int idx = findKeyIdx(val);
        if (idx < keys.size() && keys[idx] == val) {
            if (is_leaf)
                keys.erase(keys.begin() + idx);
            else
                removeWithChange(idx);
        } else {
            if (is_leaf) {
                // cout << "Key not found\n";
                return;
            }
            bool atLast = (idx == keys.size());
            if (children[idx]->keys.size() < t) {
                fillChild(idx);
            }
            if (atLast && idx > keys.size())
                children[idx - 1]->remove(val);
            else
                children[idx]->remove(val);
        }
    }

    // remove from nonleaf node
    void removeWithChange(int idx) {
        int val = keys[idx];
        if (children[idx]->keys.size() >= t) {
            int x = getBiggest(idx);
            keys[idx] = x;
            children[idx]->remove(x);
        } else if (children[idx + 1]->keys.size() >= t) {
            int x = getSmallest(idx);
            keys[idx] = x;
            children[idx + 1]->remove(x);
        } else {
            merge(idx);
            children[idx]->remove(val);
        }
    }
    // get the biggest key in the left part
    int getBiggest(int idx) {
        Node* ac = children[idx];
        while (!ac->is_leaf) ac = ac->children[ac->keys.size()];
        return ac->keys.back();
    }
    // get the smallest key in the right part
    int getSmallest(int idx) {
        Node* ac = children[idx + 1];
        while (!ac->is_leaf) ac = ac->children[0];
        return ac->keys.front();
    }

    // borrow key from left
    void getFromPrev(int idx) {
        Node* son = children[idx];
        Node* sson = children[idx - 1];
        son->keys.insert(son->keys.begin(), keys[idx - 1]);
        if (!son->is_leaf) {
            son->children.insert(son->children.begin(), sson->children.back());
            sson->children.pop_back();
        }
        keys[idx - 1] = sson->keys.back();
        sson->keys.pop_back();
    }

    // borrow key from right
    void getFromNxt(int idx) {
        Node* son = children[idx];
        Node* sson = children[idx + 1];
        son->keys.push_back(keys[idx]);
        if (!son->is_leaf) {
            son->children.push_back(sson->children[0]);
            sson->children.erase(sson->children.begin());
        }
        keys[idx] = sson->keys.front();
        sson->keys.erase(sson->keys.begin());
    }

    // fill the child with at least t keys
    void fillChild(int idx) {
        if (idx > 0 && children[idx - 1]->keys.size() >= t)
            getFromPrev(idx);
        else if (idx != keys.size() && children[idx + 1]->keys.size() >= t)
            getFromNxt(idx);
        else {
            if (idx < keys.size())
                merge(idx);
            else
                merge(idx - 1);
        }
    }
};

struct BTree {
    Node* root;
    int t;

    BTree(int _t) : root(nullptr), t(_t) {}

    // traverse the tree
    void traverse() {
        if (root) root->traverse();
    }

    // insert val into the tree
    void insert(int val) {
        if (!root) {
            root = new Node(t, true);
            root->keys.push_back(val);
        } else {
            if (root->keys.size() < 2 * t - 1) {
                root->insert(val);
            } else {
                Node* newRoot = new Node(t, false);
                newRoot->children.push_back(root);
                newRoot->splitChild(0, root);
                int p = 0;
                if (newRoot->keys[0] < val) p++;
                newRoot->children[p]->insert(val);
                root = newRoot;
            }
        }
    }

    // remove val from the tree
    void remove(int val) {
        if (!root) return;
        root->remove(val);
        if (root->keys.size() == 0) {
            Node* oldRoot = root;
            if (!root->is_leaf)
                root = root->children[0];
            else
                root = nullptr;
            delete oldRoot;
        }
    }

    // return true if val is in the tree
    bool find(int val) {
        if (root == nullptr) return false;
        return root->search(val) != nullptr;
    }

    // find the biggest key equal or smaller than val
    int findBiggestSmaller(int val) {
        Node* ac = root;
        int ans = INT_MIN;

        while (ac != nullptr) {
            int i = 0;
            while (i < ac->keys.size() && ac->keys[i] <= val) {
                ans = max(ans, ac->keys[i]);
                i++;
            }
            if (ac->is_leaf) break;
            ac = ac->children[i];
        }

        return ans;
    }

    // find the smallest key equal or bigger than val
    int findSmallestBigger(int val) {
        Node* ac = root;
        int ans = INT_MAX;
        while (ac) {
            int i = 0;
            while (i < ac->keys.size() && ac->keys[i] < val) i++;
            if (i < ac->keys.size()) ans = min(ans, ac->keys[i]);
            if (ac->is_leaf) break;
            ac = ac->children[i];
        }
        return ans;
    }

    // compute the ans array
    void compRange(Node* ac, int x, int y, vector<int>& ans) {
        if (ac == nullptr) return;
        int i = 0;
        while (i < ac->keys.size() && ac->keys[i] < x) i++;

        if (!ac->is_leaf) {
            compRange(ac->children[i], x, y, ans);
        }
        while (i < ac->keys.size()) {
            if (ac->keys[i] >= x && ac->keys[i] <= y)
                ans.push_back(ac->keys[i]);
            if (ac->keys[i] > y) {
                break;
            }
            if (!ac->is_leaf) compRange(ac->children[i + 1], x, y, ans);
            i++;
        }
    }

    // print the keys in the range [x, y]
    void printRange(int x, int y) {
        vector<int> ans;
        compRange(root, x, y, ans);
        for (auto i : ans) fout << i << " ";
        fout << "\n";
    }

    // print the whole tree
    void print() {
        if (root != nullptr) root->traverse();
    }
};

int main() {
    int q;
    fin >> q;
    BTree btree(3);
    while (q--) {
        int op, x, y;
        fin >> op >> x;
        if (op == 1) {
            btree.insert(x);
        } else if (op == 2) {
            btree.remove(x);
        } else if (op == 3) {
            fout << (btree.find(x) ? 1 : 0) << "\n";
        } else if (op == 4) {
            fout << btree.findBiggestSmaller(x) << "\n";
        } else if (op == 5) {
            fout << btree.findSmallestBigger(x) << "\n";
        } else if (op == 6) {
            fin >> y;
            btree.printRange(x, y);
        }
    }
    fin.close();
    fout.close();

    return 0;
}
