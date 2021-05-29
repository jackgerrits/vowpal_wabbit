// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace vw
{
template <typename T>
class ptr_queue
{
public:
  ptr_queue(size_t max_size) : max_size(max_size) {}

  T* pop()
  {
    std::unique_lock<std::mutex> lock(mut);
    while (object_queue.size() == 0 && !done) { is_not_empty.wait(lock); }

    if (done && object_queue.size() == 0) { return nullptr; }

    auto item = object_queue.front();
    object_queue.pop();

    is_not_full.notify_all();
    return item;
  }

  void push(T* item)
  {
    std::unique_lock<std::mutex> lock(mut);
    while (object_queue.size() == max_size) { is_not_full.wait(lock); }
    object_queue.push(item);

    is_not_empty.notify_all();
  }

  void set_done()
  {
    {
      std::unique_lock<std::mutex> lock(mut);
      done = true;
    }
    is_not_empty.notify_all();
    is_not_full.notify_all();
  }

  size_t size() const
  {
    std::unique_lock<std::mutex> lock(mut);
    return object_queue.size();
  }

private:
  size_t max_size;
  std::queue<T*> object_queue;
  mutable std::mutex mut;

  volatile bool done = false;

  std::condition_variable is_not_full;
  std::condition_variable is_not_empty;
};
}  // namespace vw
