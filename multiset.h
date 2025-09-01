#ifndef MULTISET_H
#define MULTISET_H

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <utility>

#include <vector>
#include "rbtree.h"

namespace s21 {
template <typename Key>
struct MultisetKeyOfValue {
  const Key &operator()(const Key &k) const { return k; }
};

template <typename Key, typename Compare = std::less<Key>>
class multiset : private RBTree<Key, Key, MultisetKeyOfValue<Key>, Compare> {
 public:
  using BinaryTree = RBTree<Key, Key, MultisetKeyOfValue<Key>, Compare>;
  using Node = RBNode<Key>;

  using key_type = Key;
  using value_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator =
      typename BinaryTree::template Iterator<value_type *, reference>;
  using const_iterator =
      typename BinaryTree::template Iterator<const value_type *,
                                             const_reference>;
  using size_type = size_t;

  multiset() = default;
  multiset(std::initializer_list<value_type> const &items);
  multiset(const multiset &ms) = default;
  multiset(multiset &&ms) = default;
  ~multiset() = default;
  multiset &operator=(multiset &&ms) = default;

  iterator begin() {
    return this->GetRoot() ? iterator(this->FindMinimum(), this)
                           : iterator(nullptr);
  }
  iterator end() { return iterator(nullptr, this); }

  bool empty();
  size_type size() { return this->CntElements(); }
  size_type max_size();

  void clear();
  iterator insert(const value_type &value);
  template <typename... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args &&...args);
  void erase(iterator pos) { this->DelNode(pos.node_); }
  void swap(multiset &other);
  void merge(multiset &other);

  size_type count(const key_type &key);
  iterator find(const key_type &key);
  bool contains(const key_type &key) const {
    return this->Search(key) != nullptr;
  }
  std::pair<iterator, iterator> equal_range(const key_type &key) {
    return std::make_pair(lower_bound(key), upper_bound(key));
  }
  iterator lower_bound(const key_type &key);
  iterator upper_bound(const key_type &key);
};
}  // namespace s21

#include "multiset.tpp"

#endif