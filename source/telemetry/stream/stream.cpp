/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "stream.h"

Stream::Stream(std::vector<Sensor>& sensors) {
  // Create channels and the stream buffer
  for (auto it = sensors.begin(); it != sensors.end(); it++) {
    std::visit(
      [&](auto v) {
        // Create a new channel
        AbstractChannel *channel = new Channel<decltype(v)>(*it);
        _channels[it->traits["canId"]] = channel;
        if (_frequency < it->traits["frequency"]) {
          _frequency = it->traits["frequency"];
        }

        // Create the stream buffer entry for the channel
        decltype(v) variant_type = 0;
        _stream_buffer[it->traits["canId"]] = variant_type;
      },
      it->get_variant()
    );
  }
}

Stream::~Stream() {
  close();
  for (auto it = _channels.begin(); it != _channels.end(); it++) {
    if (it->second != nullptr) {
      delete it->second;
    }
  }
}

void Stream::_tap_channels() noexcept {
  std::unordered_set<unsigned int> change_set;
  while (1) {
    // Narrow the lock scope
    {
      std::lock_guard<std::mutex> safe_lock(_lock);
      if (_closed) {
        return;
      }

      // Determine which channels we should be reading from. 
      for (auto it = _channels.begin(); it != _channels.end(); it++) {
        AbstractChannel *abstract_channel = it->second;
        std::visit(
          [&](auto v) {
            // Determine whether the sensor value should be read on the next frequency sync
            unsigned int channel_frequency = abstract_channel->sensor.traits["frequency"];
            int divisor = roundf(1000.0f / float(channel_frequency));
            if (_timestamp % divisor == 0) {
              change_set.insert(it->first);
            } 
          }, 
          abstract_channel->sensor.get_variant()
        );
      }

      // Perform a read on the change_set if the timestamp aligns with the highest channel frequency
      int min_divisor = roundf(1000.0f / _frequency);
      if (_timestamp % min_divisor == 0) {
        // Determine which channel data should go into the next frequency synchronization
        std::vector<SensorVariantPair> data;
        for (const unsigned int& channel_id: change_set) {
          std::visit(
            [&](auto last_value) {
              Channel<decltype(last_value)> *channel = dynamic_cast<Channel<decltype(last_value)>*>(_channels[channel_id]);
              auto current_value = channel->read(_timestamp);
              // TODO in future
              // double upper_bound = channel->sensor.traits["upperBound"];
              // double lower_bound = channel->sensor.traits["lowerBound"];
              // int epsilon = (upper_bound - lower_bound) * 0.005f;
              if (current_value.has_value()) {
                if (abs(*current_value - last_value) > 0) {
                  _stream_buffer[channel->sensor.traits["canId"]] = *current_value;
                  data.push_back({channel->sensor.traits["smallId"], *current_value});
                }
              }
            }, 
            _stream_buffer[channel_id]
          );
        }

        // Notify the subscribers of new data; TODO: Call this async
        if (data.size() != 0) {
          for (auto it = _callbacks.begin(); it != _callbacks.end(); it++) {
            std::future<void> f = std::async(std::launch::async, [=](){
              it->second(_timestamp, data);
            });
          }
          change_set.clear();
        }
      }
      _timestamp = _timestamp + 1;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

bool Stream::subscribe(const unsigned int& id, ReadCallback callback) noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (_callbacks.find(id) == _callbacks.end()) {
    _callbacks[id] = callback;
    return true;
  } else {
    return false;
  }
}

bool Stream::unsubscribe(const unsigned int& id) noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (_callbacks.find(id) == _callbacks.end()) {
    return false;
  } else {
    _callbacks.erase(id);
    return true;
  }
}

void Stream::open() noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (_closed)
  {
    _closed = false;
    _read_thread = std::thread(&Stream::_tap_channels, this);
  } 
}

void Stream::close() noexcept {
  if (!_closed) {
    {
      std::lock_guard<std::mutex> safe_lock(_lock);
      _closed = true;
      _timestamp = 0;
    }
    _read_thread.join();
  }
}

bool Stream::is_open() const noexcept {
  return !_closed;
}
