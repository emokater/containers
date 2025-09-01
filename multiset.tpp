#include <limits>

#include "multiset.h"

template <typename Key, typename Compare>
s21::multiset<Key, Compare>::multiset(
    std::initializer_list<value_type> const &items) {
  for (const auto &elem : items) {
    insert(elem);
  }
}

template <typename Key, typename Compare>
bool s21::multiset<Key, Compare>::empty() {
  bool res = this->GetRoot() == nullptr ? true : false;
  return res;
}

template <typename Key, typename Compare>
typename s21::multiset<Key, Compare>::size_type
s21::multiset<Key, Compare>::max_size() {
  using node_t = s21::RBNode<Key>;
  return std::numeric_limits<size_type>::max() / sizeof(node_t);
}

template <typename Key, typename Compare>
void s21::multiset<Key, Compare>::clear() {
  this->DelTree(this->root_);
  this->root_ = nullptr;
}

template <typename Key, typename Compare>
typename s21::multiset<Key, Compare>::iterator
s21::multiset<Key, Compare>::insert(const value_type &value) {
  iterator res;

  if (!(this->root_)) {
    this->root_ = new Node(value);
    this->root_->color_ = BLACK;
    res.node_ = this->root_;
  } else {
    Node *target = this->root_;
    Node *parent = nullptr;
    while (target) {
      parent = target;
      if (this->key_less(value, target->data_)) {
        target = target->left_;
      } else {
        target = target->right_;
      }
    }

    target = new Node(value);
    target->parent_ = parent;

    if (this->key_less(value, parent->data_)) {
      parent->left_ = target;
    } else {
      parent->right_ = target;
    }

    this->Balance(target);

    res.node_ = target;
  }

  return res;
}

template <typename Key, typename Compare>
template <typename... Args>
std::vector<std::pair<typename s21::multiset<Key, Compare>::iterator, bool>>
s21::multiset<Key, Compare>::insert_many(Args &&...args) {
  std::vector<std::pair<iterator, bool>> results;

  for (auto value : std::initializer_list<value_type>{args...}) {
    iterator it = insert(value);
    results.push_back({it, true});
  }

  return results;
}

template <typename Key, typename Compare>
void s21::multiset<Key, Compare>::swap(multiset &other) {
  auto tmp_root = this->root_;
  this->root_ = other.root_;
  other.root_ = tmp_root;

  auto tmp_comp = this->comp;
  this->comp = other.comp;
  other.comp = tmp_comp;

  auto tmp_key = this->key_of_value;
  this->key_of_value = other.key_of_value;
  other.key_of_value = tmp_key;
}

template <typename Key, typename Compare>
void s21::multiset<Key, Compare>::merge(multiset &other) {
  if (this != &other) {
    while (!other.empty()) {
      insert(*other.begin());
      other.erase(other.begin());
    }
  }
}

template <typename Key, typename Compare>
typename s21::multiset<Key, Compare>::size_type
s21::multiset<Key, Compare>::count(const key_type &key) {
  iterator first = lower_bound(key);
  iterator last = upper_bound(key);
  size_type n = 0;
  for (iterator it = first; it != last; ++it) ++n;
  return n;
}

template <typename Key, typename Compare>
typename s21::multiset<Key, Compare>::iterator
s21::multiset<Key, Compare>::find(const key_type &key) {
  iterator res = lower_bound(key);

  if (res.node_ && this->comp(key, *res)) {
    res = end();
  }

  return res;
}

// first element not less than the given
template <typename Key, typename Compare>
typename s21::multiset<Key, Compare>::iterator
s21::multiset<Key, Compare>::lower_bound(const key_type &key) {
  Node *tmp = this->root_;
  Node *cand = nullptr;

  while (tmp) {
    if (this->comp(tmp->data_, key)) {
      tmp = tmp->right_;
    } else {
      cand = tmp;
      tmp = tmp->left_;
    }
  }

  return iterator(cand);
}

template <typename Key, typename Compare>
typename s21::multiset<Key, Compare>::iterator
s21::multiset<Key, Compare>::upper_bound(const key_type &key) {
  Node *tmp = this->root_;
  Node *cand = nullptr;

  while (tmp) {
    if (this->comp(key, tmp->data_)) {
      cand = tmp;
      tmp = tmp->left_;
    } else {
      tmp = tmp->right_;
    }
  }

  return iterator(cand);
}