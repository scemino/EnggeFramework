#include <utility>
#include "IndexedPriorityQueue.h"

namespace ngf {
IndexedPriorityQueue::IndexedPriorityQueue(std::vector<float> &keys)
    : _keys(keys) {
}

void IndexedPriorityQueue::insert(int index) {
  _data.push_back(index);
  reorderUp();
}

int IndexedPriorityQueue::pop() {
  int r = _data[0];
  _data[0] = _data[_data.size() - 1];
  _data.pop_back();
  reorderDown();
  return r;
}

void IndexedPriorityQueue::reorderUp() {
  if (_data.empty())
    return;
  size_t a = _data.size() - 1;
  while (a > 0) {
    if (_keys[_data[a]] >= _keys[_data[a - 1]])
      return;
    int tmp = _data[a];
    _data[a] = _data[a - 1];
    _data[a - 1] = tmp;
    a--;
  }
}

void IndexedPriorityQueue::reorderDown() {
  if (_data.empty())
    return;
  for (int a = 0; a < static_cast<int>(_data.size() - 1); a++) {
    if (_keys[_data[a]] <= _keys[_data[a + 1]])
      return;
    int tmp = _data[a];
    _data[a] = _data[a + 1];
    _data[a + 1] = tmp;
  }
}

bool IndexedPriorityQueue::isEmpty() {
  return _data.empty();
}
}