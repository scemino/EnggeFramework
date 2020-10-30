#pragma once
#include <vector>

namespace ngf {
class IndexedPriorityQueue {
  std::vector<float> &_keys;
  std::vector<int> _data;

public:
  explicit IndexedPriorityQueue(std::vector<float> &keys);

  void insert(int index);
  int pop();

  void reorderUp();
  void reorderDown();

  bool isEmpty();
};
}
