module;

#include <cstddef>
#include <print>
#include <utility>
#include <vector>

export module bplus_tree;

namespace bplus_tree {
  export struct Node {
    struct Elem {
      int key;
      Node* child;
    };
    std::vector<Elem> elems;
  };

  export void insert(Node* root, int key) {

    bool found = 0;
    for(int i = 0; i < root->elems.size(); ++i) {
      if(key < root->elems[i].key) {
        found = true; 
        root->elems.insert(root->elems.begin() + i, {key, nullptr});
        break;
      }
    }
    if(!found) {
      /* If we get to the end, then we check if we have exceeded max size.
       * If we have have, then make the middle left element a child.    
       * */
      root->elems.push_back({key, nullptr});
    }

  }
}



