/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "channel.h"
#include <any>
#include <iostream>

template <typename T> requires (std::is_arithmetic<T>::value)
void Channel<T>::send(T v) {
  if (!_closed) {
    std::lock_guard<std::mutex> safe_lock(_lock);
    _value = v;
  }
}

template<typename T> requires (std::is_arithmetic<T>::value)
std::optional<T> Channel<T>::read() const {
  if (_closed) return std::nullopt;
  std::lock_guard<std::mutex> safe_lock(_lock);
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