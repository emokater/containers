#ifndef SET_H
#define SET_H

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <limits>
#include <utility>

#include "rbtree.h"
#include <vector>

namespace s21 {
template <typename Key>

struct SetKeyOfValue {
  const Key &operator()(const Key &k) const { return k; }
};

template <typename Key, typename Compare = std::less<Key>>
class set : private RBTree<Key, Key, SetKeyOfValue<Key>, Compare> {
 public:
  using BinaryKeyree = RBTree<Key, Key, SetKeyOfValue<Key>, Compare>;
  using Node = RBNode<Key>;

  using key_type = Key;
  using value_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = typename BinaryKeyree::template Iterator<Key *, reference>;
  using const_iterator =
      typename BinaryKeyree::template Iterator<const Key *, const_reference>;

  using size_type = size_t;

 public:
  set() = default;
  set(std::initializer_list<value_type> const &items);
  set(const set &s) = default;
  set(set &&s) = default;
  ~set() = default;
  set &operator=(set &&s) = default;

  iterator begin() {
    return this->GetRoot() ? iterator(this->FindMinimum(), this)
                           : iterator(nullptr);
  }
  iterator end() { return iterator(nullptr, this); }

  bool empty();
  size_type size() { return this->CntElements(); }
  size_type max_size();

  void clear();
  std::pair<iterator, bool> insert(const value_type &value);

  template <typename... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args &&...args);

  void erase(iterator pos) { this->DelNode(pos.node_); }
  void swap(set &other);
  void merge(set &other);

  iterator find(const key_type &key);
  bool contains(const key_type &key);
};
}  // namespace s21

#include "set.tpp"

#endif