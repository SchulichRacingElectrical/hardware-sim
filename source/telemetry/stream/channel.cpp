/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "channel.h"

template <typename T>
  requires (std::is_arithmetic<T>::value)
void Channel<T>::send(T value) {
  std::function<void()> insert_value = [this, value]() {
    std::lock_guard<std::mutex> safe_lock(_lock);
    _queue.push_back(value);
  };
  if (_queue.empty()) insert_value();
  T current = _queue.back();
  if (current != value) {
    double difference = static_cast<double>(abs(current - value));
    if (difference >= _epsilon) insert_value();
  }
}

template<typename T>
  requires (std::is_arithmetic<T>::value)
std::optional<T> Channel<T>::read() {
  while (true) {
    {
      std::lock_guard<std::mutex> safe_lock(_lock);
      if (!_queue.empty()) {
        T value = _queue.front();
        _queue.pop_front();
        return value;
      }
      if (_closed)
        return std::nullopt;
    }
    usleep(1000);
  }
}

template<typename T>
  requires (std::is_arithmetic<T>::value)
void Channel<T>::close() {
  std::lock_guard<std::mutex> safe_lock(_lock);
  _closed = true;
}

/* Tests */
void test_epsilon() {
  // Test non-significant insertion
  Channel<int> c(1);
  c.send(1);
  c.send(1);
  int count_reads = 0;
  c.close();
  while (c.read().has_value()) count_reads++;
  assert(count_reads == 1);

  // Test significant insertion
  Channel<int> a(1);
  a.send(1);
  a.send(2);
  a.send(3);
  count_reads = 0;
  a.close();
  while (a.read().has_value()) count_reads++;
  assert(count_reads == 3);

  // Test significant and non-significant insertion
  Channel<int> b(1);
  b.send(1);
  b.send(1);
  b.send(2);
  b.send(3);
  b.send(3);
  count_reads = 0;
  b.close();
  while (b.read().has_value()) count_reads++;
  assert(count_reads == 3);
}

int main() {
  test_epsilon();
}