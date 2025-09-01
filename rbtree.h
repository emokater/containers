#ifndef RB_TREE_H
#define RB_TREE_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace s21 {
typedef enum { RED, BLACK } Color;

template <typename DataType>
class RBNode {
 public:
  DataType data_;
  Color color_;

  RBNode *left_, *right_, *parent_;

 public:
  RBNode() {
    data_ = DataType();
    color_ = RED;
    left_ = nullptr;
    right_ = nullptr;
    parent_ = nullptr;
  }

  RBNode(const DataType data)
      : data_(data),
        color_(RED),
        left_(nullptr),
        right_(nullptr),
        parent_(nullptr) {}
};

template <typename DataType, typename Key, typename KeyOfValue,
          typename Compare = std::less<Key>>
class RBTree {
 public:
  template <typename Pointer, typename Reference>
  class Iterator {
   public:
    RBNode<DataType> *node_ = nullptr;
    const RBTree *owner_ = nullptr;

    Iterator() = default;
    Iterator(RBNode<DataType> *node) : node_(node), owner_(nullptr) {}
    Iterator(RBNode<DataType> *node, RBTree *owner)
        : node_(node), owner_(owner) {}

    Reference operator*() const { return node_->data_; }
    Pointer operator->() const { return &node_->data_; }

    Iterator &operator++() & {
      if (node_ && node_->right_) {
        node_ = node_->right_;
        while (node_->left_) {
          node_ = node_->left_;
        }
      } else if (node_) {
        RBNode<DataType> *parent = node_->parent_;
        while (parent && node_ == parent->right_) {
          node_ = parent;
          parent = parent->parent_;
        }
        node_ = parent;
      }

      return *this;
    }

    Iterator &operator--() & {
      if (node_) {
        if (node_->left_) {
          node_ = node_->left_;
          while (node_->right_) {
            node_ = node_->right_;
          }
        } else {
          RBNode<DataType> *p = node_->parent_;
          while (p && node_ == p->left_) {
            node_ = p;
            p = p->parent_;
          }
          node_ = p;
        }
      } else if (owner_) {
        node_ = owner_->GetRoot();
        if (node_) {
          while (node_->right_) {
            node_ = node_->right_;
          }
        }
      } else {
        node_ = nullptr;
      }

      return *this;
    }

    bool operator==(const Iterator &other) const {
      return node_ == other.node_;
    }
    bool operator!=(const Iterator &other) const {
      return node_ != other.node_;
    }
  };

 public:
  KeyOfValue key_of_value;
  Compare comp;

  bool key_less(const DataType &a, const DataType &b) const {
    return comp(key_of_value(a), key_of_value(b));
  }

  RBTree() { root_ = nullptr; }
  RBTree(const DataType data) {
    root_ = new RBNode<DataType>(data);
    root_->color_ = BLACK;
  }
  RBTree(const RBTree &other) {
    root_ = CloneSubtree(other.root_, nullptr);
    comp = other.comp;
  }
  RBTree(RBTree &&other) {
    root_ = other.root_;
    comp = other.comp;
    other.root_ = nullptr;
  }
  ~RBTree() {
    DelTree(root_);
    this->root_ = nullptr;
  }

  RBTree &operator=(const RBTree &other) {
    if (this != &other) {
      DelTree(root_);
      root_ = CloneSubtree(other.root_, nullptr);
      comp = other.comp;
    }
    return *this;
  }

  RBTree &operator=(RBTree &&other) {
    if (this != &other) {
      DelTree(root_);
      root_ = other.root_;
      comp = std::move(other.comp);
      other.root_ = nullptr;
    }
    return *this;
  }

  int Insert(const DataType &data) {
    int flag = 1;
    if (!root_) {
      root_ = new RBNode<DataType>(data);
      root_->color_ = BLACK;
    } else {
      RBNode<DataType> *target = root_;
      RBNode<DataType> *parent = nullptr;
      while (target && flag) {
        parent = target;
        if (key_less(data, target->data_)) {
          target = target->left_;
        } else if (key_less(target->data_, data)) {
          target = target->right_;
        } else {
          flag = 0;
        }
      }

      if (flag) {
        target = new RBNode<DataType>(data);
        target->parent_ = parent;

        if (key_less(data, parent->data_)) {
          parent->left_ = target;
        } else {
          parent->right_ = target;
        }

        Balance(target);
      }
    }

    return flag;
  }

  RBNode<DataType> *Search(const Key &key) const {
    int flag = 1;
    RBNode<DataType> *cur = root_;

    if (root_) {
      while (cur && flag) {
        if (!(comp(key_of_value(cur->data_), key)) &&
            !(comp(key, key_of_value(cur->data_)))) {
          flag = 0;
        } else if (comp(key, key_of_value(cur->data_))) {
          cur = cur->left_;
        } else {
          cur = cur->right_;
        }
      }
    }

    RBNode<DataType> *res = flag == 0 ? cur : nullptr;

    return res;
  }

  RBNode<DataType> *FindMinimum() const {
    return root_ ? SupportFindMinimum(root_) : nullptr;
  }

  RBNode<DataType> *GetRoot() const { return root_; }

  size_t CntElements() {
    size_t cnt = 0;
    CntElementsSupport(root_, &cnt);
    return cnt;
  }

  void DelTree(RBNode<DataType> *root) {
    if (root) {
      DelTree(root->left_);
      DelTree(root->right_);
      delete root;
    }
  }

  void DelNode(RBNode<DataType> *n) {
    if (!n) {
      return;
    }
    int ch = CntChild(n);

    if (n->color_ == RED && ch == 0) {
      ChangeConnections(n, nullptr);
      delete n;
    } else if (ch == 2) {
      RBNode<DataType> *el_for_swap = SupportFindMinimum(n->right_);
      Color his_clr = el_for_swap->color_;
      RBNode<DataType> *his_chld = el_for_swap->right_;
      RBNode<DataType> *parent_his_ch = nullptr;

      if (el_for_swap->parent_ == n) {
        parent_his_ch = el_for_swap;
      } else {
        ChangeConnections(el_for_swap, el_for_swap->right_);
        parent_his_ch = el_for_swap->parent_;
        el_for_swap->right_ = n->right_;
        n->right_->parent_ = el_for_swap;
      }

      ChangeConnections(n, el_for_swap);
      el_for_swap->left_ = n->left_;
      if (el_for_swap->left_) {
        el_for_swap->left_->parent_ = el_for_swap;
      }
      el_for_swap->color_ = n->color_;

      delete n;

      if (his_clr == BLACK) {
        DeleteFixup(his_chld, parent_his_ch);
      }
    } else if (n->color_ == BLACK && ch == 0) {
      RBNode<DataType> *p = n->parent_;
      ChangeConnections(n, nullptr);
      delete n;
      DeleteFixup(nullptr, p);
    } else if (n->color_ == BLACK && ch == 1) {
      RBNode<DataType> *child = n->left_ ? n->left_ : n->right_;
      ChangeConnections(n, child);
      child->color_ = BLACK;
      delete n;
    }

    return;
  }

 protected:
  RBNode<DataType> *root_;

  Color ColorOf(RBNode<DataType> *x) { return x ? x->color_ : BLACK; }
  void SetColor(RBNode<DataType> *x, Color clr) {
    if (x) x->color_ = clr;
  }
  void DeleteFixup(RBNode<DataType> *x, RBNode<DataType> *parent) {
    while (x != root_ && ColorOf(x) == BLACK) {
      if (!parent) break;

      if (x == parent->left_) {
        RBNode<DataType> *w = parent ? parent->right_ : nullptr;

        if (ColorOf(w) == RED) {
          SetColor(w, BLACK);
          SetColor(parent, RED);
          LeftRotate(w, parent, parent ? parent->parent_ : nullptr);
          w = parent ? parent->right_ : nullptr;
        }

        if (ColorOf(w ? w->left_ : nullptr) == BLACK &&
            ColorOf(w ? w->right_ : nullptr) == BLACK) {
          SetColor(w, RED);
          x = parent;
          parent = x ? x->parent_ : nullptr;
        } else {
          if (ColorOf(w ? w->right_ : nullptr) == BLACK) {
            SetColor(w ? w->left_ : nullptr, BLACK);
            SetColor(w, RED);
            if (w && w->left_) RightRotate(w->left_, w, parent);
            w = parent ? parent->right_ : nullptr;
          }
          if (w) SetColor(w, ColorOf(parent));
          SetColor(parent, BLACK);
          SetColor(w ? w->right_ : nullptr, BLACK);
          LeftRotate(w, parent, parent ? parent->parent_ : nullptr);
          x = root_;
          break;
        }
      } else {
        RBNode<DataType> *w = parent ? parent->left_ : nullptr;

        if (ColorOf(w) == RED) {
          SetColor(w, BLACK);
          SetColor(parent, RED);
          RightRotate(w, parent, parent ? parent->parent_ : nullptr);
          w = parent ? parent->left_ : nullptr;
        }
        if (ColorOf(w ? w->right_ : nullptr) == BLACK &&
            ColorOf(w ? w->left_ : nullptr) == BLACK) {
          SetColor(w, RED);
          x = parent;
          parent = x ? x->parent_ : nullptr;
        } else {
          if (ColorOf(w ? w->left_ : nullptr) == BLACK) {
            SetColor(w ? w->right_ : nullptr, BLACK);
            SetColor(w, RED);
            if (w && w->right_) LeftRotate(w->right_, w, parent);
            w = parent ? parent->left_ : nullptr;
          }
          if (w) SetColor(w, ColorOf(parent));
          SetColor(parent, BLACK);
          SetColor(w ? w->left_ : nullptr, BLACK);
          RightRotate(w, parent, parent ? parent->parent_ : nullptr);
          x = root_;
          break;
        }
      }
    }
    SetColor(x, BLACK);
    if (root_) root_->color_ = BLACK;
  }

  void ChangeConnections(RBNode<DataType> *being_deleted,
                         RBNode<DataType> *new_el) {
    RBNode<DataType> *p = being_deleted->parent_;
    if (!p) {
      root_ = new_el;
    } else if (p->left_ == being_deleted) {
      p->left_ = new_el;
    } else {
      p->right_ = new_el;
    }

    if (new_el) {
      new_el->parent_ = p;
    }
  }

  int CntChild(RBNode<DataType> *node) {
    int res = 0;
    if (node->left_) {
      res++;
    }
    if (node->right_) {
      res++;
    }
    return res;
  }

  void CntElementsSupport(RBNode<DataType> *node, size_t *cnt) {
    if (node) {
      (*cnt)++;
      CntElementsSupport(node->left_, cnt);
      CntElementsSupport(node->right_, cnt);
    }
  }

  RBNode<DataType> *CloneSubtree(RBNode<DataType> *node,
                                 RBNode<DataType> *parent) {
    RBNode<DataType> *copy = nullptr;
    if (node) {
      copy = new RBNode<DataType>(node->data_);
      copy->color_ = node->color_;
      copy->parent_ = parent;
      copy->left_ = CloneSubtree(node->left_, copy);
      copy->right_ = CloneSubtree(node->right_, copy);
    }

    return copy;
  }

  RBNode<DataType> *SupportFindMinimum(RBNode<DataType> *root) const {
    RBNode<DataType> *cur = root;
    while (cur->left_) {
      cur = cur->left_;
    }
    return cur;
  }

  void Balance(RBNode<DataType> *node) {
    RBNode<DataType> *dad = node->parent_;

    while (dad && dad->color_ == RED) {
      RBNode<DataType> *grand = dad->parent_;
      RBNode<DataType> *uncle;

      if (!grand) {
        return;
      } else if (dad == grand->left_) {
        uncle = grand->right_;

        if (uncle && uncle->color_ == RED) {
          dad->color_ = BLACK;
          uncle->color_ = BLACK;
          grand->color_ = RED;

          node = grand;
          dad = node->parent_;
          if (dad) {
            grand = dad->parent_;
          }

          continue;
        } else {
          if (dad->right_ == node) {
            LeftRotate(node, dad, grand);
            node = dad;
            dad = node->parent_;
          }
          dad->color_ = BLACK;
          grand->color_ = RED;
          RightRotate(dad, grand, grand->parent_);
          break;
        }
      } else {
        uncle = grand->left_;

        if (uncle && uncle->color_ == RED) {
          dad->color_ = BLACK;
          uncle->color_ = BLACK;
          grand->color_ = RED;

          node = grand;
          dad = node->parent_;
          if (dad) {
            grand = dad->parent_;
          }

          continue;
        } else {
          if (dad->left_ == node) {
            RightRotate(node, dad, grand);
            node = dad;
            dad = node->parent_;
          }
          dad->color_ = BLACK;
          grand->color_ = RED;
          LeftRotate(dad, grand, grand->parent_);
          break;
        }
      }
    }

    root_->color_ = BLACK;
  }

  void LeftRotate(RBNode<DataType> *child, RBNode<DataType> *dad,
                  RBNode<DataType> *grand) {
    RBNode<DataType> *grandson = child->left_;

    dad->right_ = grandson;
    if (grandson) {
      grandson->parent_ = dad;
    }

    child->left_ = dad;
    dad->parent_ = child;

    child->parent_ = grand;
    if (!grand) {
      root_ = child;
    } else if (grand->left_ == dad) {
      grand->left_ = child;
    } else {
      grand->right_ = child;
    }
  }

  void RightRotate(RBNode<DataType> *child, RBNode<DataType> *dad,
                   RBNode<DataType> *grand) {
    RBNode<DataType> *grandson = child->right_;

    dad->left_ = grandson;
    if (grandson) {
      grandson->parent_ = dad;
    }

    dad->parent_ = child;
    child->right_ = dad;

    child->parent_ = grand;
    if (!grand) {
      root_ = child;
    } else if (grand->right_ == dad) {
      grand->right_ = child;
    } else {
      grand->left_ = child;
    }
  }
};
}  // namespace s21

#endif