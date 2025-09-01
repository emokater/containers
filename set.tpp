#include "set.h"

using namespace s21;

template <typename T, typename Compare>
set<T, Compare>::set(std::initializer_list<value_type> const &items) {
  for (const auto &elem : items) {
    this->Insert(elem);
  }
}

template <typename T, typename Compare>
bool set<T, Compare>::empty() {
  bool res = this->root_ == nullptr ? true : false;
  return res;
}

template <typename T, typename Compare>
typename set<T, Compare>::size_type set<T, Compare>::max_size() {
  using node_t = s21::RBNode<T>;
  return std::numeric_limits<size_type>::max() / sizeof(node_t);
}

template <typename T, typename Compare>
void set<T, Compare>::clear() {
  this->DelTree(this->root_);
  this->root_ = nullptr;
}

template <typename T, typename Compare>
std::pair<typename set<T, Compare>::iterator, bool> set<T, Compare>::insert(
    const value_type &value) {
  iterator res1;
  int flag = this->Insert(value);
  res1.node_ = this->Search(value);

  bool res2 = res1.node_ == nullptr || !flag ? false : true;

  std::pair<iterator, bool> res;
  res.first = res1;
  res.second = res2 && flag;

  return res;
}

template <typename T, typename Compare>
template <typename... Args>
std::vector<std::pair<typename set<T, Compare>::iterator, bool>>
set<T, Compare>::insert_many(Args &&...args) {
  std::vector<std::pair<iterator, bool>> results;

  for (auto value : std::initializer_list<value_type>{args...}) {
    results.push_back(insert(value));
  }

  return results;
}

template <typename T, typename Compare>
void set<T, Compare>::swap(set &other) {
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

template <typename T, typename Compare>
void set<T, Compare>::merge(set &other) {
  iterator it = other.begin();
  iterator next = it;
  while (it != other.end()) {
    next = it;
    ++next;
    if (insert(std::move(*it)).second) {
      other.erase(it);
    }
    it = next;
  }
}

template <typename T, typename Compare>
typename set<T, Compare>::iterator set<T, Compare>::find(const key_type &key) {
  iterator res;
  res.node_ = this->Search(key);
  return res;
}

template <typename T, typename Compare>
bool set<T, Compare>::contains(const key_type &key) {
  bool res = this->Search(key) == nullptr ? false : true;
  return res;
}