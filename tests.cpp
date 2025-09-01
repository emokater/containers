#include <gtest/gtest.h>
#include "multiset.h"
#include "set.h"

TEST(Multiset, Member_functions) {
  using s21::multiset;

  multiset<int> a;
  EXPECT_TRUE(a.empty());
  EXPECT_EQ(a.size(), 0u);
  EXPECT_EQ(a.begin(), a.end());

  multiset<int> b{3, 1, 2, 2, 5};
  EXPECT_FALSE(b.empty());
  EXPECT_EQ(b.size(), 5u);
  EXPECT_EQ(b.count(2), 2u);

  multiset<int> f{};
  EXPECT_TRUE(f.empty());
  EXPECT_EQ(f.size(), 0u);

  multiset<int> c(b);
  EXPECT_EQ(c.size(), b.size());
  EXPECT_EQ(c.count(2), b.count(2));
  b.insert(4);
  EXPECT_EQ(b.size(), 6u);
  EXPECT_EQ(c.size(), 5u);

  multiset<int> d(std::move(b));
  EXPECT_EQ(d.size(), 6u);
  EXPECT_TRUE(b.empty());

  multiset<int> e;
  e = std::move(d);
  EXPECT_EQ(e.size(), 6u);
  EXPECT_TRUE(d.empty());
}

TEST(Multiset, Iterators) {
  using s21::multiset;

  multiset<int> empty;
  EXPECT_EQ(empty.begin(), empty.end());

  multiset<int> ms{3, 1, 2, 2, 5};
  auto it1 = ms.begin();
  ASSERT_NE(it1, ms.end());
  EXPECT_EQ(*it1, 1);
  ++it1;
  EXPECT_EQ(*it1, 2);
  ++it1;
  EXPECT_EQ(*it1, 2);
  ++it1;
  EXPECT_EQ(*it1, 3);
  ++it1;
  EXPECT_EQ(*it1, 5);
  ++it1;
  EXPECT_EQ(it1, ms.end());

  auto it2 = ms.end();
  --it2;
  EXPECT_EQ(*it2, 5);
  --it2;
  EXPECT_EQ(*it2, 3);
  --it2;
  EXPECT_EQ(*it2, 2);
  --it2;
  EXPECT_EQ(*it2, 2);
  --it2;
  EXPECT_EQ(*it2, 1);
  EXPECT_EQ(it2, ms.begin());

  multiset<int> one{42};
  auto i = one.begin();
  EXPECT_EQ(*i, 42);
  ++i;
  EXPECT_EQ(i, one.end());
  auto j = one.end();
  --j;
  EXPECT_EQ(*j, 42);
}

TEST(Multiset, Capacity) {
  using s21::multiset;

  multiset<int> a;
  EXPECT_TRUE(a.empty());
  EXPECT_EQ(a.size(), 0u);

  multiset<int> b{3, 1, 2, 2, 5};
  EXPECT_FALSE(b.empty());
  EXPECT_EQ(b.size(), 5u);

  auto mx = b.max_size();
  EXPECT_GE(mx, b.size());
  EXPECT_GT(mx, static_cast<multiset<int>::size_type>(1000));

  b.clear();
  EXPECT_TRUE(b.empty());
  EXPECT_EQ(b.size(), 0u);
  EXPECT_EQ(b.max_size(), mx);

  b.insert(7);
  b.insert(7);
  EXPECT_FALSE(b.empty());
  EXPECT_EQ(b.size(), 2u);

  multiset<int> c(b);
  EXPECT_EQ(c.size(), 2u);
  EXPECT_EQ(c.max_size(), b.max_size());

  multiset<int> d(std::move(b));
  EXPECT_EQ(d.size(), 2u);
  EXPECT_TRUE(b.empty());
  EXPECT_EQ(d.max_size(), c.max_size());

  multiset<int> e;
  e = std::move(d);
  EXPECT_EQ(e.size(), 2u);
  EXPECT_TRUE(d.empty());
  EXPECT_EQ(e.max_size(), c.max_size());
}

TEST(Multiset, Modifiers) {
  using s21::multiset;

  multiset<int> ms;
  auto it3 = ms.insert(3);
  EXPECT_EQ(*it3, 3);
  auto it1 = ms.insert(1);
  EXPECT_EQ(*it1, 1);
  ms.insert(2);
  ms.insert(2);
  EXPECT_EQ(ms.size(), 4u);

  {
    auto it = ms.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, ms.end());
  }

  {
    auto results = ms.insert_many(5, 5, 1);
    ASSERT_EQ(results.size(), 3u);
    for (auto &pr : results) {
      EXPECT_TRUE(pr.second);
    }
    EXPECT_EQ(ms.count(5), 2u);
    EXPECT_EQ(ms.count(1), 2u);
    EXPECT_EQ(ms.size(), 7u);
  }

  {
    auto first2 = ms.lower_bound(2);
    EXPECT_EQ(*first2, 2);
    ms.erase(first2);
    EXPECT_EQ(ms.count(2), 1u);
    EXPECT_EQ(ms.size(), 6u);
  }

  {
    ms.clear();
    EXPECT_TRUE(ms.empty());
    EXPECT_EQ(ms.size(), 0u);
    EXPECT_EQ(ms.begin(), ms.end());
  }

  multiset<int> a{1, 1, 4};
  multiset<int> b{2, 3};
  a.swap(b);
  EXPECT_EQ(a.size(), 2u);
  EXPECT_EQ(b.size(), 3u);
  EXPECT_EQ(a.count(2), 1u);
  EXPECT_EQ(a.count(3), 1u);
  EXPECT_EQ(b.count(1), 2u);
  EXPECT_EQ(b.count(4), 1u);

  {
    multiset<int> x{1, 2, 2};
    multiset<int> y{2, 3};
    x.merge(y);
    EXPECT_TRUE(y.empty());
    EXPECT_EQ(x.size(), 5u);
    EXPECT_EQ(x.count(1), 1u);
    EXPECT_EQ(x.count(2), 3u);
    EXPECT_EQ(x.count(3), 1u);

    auto it = x.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, x.end());
  }

  {
    multiset<int> dst;
    multiset<int> src{4, 1, 4, 2};
    dst.merge(src);
    EXPECT_TRUE(src.empty());
    EXPECT_EQ(dst.size(), 4u);
    EXPECT_EQ(dst.count(1), 1u);
    EXPECT_EQ(dst.count(2), 1u);
    EXPECT_EQ(dst.count(4), 2u);
  }

  {
    multiset<int> dst{5, 6};
    multiset<int> src;
    dst.merge(src);
    EXPECT_TRUE(src.empty());
    EXPECT_EQ(dst.size(), 2u);
    EXPECT_EQ(dst.count(5), 1u);
    EXPECT_EQ(dst.count(6), 1u);
  }

  {
    multiset<int> self{1, 2, 2, 3};
    multiset<int>::size_type size_before = self.size();
    multiset<int>::size_type cnt2_before = self.count(2);
    self.merge(self);
    EXPECT_EQ(self.size(), size_before);
    EXPECT_EQ(self.count(2), cnt2_before);
  }
}

TEST(Multiset, Lookup) {
  using s21::multiset;

  {
    multiset<int> ms;
    EXPECT_EQ(ms.count(10), 0u);
    EXPECT_FALSE(ms.contains(10));
    EXPECT_EQ(ms.find(10), ms.end());

    EXPECT_EQ(ms.lower_bound(10), ms.end());
    EXPECT_EQ(ms.upper_bound(10), ms.end());

    auto r = ms.equal_range(10);
    EXPECT_EQ(r.first, ms.end());
    EXPECT_EQ(r.second, ms.end());
  }

  multiset<int> ms{3, 1, 2, 2, 5};

  {
    EXPECT_TRUE(ms.contains(2));

    auto itf = ms.find(2);
    EXPECT_EQ(*itf, 2);

    auto lb = ms.lower_bound(2);
    auto ub = ms.upper_bound(2);
    ASSERT_NE(lb, ms.end());
    EXPECT_EQ(*lb, 2);
    ASSERT_NE(ub, ms.end());
    EXPECT_EQ(*ub, 3);

    auto eq = ms.equal_range(2);
    EXPECT_EQ(*eq.first, 2);
    EXPECT_EQ(*eq.second, 3);

    multiset<int>::size_type cnt = 0;
    for (auto it = eq.first; it != eq.second; ++it) {
      ++cnt;
    }
    EXPECT_EQ(cnt, 2u);
  }

  {
    auto lb = ms.lower_bound(5);
    auto ub = ms.upper_bound(5);
    EXPECT_EQ(*lb, 5);
    EXPECT_EQ(ub, ms.end());
  }

  {
    EXPECT_FALSE(ms.contains(4));
    EXPECT_EQ(ms.count(4), 0u);
    EXPECT_EQ(ms.find(4), ms.end());

    auto lb = ms.lower_bound(4);
    auto ub = ms.upper_bound(4);
    EXPECT_EQ(*lb, 5);
    EXPECT_EQ(*ub, 5);

    auto eq = ms.equal_range(4);
    EXPECT_EQ(*eq.first, 5);
    EXPECT_EQ(*eq.second, 5);
  }

  {
    EXPECT_FALSE(ms.contains(0));
    EXPECT_EQ(ms.count(0), 0u);
    EXPECT_EQ(ms.find(0), ms.end());

    auto lb = ms.lower_bound(0);
    auto ub = ms.upper_bound(0);

    EXPECT_EQ(*lb, 1);
    EXPECT_EQ(*ub, 1);
  }

  {
    EXPECT_FALSE(ms.contains(6));
    EXPECT_EQ(ms.count(6), 0u);

    EXPECT_EQ(ms.find(6), ms.end());
    EXPECT_EQ(ms.lower_bound(6), ms.end());
    EXPECT_EQ(ms.upper_bound(6), ms.end());

    auto eq = ms.equal_range(6);
    EXPECT_EQ(eq.first, ms.end());
    EXPECT_EQ(eq.second, ms.end());
  }

  {
    multiset<int> dup{2, 2, 2};

    auto lb = dup.lower_bound(2);
    auto ub = dup.upper_bound(2);
    EXPECT_EQ(*lb, 2);
    EXPECT_EQ(ub, dup.end());

    auto eq = dup.equal_range(2);
    EXPECT_EQ(eq.second, dup.end());

    multiset<int>::size_type cnt = 0u;
    for (auto it = eq.first; it != eq.second; ++it) {
      ++cnt;
    }
    EXPECT_EQ(cnt, 3u);
  }
}

TEST(SetTest, DelNesk) {
  s21::set<int> s{26, 17, 41, 14, 21, 30, 47, 10, 19, 23, 38, 7, 12, 35, 39};
  auto it = s.find(41);
  --it;
  EXPECT_EQ(*it, 39);
}

TEST(SetTest, DefaultConstructor) {
  s21::set<int> s;
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
}

TEST(SetTest, InitializerListConstructor) {
  s21::set<int> s{1, 2, 3, 4, 5};
  EXPECT_EQ(s.size(), 5u);
  EXPECT_TRUE(s.contains(1));
  EXPECT_TRUE(s.contains(3));
  EXPECT_TRUE(s.contains(5));
}

TEST(SetTest, CopyConstructor) {
  s21::set<int> original{1, 2, 3};
  s21::set<int> copy(original);

  EXPECT_EQ(original.size(), 3u);
  EXPECT_EQ(copy.size(), 3u);
  EXPECT_TRUE(copy.contains(1));
  EXPECT_TRUE(copy.contains(2));
  EXPECT_TRUE(copy.contains(3));
}

TEST(SetTest, MoveConstructor) {
  s21::set<int> original{1, 2, 3};
  s21::set<int> moved(std::move(original));

  EXPECT_EQ(moved.size(), 3u);
  EXPECT_TRUE(moved.contains(1));
  EXPECT_TRUE(moved.contains(2));
  EXPECT_TRUE(moved.contains(3));
  EXPECT_TRUE(original.empty());
}

TEST(SetTest, MoveAssignment) {
  s21::set<int> original{1, 2, 3};
  s21::set<int> target;
  target = std::move(original);

  EXPECT_EQ(target.size(), 3u);
  EXPECT_TRUE(target.contains(1));
  EXPECT_TRUE(target.contains(2));
  EXPECT_TRUE(target.contains(3));

  EXPECT_TRUE(original.empty());
}

TEST(SetTest, Empty) {
  s21::set<int> s;
  EXPECT_TRUE(s.empty());

  s.insert(1);
  EXPECT_FALSE(s.empty());

  s.erase(s.begin());
  EXPECT_TRUE(s.empty());
}

TEST(SetTest, Size) {
  s21::set<int> s;
  EXPECT_EQ(s.size(), 0u);

  s.insert(1);
  EXPECT_EQ(s.size(), 1u);

  s.insert(2);
  EXPECT_EQ(s.size(), 2u);

  s.erase(s.begin());
  EXPECT_EQ(s.size(), 1u);
}

TEST(SetTest, MaxSize) {
  s21::set<int> s;
  EXPECT_GT(s.max_size(), 0u);
}

TEST(SetTest, Clear) {
  s21::set<int> s{1, 2, 3, 4, 5};
  EXPECT_EQ(s.size(), 5u);

  s.clear();
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
}

TEST(SetTest, Insert) {
  s21::set<int> s;

  auto result1 = s.insert(1);
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(*result1.first, 1);
  EXPECT_EQ(s.size(), 1u);

  auto result2 = s.insert(1);
  EXPECT_FALSE(result2.second);
  EXPECT_EQ(*result2.first, 1);
  EXPECT_EQ(s.size(), 1u);

  auto result3 = s.insert(2);
  EXPECT_TRUE(result3.second);
  EXPECT_EQ(*result3.first, 2);
  EXPECT_EQ(s.size(), 2u);
}

TEST(SetTest, Erase) {
  s21::set<int> s{1, 2, 3, 4, 5};
  EXPECT_EQ(s.size(), 5u);

  auto it = s.begin();
  s.erase(it);
  EXPECT_EQ(s.size(), 4u);
  EXPECT_FALSE(s.contains(1));

  it = s.find(3);
  s.erase(it);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_FALSE(s.contains(3));

  it = s.find(5);
  s.erase(it);
  EXPECT_EQ(s.size(), 2u);
  EXPECT_FALSE(s.contains(5));
}

TEST(SetTest, Swap) {
  s21::set<int> s1{1, 2, 3};
  s21::set<int> s2{4, 5};

  s1.swap(s2);

  EXPECT_EQ(s1.size(), 2u);
  EXPECT_TRUE(s1.contains(4));
  EXPECT_TRUE(s1.contains(5));

  EXPECT_EQ(s2.size(), 3u);
  EXPECT_TRUE(s2.contains(1));
  EXPECT_TRUE(s2.contains(2));
  EXPECT_TRUE(s2.contains(3));
}

TEST(SetTest, Merge) {
  s21::set<int> s1{1, 2, 3};
  s21::set<int> s2{3, 4, 5};

  s1.merge(s2);

  EXPECT_EQ(s1.size(), 5u);
  EXPECT_TRUE(s1.contains(1));
  EXPECT_TRUE(s1.contains(2));
  EXPECT_TRUE(s1.contains(3));
  EXPECT_TRUE(s1.contains(4));
  EXPECT_TRUE(s1.contains(5));

  EXPECT_EQ(s2.size(), 1u);
  EXPECT_TRUE(s2.contains(3));
}

TEST(SetTest, Find) {
  s21::set<int> s{1, 2, 3, 4, 5};

  auto it = s.find(3);
  EXPECT_NE(it, s.end());
  EXPECT_EQ(*it, 3);

  it = s.find(10);
  EXPECT_EQ(it, s.end());
}

TEST(SetTest, Contains) {
  s21::set<int> s{1, 2, 3};

  EXPECT_TRUE(s.contains(1));
  EXPECT_TRUE(s.contains(2));
  EXPECT_TRUE(s.contains(3));
  EXPECT_FALSE(s.contains(4));
  EXPECT_FALSE(s.contains(0));
}

TEST(SetTest, InsertMany) {
  s21::set<int> s;

  auto results = s.insert_many(1, 2, 3, 2, 4);

  EXPECT_EQ(results.size(), 5u);
  EXPECT_TRUE(results[0].second);
  EXPECT_TRUE(results[1].second);
  EXPECT_TRUE(results[2].second);
  EXPECT_FALSE(results[3].second);
  EXPECT_TRUE(results[4].second);

  EXPECT_EQ(s.size(), 4u);
  EXPECT_TRUE(s.contains(1));
  EXPECT_TRUE(s.contains(2));
  EXPECT_TRUE(s.contains(3));
  EXPECT_TRUE(s.contains(4));
}

TEST(SetTest, Iterator) {
  s21::set<int> s{3, 1, 4, 2, 5};

  auto it = s.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(*it, 5);
  ++it;
  EXPECT_EQ(it, s.end());
}

TEST(SetTest, LargeSet) {
  s21::set<int> s;

  for (int i = 0; i < 1000; ++i) {
    s.insert(i);
  }

  EXPECT_EQ(s.size(), 1000u);

  for (int i = 0; i < 1000; ++i) {
    EXPECT_TRUE(s.contains(i));
  }

  for (int i = 0; i < 1000; ++i) {
    auto it = s.find(i);
    s.erase(it);
  }

  EXPECT_TRUE(s.empty());
}

TEST(SetTest, StringSet) {
  s21::set<std::string> s;

  s.insert("apple");
  s.insert("banana");
  s.insert("cherry");

  EXPECT_EQ(s.size(), 3u);
  EXPECT_TRUE(s.contains("apple"));
  EXPECT_TRUE(s.contains("banana"));
  EXPECT_TRUE(s.contains("cherry"));
  EXPECT_FALSE(s.contains("date"));

  auto it = s.find("banana");
  EXPECT_NE(it, s.end());
  EXPECT_EQ(*it, "banana");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}