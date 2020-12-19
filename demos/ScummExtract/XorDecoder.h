#pragma once
#include <iostream>

class XorDecoder {
public:
  void setInput(std::istream &input) {
    m_input = &input;
  }

  void setEncodeByte(uint8_t encByte) {
    m_encByte = encByte;
  }

  void read(char *data, size_t size) {
    assert(m_input);
    m_input->read((char *) data, size);
    for (auto i = 0; i < size; i++) {
      *data = *data ^ m_encByte;
      data++;
    }
  }

  uint8_t readByte() {
    uint8_t b;
    read((char *) &b, 1);
    return b;
  }

  uint16_t readUShort() {
    uint16_t us;
    read((char *) &us, 2);
    return us;
  }

private:
  std::istream *m_input{nullptr};
  uint8_t m_encByte{0x69};
};
