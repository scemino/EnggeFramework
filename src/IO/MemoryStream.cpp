#include <ngf/IO/MemoryStream.h>

namespace ngf {

MemoryStream::membuf::membuf(char *begin, char *end) {
  this->setg(begin, begin, end);
}

void MemoryStream::membuf::setBuffer(char *begin, char *end){
  this->setg(begin, begin, end);
}

MemoryStream::pos_type MemoryStream::membuf::seekoff(
    off_type off, seekdir dir, openmode) {
  if (dir == cur)
    gbump(off);
  else if (dir == end)
    setg(eback(), egptr() + off, egptr());
  else if (dir == beg)
    setg(eback(), eback() + off, egptr());
  return gptr() - eback();
}

MemoryStream::pos_type MemoryStream::membuf::seekpos(
    pos_type sp, openmode which) {
  return seekoff(sp - pos_type(off_type(0)), beg, which);
}

MemoryStream::MemoryStream(char *begin, char *end)
    : std::istream(&m_buf), m_buf(begin, end) {
}
}
