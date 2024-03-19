module;

#include <print>
#include <ranges>
#include <vector>

export module tree;

export namespace tree {

  struct Node {
    Node(size_t key) : m_key(key) {}
    size_t m_key;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;
    std::vector<std::string> m_data;
  };

  void print(Node* root, int depth = 0) {
    if(root->left != nullptr) {
      depth++;
      print(root->left, depth);
      depth--;
    }


    for([[maybe_unused]] auto d: std::views::iota(0, depth * 2)) {
      std::print(" ");
    }

    std::println("{}", root->m_key);
    if(root->right != nullptr) {
      depth++;
      print(root->right, depth);
      depth--;
    }
  }

  Node* search(Node* root, size_t key) {
    if(key < root->m_key) {
      return search(root->left, key);
    } else if (key > root->m_key) {
      return search(root->right, key);
    } else {
      return root;
    }
    return nullptr;
  }

  Node* insert(Node* root, size_t key, std::string data = "") {

    while(root) {
      if(key < root->m_key) {
        if(!root->left) {
          root->left = new Node(key);
          root->left->parent = root;
          return root->left;
        } else {
          root = root->left;
          continue;
        }
      } else if (key > root->m_key) {
        if(!root->right) {
          root->right = new Node(key);
          root->right->parent = root;
          return root->right;
        } else {
          root = root->right;
          continue;
        }
      } else {
        return root;
      }
    }
    std::unreachable();
  }
}

