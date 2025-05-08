#include <fstream>
#include <iostream>
#include <stack>
#include <vector>

std::ifstream fin("abce.in");
std::ofstream fout("abce.out");

int N, Q, op;

std::vector<int> merge(std::vector<int> arr1, std::vector<int> arr2) {
    int n = arr1.size();
    int m = arr2.size();
    std::vector<int> result;

    int i = 0, j = 0;
    while (i < n && j < m) {
        if (arr1[i] > arr2[j]) {
            result.push_back(arr2[j]);
            j++;
        } else if (arr1[i] < arr2[j]) {
            result.push_back(arr1[i]);
            i++;
        } else {
            result.push_back(arr1[i]);
            i++;
            j++;
        }
    }

    while (i < n) {
        result.push_back(arr1[i]);
        i++;
    }

    while (j < m) {
        result.push_back(arr2[j]);
        j++;
    }

    return result;
}

struct Node {
    int value;
    Node* right;
    Node* left;
    int height;

    Node(int val) : value(val), right(nullptr), left(nullptr), height(1) {}
};

int getHeight(Node* node) { return node ? node->height : 0; }

int getBalance(Node* node) {
    if (!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

void print_inorder(Node* node) {
    if (!node) return;
    print_inorder(node->left);
    std::cout << node->value << ", ";
    print_inorder(node->right);
}

Node* rotateL(Node* node) {
    Node* node_right = node->right;
    Node* aux = node_right->left;

    node_right->left = node;
    node->right = aux;

    node->height = 1 + std::max(getHeight(node->right), getHeight(node->left));
    node_right->height =
        1 + std::max(getHeight(node_right->right), getHeight(node_right->left));

    return node_right;
}

Node* rotateR(Node* node) {
    Node* node_left = node->left;
    Node* aux = node_left->right;

    node_left->right = node;
    node->left = aux;

    node->height = 1 + std::max(getHeight(node->right), getHeight(node->left));
    node_left->height =
        1 + std::max(getHeight(node_left->right), getHeight(node_left->left));

    return node_left;
}

Node* insertNode(Node* node, int value) {
    if (!node) {
        return new Node(value);
    }

    if (value < node->value) {
        node->left = insertNode(node->left, value);
    } else if (value > node->value) {
        node->right = insertNode(node->right, value);
    } else {
        return node;
    }

    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

    int balance = getBalance(node);

    if (balance > 1) {
        if (node->left && value < node->left->value) {
            return rotateR(node);
        }
        if (node->left && value > node->left->value) {
            node->left = rotateL(node->left);
            return rotateR(node);
        }
    }

    if (balance < -1) {
        if (node->right && value > node->right->value) {
            return rotateL(node);
        }
        if (node->right && value < node->right->value) {
            node->right = rotateR(node->right);
            return rotateL(node);
        }
    }

    return node;
}

int find(Node* root, int value) {
    Node* currentNode = root;

    while (currentNode != nullptr) {
        if (value == currentNode->value)
            return 1;
        else if (value < currentNode->value) {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }
    }

    return 0;
}

void query_6(Node* root, int minim, int maxim) {
    if (!root) return;

    if (minim < root->value) {
        query_6(root->left, minim, maxim);
    }

    if (minim <= root->value && maxim >= root->value) {
        fout << root->value << ' ';
    }

    if (maxim > root->value) {
        query_6(root->right, minim, maxim);
    }
}

int query_5(Node* root, int x_val) {
    int current_best = 2000000000; 
    Node* current_node = root;

    while (current_node != nullptr) {
        if (current_node->value >= x_val) {
            if (current_node->value < current_best) {
                current_best = current_node->value;
            }
            current_node = current_node->left; 
        } else {
            current_node = current_node->right; 
        }
    }
    return current_best;
}

int query_4(Node* root, int x_val) {
    int current_best = -2000000000;
    Node* current_node = root;

    while (current_node != nullptr) {
        if (current_node->value <= x_val) {
            if (current_node->value > current_best) {
                current_best = current_node->value;
            }
            current_node = current_node->right;
        } else {
            current_node = current_node->left; 
        }
    }
    return current_best;
}

Node* minFromTree(Node* root) {
    while (root->left != nullptr) {
        root = root->left;
    }
    return root;
}

Node* maxFromTree(Node* root) {
    while (root->right != nullptr) {
        root = root->right;
    }
    return root;
}

Node* deleteNode(Node* root, int value) {
    if (!root) return root;

    if (value < root->value) {
        root->left = deleteNode(root->left, value);
    } else if (value > root->value) {
        root->right = deleteNode(root->right, value);
    } else {
        if (!root->left || !root->right) {
            Node* aux = root->left ? root->left : root->right;
            if (!aux) {
                aux = root;
                root = nullptr;
            } else {
                *root = *aux;
            }
            delete aux;
        } else {
            Node* aux = maxFromTree(root->left);
            root->value = aux->value;
            root->left = deleteNode(root->left, aux->value);
        }
    }

    if (!root) return root;

    root->height = 1 + std::max(getHeight(root->left), getHeight(root->right));

    int balance = getBalance(root);

    if (balance > 1) {
        if (getBalance(root->left) >= 0) { 
            return rotateR(root);
        } else { 
            root->left = rotateL(root->left);
            return rotateR(root);
        }
    }

    if (balance < -1) {
        if (getBalance(root->right) <= 0) {
            return rotateL(root);
        } else { 
            root->right = rotateR(root->right);
            return rotateL(root);
        }
    }

    return root;
}

void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

Node* sorted_toAVL(std::vector<int> arr, int start, int end) {
    if (start > end) return nullptr;

    int mid = (start + end) / 2;
    Node* node = new Node(arr[mid]);

    node->left = sorted_toAVL(arr, start, mid - 1);
    node->right = sorted_toAVL(arr, mid + 1, end);

    node->height = 1 + std::max(getHeight(node->right), getHeight(node->left));

    return node;
}

Node* mergeAVL(Node* root1, Node* root2) {
    std::vector<int> vec1;
    std::vector<int> vec2;
    std::stack<Node*> s;

    Node* currentNode = root1;
    while (currentNode != nullptr || !s.empty()) {
        while (currentNode != nullptr) {
            s.push(currentNode);
            currentNode = currentNode->left;
        }

        currentNode = s.top();
        vec1.push_back(currentNode->value);
        s.pop();

        currentNode = currentNode->right;
    }

    currentNode = root2;
    while (currentNode != nullptr || !s.empty()) {
        while (currentNode != nullptr) {
            s.push(currentNode);
            currentNode = currentNode->left;
        }

        currentNode = s.top();
        vec2.push_back(currentNode->value);
        s.pop();

        currentNode = currentNode->right;
    }

    std::vector<int> sorted = merge(vec1, vec2);

    return sorted_toAVL(sorted, 0, sorted.size() - 1);
}

int main() {
    Node* root = nullptr;
    fin >> Q;
    int x;
    for (int i = 0; i < Q; i++) {
        fin >> op;
        fin >> x;
        switch (op) {
            case 1: {
                root = insertNode(root, x);
                break;
            }
            case 2: {
                root = deleteNode(root, x);
                break;
            }
            case 3: {
                fout << find(root, x) << '\n';
                break;
            }
            case 4: {
                fout << query_4(root, x) << '\n';
                break;
            }
            case 5: {
                fout << query_5(root, x) << '\n';
                break;
            }
            case 6: {
                int y;
                fin >> y;
                query_6(root, x, y);
                fout << '\n';
                break;
            }
        }
    }

    fin.close();
    fout.close();
    deleteTree(root);

    return 0;
}
