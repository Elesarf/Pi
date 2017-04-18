#include "long_hash.h"

#include <sstream>
#include <iostream>

#include <iomanip>


ostream& operator<<(ostream& o, const Long_hash& bn) {return (o << bn.Dump());}

Long_hash::Long_hash(uint16_t size) {
  __size_data_bits = size;
  __size_data_bytes =
      ((size + 8 - 1) / 8 + sizeof(uint64_t) - 1) / sizeof(uint64_t);

  __data = new uint64_t[__size_data_bytes];

  for (uint64_t i = 0; i < __size_data_bytes; ++i) {
    __data[i] = 0;
  }
}

Long_hash::~Long_hash() {
  if (__data)
    delete[] __data;
}

string Long_hash::Dump() const {
  stringstream sstr;
  sstr << "0x";
  for (int i = 0; i < __size_data_bytes; ++i) {
    sstr << setfill('0') << setw(16) << hex << __data[i] << " ";
  }

  sstr << "\n";
  return sstr.str();
}

void Long_hash::SetBit(uint16_t bitNum, bool on) {
  int arrIndex = bitNum / 8 / sizeof(uint64_t);
  int bitIndex = bitNum % (sizeof(uint64_t) * 8);

  if ((on) && (bitNum <= __size_data_bytes * sizeof(uint64_t) * 8)) {
    __data[arrIndex] |= 1ULL << bitIndex;
  } else {
    __data[arrIndex] &= ~(1ULL << bitIndex);
  }
}

bool Long_hash::GetBit(uint16_t bitNum) {
  int arrIndex = bitNum / 8 / sizeof(uint64_t);
  int bitIndex = bitNum % (sizeof(uint64_t) * 8);

  return __data[arrIndex] & (1UL << bitIndex);
}

uint64_t Long_hash::distance(Long_hash* hash) {
  uint64_t dist = 0;

  for (uint16_t i = 0; i < this->GetSizeBits(); ++i) {
    if (((this->GetBit(i)) && !(hash->GetBit(i))) ||
        (!(this->GetBit(i)) && (hash->GetBit(i)))) {
      ++dist;
    }
  }

  return dist;
}

