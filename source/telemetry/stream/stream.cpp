/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include "stream.h"
#include <iostream>

Stream::Stream(std::vector<Sensor>& sensors) {
  for (auto it = sensors.begin(); it != sensors.end(); it++) {
    std::visit(
      [&](auto v) {
        // Create a new channel
        AbstractChannel *channel = new Channel<decltype(v)>(*it);
        _channels[it->channel_id] = channel;
        if (_frequency < it->frequency) {
          _frequency = it->frequency;
        }

        // Create the stream buffer entry for the channel
        decltype(v) variant_type = 0;
        _stream_buffer[it->channel_id] = variant_type;
      },
      it->get_variant()
    );
  }
}

Stream::~Stream() {
  for (auto it = _channels.begin(); it != _channels.end(); it++) {
    delete it->second;
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
      if (_paused) {
        continue;
      }

      // Read from every channel, update if the value has changed significantly
      for (auto it = _channels.begin(); it != _channels.end(); it++) {
        AbstractChannel *abstract_channel = it->second;
        std::visit(
          [&](auto v) {
            // Cast the channel to the right type and check if we should read from the channel at this timestamp
            Channel<decltype(v)> *channel = dynamic_cast<Channel<decltype(v)>*>(abstract_channel);
            unsigned int channel_frequency = channel->sensor.frequency;
            float divisor = floor(1000.0f / float(channel_frequency));
            if (std::fmod(_timestamp, divisor) > std::numeric_limits<float>::epsilon()) {
              return;
            }

            // Update the buffer if the value has significantly changed (0.5% difference)
            SensorRange bounds = channel->sensor.bounds;
            int epsilon = (bounds.upper - bounds.lower) * 0.005f;
            std::visit(
              [&](auto last_value) {
                auto current_value = channel->read();
                if (current_value.has_value()) {
                  if (abs(*current_value - last_value) > epsilon) {
                    _stream_buffer[channel->sensor.channel_id] = *current_value;
                    change_set.insert(it->first);
                  }
                }
              }, 
              _stream_buffer[channel->sensor.channel_id]
            );
          }, 
          abstract_channel->sensor.get_variant()
        );
      }

      // Create the data that only contains values for this timestep and have significantly changed
      std::vector<SensorVariantPair> data;
      for (const unsigned int& channel_id: change_set) {
        unsigned char sensor_id = _channels[channel_id]->sensor.id;
        auto value = _stream_buffer[channel_id];
        data.emplace_back(SensorVariantPair{sensor_id, value});
      }

      // Notify all subscribers of a new data frame. 
      float min_divisor = floor(1000.0f / _frequency);
      if (data.size() != 0 && std::fmod(_timestamp, min_divisor) < std::numeric_limits<float>::epsilon()) {
        for (auto it = _callbacks.begin(); it != _callbacks.end(); it++) {
          it->second(_timestamp, data);
        }
        change_set.clear();
      }
      _timestamp = _timestamp + 1;
    }
    // Run the loop at the highest channel frequency
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

bool Stream::subscribe(unsigned int id, ReadCallback callback) noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (_callbacks.find(id) == _callbacks.end()) {
    _callbacks[id] = callback;
    return true;
  } else {
    return false;
  }
}

bool Stream::unsubscribe(unsigned int id) noexcept {
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
    _paused = false;
    _read_thread = std::thread(&Stream::_tap_channels, this);
  } 
}

void Stream::close() noexcept {
  if (!_closed) {
    {
      std::lock_guard<std::mutex> safe_lock(_lock);
      _paused = true;
      _closed = true;
    }
    _read_thread.join();
  }
}

void Stream::pause() noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (!_closed) {
    _paused = true;
  }
}

void Stream::unpause() noexcept {
  std::lock_guard<std::mutex> safe_lock(_lock);
  if (!_closed) {
    _paused = false;
  }
}

bool Stream::is_open() const noexcept { // Do we need a lock?
  return !_closed;
}

bool Stream::is_paused() const noexcept { // Do we need a lock?
  return _paused;
}