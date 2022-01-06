/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <type_traits>
#include <functional>
#include <optional>
#include <mutex>
#include <deque>

template <typename T>
  requires (std::is_arithmetic<T>::value)
class Channel
{
private:
  std::mutex _lock;
  std::deque<T> _queue;
  bool _closed = false;
  double _epsilon;

public:
  Channel(double e) : _epsilon(e) {}
  void send(T value);
  std::optional<T> read();
  void close();
};