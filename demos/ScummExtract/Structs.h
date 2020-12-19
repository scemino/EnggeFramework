#pragma once

enum class ResType {
  Room, Script, Sound, Costume
};

#pragma pack(1)
struct Block {
  uint32_t size;
  uint16_t type;
};

#pragma pack(1)
struct RoomName {
  uint8_t room;
  char name[10];
};

#pragma pack(1)
struct RoomHeader {
  uint16_t width;
  uint16_t height;
  uint16_t n;
};

#pragma pack(1)
struct FileOffset {
  uint8_t room;
  uint32_t offset;
};

#pragma pack(1)
struct Res {
  uint8_t room;
  uint32_t offset;
};

#pragma pack(1)
struct Box {
  int16_t ulx, uly;
  int16_t urx, ury;
  int16_t lrx, lry;
  int16_t llx, lly;
  uint8_t mask;
  uint8_t flags;
  uint16_t scale;
};

#pragma pack(1)
struct CodeHeader {
  uint16_t obj_id;
  uint8_t unk;
  uint8_t x;
  uint8_t y: 7;
  uint8_t parentState: 1;
  uint8_t width;
  uint8_t parent;
  int16_t walk_x;
  int16_t walk_y;
  uint8_t actordir: 3;
  uint8_t height: 5;
};

#pragma pack(1)
struct ImageHeader {
  uint16_t obj_id;
  uint16_t image_count;
  uint16_t unk[1];
  uint8_t flags;
  uint8_t unk1;
  uint16_t unk2[2];
  uint16_t width;
  uint16_t height;
  uint16_t hotspot_num;
  struct {
    int16_t x, y;
  } hotspot[15];
};
