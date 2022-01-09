/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "channel.h"

template <typename T> requires (std::is_arithmetic<T>::value)
void Channel<T>::send(T v) {
  if (!_closed) {
    std::lock_guard<std::mutex> safe_lock(_lock);
    _value = v;
  }
}

template<typename T> requires (std::is_arithmetic<T>::value)
std::optional<T> Channel<T>::read() {
  if (_closed) return std::nullopt;
  std::lock_guard<std::mutex> safe_lock(_lock);
  _value = _generate_random();
  return _value;
}

template<typename T> requires (std::is_arithmetic<T>::value)
void Channel<T>::open() {
  std::lock_guard<std::mutex> safe_lock(_lock);
  _closed = false;
}

template<typename T> requires (std::is_arithmetic<T>::value)
void Channel<T>::close() {
  std::lock_guard<std::mutex> safe_lock(_lock);
  _closed = true;
}

template<typename T> requires (std::is_arithmetic<T>::value)
T Channel<T>::_generate_random() {
  srand(time(nullptr));
  SensorRange bounds = _sensor._bounds;
  unsigned long range = bounds.upper = bounds.lower;
  return (T)((rand() * range) + bounds.lower);
}