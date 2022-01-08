/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

/* Unit Tests */
#define TESTING 1
#if TESTING == 1
  #include "vfdc_encoder.h"
  #include <iostream>
  #include <cassert>
  #include <bitset>

  void test_one_value() {
    unsigned char id = 1;
    SensorDataVariant datum = int(1000);
    SensorVariantPair pair = { id, datum };
    std::vector<SensorVariantPair> data;
    data.push_back(pair);
    CompressedData compressed = VFDCPEncoder::get().encode_data(data);

    // Ensure our size is 6
    assert(std::get<1>(compressed) == 6);

    // Ensure out sensor count is 1
    char *encoding = std::get<0>(compressed);
    assert(encoding[0] == 1);

    // Ensure the first byte is the char id = 1
    assert(encoding[1] == 1);
    
    // Ensure the first value is 1000
    int value = *(int*)(&encoding[2]);
    assert(value == 1000);
  }

  void test_multiple_values() {
    std::vector<SensorVariantPair> data;
    long long first = 30;
    data.push_back({1, first}); // 8
    data.push_back({2, double(15)}); // 8
    data.push_back({3, float(10)}); // 4
    data.push_back({4, int(4)}); // 4
    data.push_back({5, short(3)}); // 2
    data.push_back({6, char(2)}); // 1
    data.push_back({7, bool(false)}); // 1
    CompressedData compressed = VFDCPEncoder::get().encode_data(data);
    
    // Ensure our size is 1 + 7 + 8 + 8 + 4 + 4 + 2 + 1 + 1 = 36 bytes
    assert(std::get<1>(compressed) == 36);
  }

  int main() {
    test_one_value();
    test_multiple_values();
  }
#endif