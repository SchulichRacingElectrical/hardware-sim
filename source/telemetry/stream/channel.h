/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#pragma once
#include <functional>
#include <optional>
#include <mutex>
#include <deque>

class AbstractChannel {
  public:
    virtual ~AbstractChannel() {}
};

/**
 * We assume that multiple threads can write to and read from the channel at once. 
 * The channel may be open and closed by any thread as well.
 */
template <typename T> requires (std::is_arithmetic<T>::value)
class Channel: public AbstractChannel {
private:
  std::optional<T> _value = std::nullopt;
  std::mutex _lock;
  bool _closed = false;

public:
  void send(T value);
  std::optional<T> read() const;
  void open();
  void close();
};