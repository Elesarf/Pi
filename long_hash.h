#ifndef LONG_HASH_H
#define LONG_HASH_H

#include <stdint.h>
#include <string>
#include <iostream>

using namespace std;
class Long_hash {
 public:
  Long_hash(uint16_t size);
  ~Long_hash();

  string Dump() const;
  void SetBit(int bitNum, bool on = true);
  bool GetBit(int bitNum);
  uint64_t distance(Long_hash& hash);

  uint16_t GetSize() { return __size_data_bytes; }
  uint16_t GetSizeBits() { return __size_data_bits; }

  friend ostream& operator<<(ostream& o, const Long_hash& bn);

 private:
  uint16_t __size_data_bytes;
  uint16_t __size_data_bits;

  uint64_t* __data;
};


#endif // LONG_HASH_H
