#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <imgui.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include "Structs.h"
#include "XorDecoder.h"

struct ResNode {
  uint32_t offset{0};
  int disk{0};
  Block block{};
  std::shared_ptr<ResNode> parent;
  std::vector<std::shared_ptr<ResNode>> nodes;
};

#pragma pack(1)
struct LimbPic {
  uint16_t width;
  uint16_t height;
  int16_t rel_x;
  int16_t rel_y;
  int16_t move_x;
  int16_t move_y;
};

struct CostumeAnim {
  uint16_t stopped;
  uint16_t curpos[16];
  uint16_t start[16];
  uint16_t end[16];
  uint16_t frame[16];

  CostumeAnim() {
    stopped = 0;
    for (int i = 0; i < 16; i++) {
      curpos[i] = start[i] = end[i] = frame[i] = 0xFFFF;
    }
  }
};

class ScummExtractApplication final : public ngf::Application {
public:
private:
  void onInit() override {
    m_window.init({"Scumm Extract", {1024, 768}});
  }

  void onEvent(ngf::Event &event) {
    switch (event.type) {
    case ngf::EventType::DropFile: {
      load(event.drop.file);
    }
      break;
    }
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::AliceBlue);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Explorer");
    for (const auto &node: m_nodes) {
      drawNode(node);
    }
    ImGui::End();

    ImGui::Begin("Preview");
    if (m_selectedNode) {
      ImGui::Text("Type: 0x%X", m_selectedNode->block.type);
      ImGui::Text("Offset: %u", m_selectedNode->offset);
      ImGui::Text("Size: %u", m_selectedNode->block.size);
      ImGui::Separator();
      switch (m_selectedNode->block.type) {
      case 0x4D42: // BM
      {
        if (m_textures.empty()) {
          auto imgs = getImages(*m_selectedNode);
          for (const auto &img : imgs) {
            m_textures.push_back(std::make_unique<ngf::Texture>(img));
          }
        }
        for (const auto &texture : m_textures) {
          ImGui::Image((ImTextureID) texture->getHandle(), ImVec2(texture->getSize().x, texture->getSize().y));
          ImGui::PushID(texture.get());
          if (ImGui::Button("Extract")) {
            auto img = texture->copyToImage();
            img.saveToFile("image.png");
          }
          ImGui::PopID();
        }
      }
        break;
      case 0x4448: // HD
      {
        RoomHeader header{};
        readData(*m_selectedNode, &header);
        ImGui::Text("Width: %u", header.width);
        ImGui::Text("Height: %u", header.height);
        ImGui::Text("#Objects: %u", header.n);
      }
        break;
      case 0x494F: // OI
      {
        std::vector<uint8_t> data(m_selectedNode->block.size);
        readData(*m_selectedNode, data.data());
        auto id = *(uint16_t *) data.data();
        ImGui::Text("Id: %u", id);
        if (ImGui::Button("Extract All")) {
          extractAll(*m_selectedNode);
        }
        if (m_selectedNode->block.size > 8) {
          if (m_textures.empty()) {
            auto imgs = getObjectImages(*m_selectedNode);
            for (const auto &img : imgs) {
              m_textures.push_back(std::make_unique<ngf::Texture>(img));
            }
          }
          for (const auto &texture : m_textures) {
            ImGui::Image((ImTextureID) texture->getHandle(), ImVec2(texture->getSize().x, texture->getSize().y));
            ImGui::PushID(texture.get());
            if (ImGui::Button("Extract")) {
              auto img = texture->copyToImage();
              img.saveToFile("image.png");
            }
            ImGui::PopID();
          }
        }
      }
        break;
      case 0x434F: // OC
      {
        std::vector<uint8_t> data(m_selectedNode->block.size);
        readData(*m_selectedNode, data.data());
        CodeHeader header = *(CodeHeader *) data.data();
        auto offsetName = *(data.data() + sizeof(CodeHeader));
        auto ptrVerb = (data.data() + sizeof(CodeHeader) + 1);
        auto ptrName = (char *) (data.data() + offsetName - sizeof(Block));
        ImGui::Text("Name: %s", ptrName);
        ImGui::Text("Id: %u", header.obj_id);
        ImGui::Text("Pos: (%u,%u)", header.x * 8, header.y * 8);
        ImGui::Text("Size: (%u,%u)", header.width * 8, header.height * 8);
        ImGui::Text("Walk: (%d,%d)", header.walk_x, header.walk_y);
        ImGui::Text("Actor dir: %d", header.actordir);
        while (*ptrVerb) {
          ImGui::Text("Verb: %d-%d", *ptrVerb, *(uint16_t *) (ptrVerb + 1));
          ptrVerb += 3;
        }
      }
        break;
      case 0x4F43: // CO
      {
        auto pos = m_input.tellg();
        std::vector<uint8_t> data(m_selectedNode->block.size);
        readData(*m_selectedNode, data.data());

        auto nodeRO = getChildNode(m_selectedNode->parent, 0x4F52);
        auto nodePA = getChildNode(nodeRO, 0x4150);
        std::vector<uint8_t> palette(nodePA->block.size);
        readData(*nodePA, palette.data());
        uint8_t *pal = palette.data() + 2;

        uint8_t numAnim = data[0];
        uint8_t numColors = (1 + data[1] & 1) * 16;
        auto format = data[1] & 0x7F;
        bool mirror = (data[1] & 0x80);
        uint8_t *actorPalette = data.data() + 2;
        uint16_t animCmdOffset = *(uint16_t *) (actorPalette + numColors);
        uint8_t *animCmds = data.data() - 6 + animCmdOffset;
        uint8_t *limbsOffsets = (uint8_t *) (actorPalette + numColors + 2);
        auto *animOffsets = (uint8_t *) (limbsOffsets + 16);

        std::vector<CostumeAnim> anims(numAnim);
        uint8_t extra, cmd;
        uint16_t j;
        for (auto nAnim = 0; nAnim < numAnim; nAnim++) {
          uint16_t usemask = 0xFFFF;
          auto frame = nAnim / 4;
          auto &anim = anims[nAnim];
          auto offset = *(uint16_t *) (animOffsets + nAnim * 2);
          if (offset == 0)
            continue;
          uint8_t *r = data.data() - 6 + offset;
          uint16_t mask = *(uint16_t *) r;
          r += 2;
          int i = 0;
          do {
            if (mask & 0x8000) {
              j = *(uint16_t *) r;
              r += 2;
              if (usemask & 0x8000) {
                if (j == 0xFFFF) {
                  anim.curpos[i] = 0xFFFF;
                  anim.start[i] = 0;
                  anim.frame[i] = frame;
                } else {
                  extra = *r++;
                  cmd = animCmds[j];
                  if (cmd == 0x7A) {
                    anim.stopped &= ~(1 << i);
                  } else if (cmd == 0x79) {
                    anim.stopped |= (1 << i);
                  } else {
                    anim.curpos[i] = anim.start[i] = j;
                    anim.end[i] = j + (extra & 0x7F);
                    if (extra & 0x80)
                      anim.curpos[i] |= 0x8000;
                    anim.frame[i] = frame;
                  }
                }
              } else {
                if (j != 0xFFFF)
                  r++;
              }
            }
            i++;
            usemask <<= 1;
            mask <<= 1;
          } while (mask & 0xFFFF);
        }

        for (size_t nAnim = 0; nAnim < anims.size(); nAnim++) {
          const auto &anim = anims[nAnim];
          for (auto limb = 0; limb < 16; limb++) {
            std::cout << "Anim #" << nAnim << " limb #" << limb << std::endl;
            // If the specified limb is stopped or not existing, do nothing.
            if (anim.curpos[limb] == 0xFFFF || anim.stopped & (1 << limb))
              continue;

            // Determine the position the limb is at
            int i = anim.curpos[limb] & 0x7FFF;
            auto limbOffset = *(uint16_t *) (limbsOffsets + limb * 2);
            uint8_t *frameptr = &data.at(limbOffset - 6);

            // Determine the offset to the costume data for the limb at position i
            uint8_t code = animCmds[i] & 0x7F;

            // Code 0x7B indicates a limb for which there is nothing to draw
            if (code == 0x7B)
              continue;

            auto frameOffset = *(uint16_t *) (frameptr + code * 2);
            if (frameOffset <= 16)
              continue;

            if ((frameOffset - 6) >= data.size())
              continue;
            auto srcptr = &data.at(frameOffset - 6);

            const auto &pic = *(const LimbPic *) srcptr;
            if (pic.width == 0xFFFF || pic.width == 0 || pic.height == 0)
              continue;
            srcptr += 12;

            std::vector<uint8_t> dst(pic.width * pic.height * 4);
            decodeRLE(srcptr, dst.data(), actorPalette, pal, pic.width, pic.height, numColors);
            ngf::Image img({pic.width, pic.height}, dst.data());
            img.saveToFile("anim" + std::to_string(limb) + "_" + std::to_string(nAnim) + ".png");
          }
        }
      }
        break;
      case 0x5842: // BX
      {
        if (ImGui::Button("Extract")) {
          extractBoxes(*m_selectedNode);
        }
        std::vector<uint8_t> data(m_selectedNode->block.size);
        readData(*m_selectedNode, data.data());
        uint8_t nBoxes = data[0];
        auto boxes = (Box *) (data.data() + 1);
        for (auto i = 0; i < nBoxes; i++) {
          auto box = boxes[i];
          ImGui::Text("Box %d", i);
          ImGui::Text("((%d,%d),(%d,%d),(%d,%d),(%d,%d))",
                      box.ulx,
                      box.uly,
                      box.urx,
                      box.ury,
                      box.lrx,
                      box.lry,
                      box.llx,
                      box.lly);
          ImGui::Text("Mask: %u", box.mask);
          ImGui::Text("Scale: %u", box.scale);
          ImGui::Text("Flags: %u", box.flags);
          ImGui::Separator();
        }
      }
        break;
      }
    }
    ImGui::End();
  }

private:
  void load(const std::string &filePath) {
    std::filesystem::path directory = filePath;
    directory = directory.parent_path();
    m_nodes.clear();
    openDisk(directory, 1);
    openDisk(directory, 2);
    openDisk(directory, 3);
    openDisk(directory, 4);
  }

  void decodeRLE(uint8_t *src,
                 uint8_t *dst,
                 uint8_t *actorPal,
                 uint8_t *pal,
                 uint16_t w,
                 uint16_t h,
                 uint8_t numColors) {
    uint8_t shift, mask, rep, color;
    int x = 0, y = 0;
    if (numColors == 16) {
      shift = 4;
      mask = 0xF;
    } else {
      shift = 3;
      mask = 0x7;
    }

    while (true) {
      rep = *src++;
      color = rep >> shift;
      rep &= mask;
      if (!rep)
        rep = *src++;
      while (rep > 0) {
        if (color >= 16)
          throw std::invalid_argument("invalid color index");
        auto p = pal + actorPal[color] * 3;
        auto ptr = dst + x * 4 + y * 4 * w;
        ptr[0] = p[0];
        ptr[1] = p[1];
        ptr[2] = p[2];
        ptr[3] = 0xFF;
        rep--;
        y++;
        if (y >= h) {
          y = 0;
          x++;
          if (x >= w)
            return;
        }
      }
    }
  }
  void extractBoxes(ResNode &node) {
    std::vector<uint8_t> data(m_selectedNode->block.size);
    readData(*m_selectedNode, data.data());
    uint8_t nBoxes = data[0];
    auto boxes = (Box *) (data.data() + 1);
    std::ofstream os("boxes.txt");
    for (auto i = 0; i < nBoxes; i++) {
      auto box = boxes[i];
      os << "{\npolygon: \"{" << box.ulx << ',' << box.uly << "};{" << box.urx << ',' << box.ury << "};{" << box.lrx
         << ',' << box.lry << "};{" << box.llx << ',' << box.lly << "}\"\n}\n";
    }
    os.close();
  }
  void extractAll(ResNode &node) {
    auto parent = node.parent;
    for (const auto &child : parent->nodes) {
      if (child->block.type != 0x494F)
        continue;
      std::vector<uint8_t> data(child->block.size);
      readData(*child, data.data());
      auto id = *(uint16_t *) data.data();
      if (id == 328 || id == 329)
        continue;
      try {
        auto imgs = getObjectImages(*child);
        if (imgs.empty())
          continue;

        std::ostringstream s;
        s << "obj_" << id << ".png";
        imgs[0].saveToFile(s.str());
      } catch (...) {

      }
    }
  }

  void drawNode(const std::shared_ptr<ResNode> &node) {
    std::ostringstream s;
    s << ((char) (node->block.type & 0xFF)) << ((char) ((node->block.type & 0xFF00) >> 8));
    ImGuiTreeNodeFlags flags = 0;
    if (m_selectedNode == node) {
      flags = ImGuiTreeNodeFlags_Selected;
    }
    bool isExpanded = ImGui::TreeNodeEx(node.get(), flags, "%s", s.str().c_str());
    if (ImGui::IsItemClicked()) {
      if (m_selectedNode != node) {
        m_selectedNode = node;
        m_textures.clear();
      }
    }
    if (isExpanded) {
      for (const auto &childNode: node->nodes) {
        drawNode(childNode);
      }
      ImGui::TreePop();
    }
  }

  void openDisk(const std::filesystem::path &directoryPath, int index) {
    auto filePath = directoryPath;
    std::ostringstream os;
    filePath = filePath.append("DISK0" + std::to_string(index) + ".LEC");
    m_input.open(filePath, std::ios::binary);
    m_decoder.setInput(m_input);
    auto node = std::make_shared<ResNode>();
    node->disk = index;
    node->offset = 0;
    node->block = readBlock();
    if (node->block.type != 0x454C) {
      assert(false);
    }
    process(node);
    m_nodes.push_back(node);
    m_input.close();
  }

#pragma pack(1)
  struct Res {
    uint8_t room;
    uint32_t offset;
  };

  void openIndex(const std::filesystem::path &directoryPath) {
    auto filePath = directoryPath;
    std::ostringstream os;
    filePath = filePath.append("000.lfl");
    m_input.open(filePath, std::ios::binary);
    m_decoder.setEncodeByte(0);
    m_decoder.setInput(m_input);
    auto block = readBlock();
    while (block.type != 0x4330) {
      m_input.seekg(block.size - 6, std::ios::cur);
      block = readBlock();
    }
    auto numCostumes = m_decoder.readUShort();
    std::vector<Res> costumes(numCostumes);
    m_input.read((char *) costumes.data(), costumes.size() * sizeof(Res));
    m_input.close();
  }

  void skip(const std::shared_ptr<ResNode> &parent, const Block &block) {
    auto pos = static_cast<uint32_t>(m_input.tellg());
    auto size = static_cast<uint32_t>(block.size - sizeof(Block));
    auto max = parent->offset + parent->block.size;
    auto posMin = std::min(pos + size, max);
    m_input.seekg(posMin, std::ios::beg);
  }

  void process(const std::shared_ptr<ResNode> &parent) {
    do {
      auto node = std::make_shared<ResNode>();
      node->disk = parent->disk;
      node->offset = m_input.tellg();
      node->block = readBlock();

      switch (node->block.type) {
      case 0x464C: // LF
      {
        m_input.seekg(2, std::ios::cur);
        process(node);
        node->parent = parent;
        parent->nodes.push_back(node);
      }
        break;
      case 0x4F52: // RO
      {
        process(node);
        node->parent = parent;
        parent->nodes.push_back(node);
      }
        break;
      default:skip(parent, node->block);
        node->parent = parent;
        parent->nodes.push_back(node);
        break;
      }
    } while (m_input.tellg() < parent->offset + parent->block.size);
  }

  Block readBlock() {
    Block block{};
    m_decoder.read((char *) &block, sizeof(Block));
    std::cout << "Block: " << (char) (block.type & 0x00FF) << (char) ((block.type & 0xFF00) >> 8)
              << " offset: " << ((int) m_input.tellg() - 6)
              << " size:   " << block.size << '\n';
    return block;
  }

  static std::shared_ptr<ResNode> getChildNode(const std::shared_ptr<ResNode> &parent, uint16_t type) {
    auto it = std::find_if(parent->nodes.begin(), parent->nodes.end(), [type](const auto node) {
      return node->block.type == type;
    });
    if (it == parent->nodes.end())
      return nullptr;
    return (*it);
  }

  static std::shared_ptr<ResNode> getChildNode(const std::shared_ptr<ResNode> &parent,
                                               std::function<bool(std::shared_ptr<ResNode>)> predicate) {
    auto it = std::find_if(parent->nodes.begin(), parent->nodes.end(), predicate);
    if (it == parent->nodes.end())
      return nullptr;
    return (*it);
  }

  static void readData(const ResNode &node, void *data) {
    std::ostringstream os;
    os << "/Users/scemino/Downloads/Secret Of Monkey Island Vga (1990)(Lucas Arts)/DISK0" << node.disk << ".LEC";
    std::ifstream input(os.str(), std::ios::binary);
    XorDecoder decoder;
    decoder.setInput(input);
    input.seekg(node.offset + sizeof(Block));
    decoder.read((char *) data, node.block.size - sizeof(Block));
    input.close();
  }

  static std::vector<ngf::Image> getObjectImages(const ResNode &node) {
    if (node.block.size <= 8)
      return {};

    // read data
    std::vector<uint8_t> data(node.block.size);
    readData(node, data.data());
    auto id = *(uint16_t *) data.data();

    // read OD corresponding to the object id
    auto nodeOD = getChildNode(node.parent, [id](const std::shared_ptr<ResNode> &node) {
      if (node->block.type != 0x434F)
        return false;
      std::vector<uint8_t> data(node->block.size);
      readData(*node, data.data());
      auto header = *(CodeHeader *) data.data();
      return header.obj_id == id;
    });

    // read header
    std::vector<uint8_t> headerData(nodeOD->block.size);
    readData(*nodeOD, headerData.data());
    auto header = *(CodeHeader *) headerData.data();
    auto width = header.width * 8;
    auto height = header.height * 8;

    // read palette
    auto nodePA = getChildNode(node.parent, 0x4150);
    std::vector<uint8_t> palette(nodePA->block.size);
    readData(*nodePA, palette.data());
    uint8_t *pal = palette.data() + 2;

    // decode images
    return decodeImages(width, height, data.data() + 2, pal);
  }

  static std::vector<ngf::Image> decodeImages(uint16_t width,
                                              uint16_t height,
                                              uint8_t *data,
                                              uint8_t *pal) {
    auto smapLen = *(uint32_t *) data;
    auto *offsets = (uint32_t *) (data + 4);
    auto numStrips = width / 8;
    auto dstPitch = width * 4;

    std::vector<uint8_t> pixels(width * height * 4);
    if (pixels.empty())
      return {};

    std::fill(pixels.begin(), pixels.end(), 255);
    auto dst = pixels.data();
    for (auto i = 0; i < numStrips; i++) {
      auto offset = offsets[i];
      auto src = data + offset;
      uint8_t code = *src++;
      switch (code) {
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:drawStripBasicV(code, src, dst, dstPitch, 4, height, false, pal);
        break;
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:drawStripBasicH(code, src, dst, dstPitch, 4, height, false, pal);
        break;
      default:throw std::logic_error("Invalid smap code: " + std::to_string(code));
      }
      dst += 8 * 4;
    }

    std::vector<ngf::Image> images;
    ngf::Image img(glm::uvec2(width, height), pixels.data());
    images.push_back(img);

    // read zplanes
    auto numZplanes = 0;
    auto zptr = data;
    auto off = smapLen;
    while (off && numZplanes < 4) {
      numZplanes++;
      zptr += off;
      std::vector<std::uint8_t> dst(width * height * 4);
      auto zplaneOffsets = (uint16_t *) (zptr + 2);
      for (auto i = 0; i < numStrips; i++) {
        auto zplaneOff = zplaneOffsets[i];
        decompressMaskImg(img.getPixelsPtr() + i * 32, dst.data() + i * 32, zptr + zplaneOff, height, dstPitch);
      }
      ngf::Image zplaneImg({width, height}, dst.data());
      images.push_back(zplaneImg);
      off = *(uint16_t *) zptr;
    }

    return images;
  }

  static std::vector<ngf::Image> getImages(const ResNode &node) {
    // read header
    auto nodeHD = getChildNode(node.parent, 0x4448);
    RoomHeader header{};
    readData(*nodeHD, &header);

    // read palette
    auto nodePA = getChildNode(node.parent, 0x4150);
    std::vector<uint8_t> palette(nodePA->block.size);
    readData(*nodePA, palette.data());
    uint8_t *pal = palette.data() + 2;

    // read data
    std::vector<uint8_t> data(node.block.size);
    readData(node, data.data());

    // decode images
    return decodeImages(header.width, header.height, data.data(), pal);
  }

  static void writeMask(uint8_t *org, uint8_t *dst, const uint8_t color) {
    uint8_t mask = 0x80;
    for (auto i = 0; i < 8; i++) {
      auto c = (color & mask);
      mask >>= 1;
      *dst++ = c ? *org : 0;
      org++;
      *dst++ = c ? *org : 0;
      org++;
      *dst++ = c ? *org : 0;
      org++;
      *dst++ = c ? *org : 0;
      org++;
    }
  }

  static void decompressMaskImg(uint8_t *org, uint8_t *dst, const uint8_t *src, int height, int dstPitch) {
    uint8_t b, c;

    while (height) {
      b = *src++;

      if (b & 0x80) {
        b &= 0x7F;
        c = *src++;

        do {
          writeMask(org, dst, c);
          org += dstPitch;
          dst += dstPitch;
          --height;
        } while (--b && height);
      } else {
        do {
          writeMask(org, dst, *src++);
          org += dstPitch;
          dst += dstPitch;
          --height;
        } while (--b && height);
      }
    }
  }

  static void fillBits(uint8_t *&src, uint8_t &cl, uint32_t &bits) {
    if (cl <= 8) {
      bits |= *src++ << cl;
      cl += 8;
    }
  }

  static uint8_t readBit(uint8_t &cl, uint8_t &bit, uint32_t &bits) {
    cl--;
    bit = bits & 1;
    bits >>= 1;
    return bit;
  }

  static void writeRoomColor(uint8_t *dst, uint8_t color, const uint8_t *palette) {
    auto c = (color & 0xFF) * 3;
    dst[0] = palette[c];
    dst[1] = palette[c + 1];
    dst[2] = palette[c + 2];
  }

  static void drawStripBasicV(uint8_t code,
                              uint8_t *&src,
                              uint8_t *dst,
                              int dstPitch,
                              int bytesPerPixel,
                              int height,
                              const bool transpCheck,
                              uint8_t *palette) {
    uint8_t decomp_shr = code % 10;
    uint8_t decomp_mask = 0xFF >> (8 - decomp_shr);

    int vertStripNextInc = height * dstPitch - 1 * bytesPerPixel;
    uint8_t color = *src++;
    uint32_t bits = *src++;
    uint8_t cl = 8;
    uint8_t bit;
    int8_t inc = -1;

    int x = 8;
    do {
      int h = height;
      do {
        fillBits(src, cl, bits);
        if (!transpCheck || color != 0xFF)
          writeRoomColor(dst, color, palette);
        dst += dstPitch;
        if (!readBit(cl, bit, bits)) {
        } else if (!readBit(cl, bit, bits)) {
          fillBits(src, cl, bits);
          color = bits & decomp_mask;
          bits >>= decomp_shr;
          cl -= decomp_shr;
          inc = -1;
        } else if (!readBit(cl, bit, bits)) {
          color += inc;
        } else {
          inc = -inc;
          color += inc;
        }
      } while (--h);
      dst -= vertStripNextInc;
    } while (--x);
  }

  static void drawStripBasicH(uint8_t code,
                              uint8_t *&src,
                              uint8_t *dst,
                              int dstPitch,
                              int bytesPerPixel,
                              int height,
                              const bool transpCheck,
                              uint8_t *palette) {
    uint8_t decomp_shr = code % 10;
    uint8_t decomp_mask = 0xFF >> (8 - decomp_shr);
    uint8_t color = *src++;
    uint32_t bits = *src++;
    uint8_t cl = 8;
    uint8_t bit;
    int8_t inc = -1;

    do {
      int x = 8;
      do {
        fillBits(src, cl, bits);
        if (!transpCheck || color != 0xFF)
          writeRoomColor(dst, color, palette);
        dst += bytesPerPixel;
        if (!readBit(cl, bit, bits)) {
        } else if (!readBit(cl, bit, bits)) {
          fillBits(src, cl, bits);
          color = bits & decomp_mask;
          bits >>= decomp_shr;
          cl -= decomp_shr;
          inc = -1;
        } else if (!readBit(cl, bit, bits)) {
          color += inc;
        } else {
          inc = -inc;
          color += inc;
        }
      } while (--x);
      dst += dstPitch - 8 * bytesPerPixel;
    } while (--height);
  }

private:
  std::ifstream m_input;
  XorDecoder m_decoder;
  std::vector<std::shared_ptr<ResNode>> m_nodes;
  std::shared_ptr<ResNode> m_selectedNode;
  std::vector<std::shared_ptr<ngf::Texture>> m_textures;
};

int main(int argc, char **argv) {
  ScummExtractApplication app{};
  app.run();
  return EXIT_SUCCESS;
}