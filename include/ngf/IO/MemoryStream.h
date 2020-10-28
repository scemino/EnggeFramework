#pragma once
#include <iostream>

namespace ngf {
struct MemoryStream : std::istream {
public:
  struct membuf : std::streambuf {
    membuf(char *begin, char *end);

    void setBuffer(char *begin, char *end);

    pos_type seekoff(off_type off, seekdir dir, openmode which) override;
    pos_type seekpos(pos_type sp, openmode which) override;
  };

public:
  MemoryStream(char *begin, char *end);


private:
  membuf m_buf;
};
}