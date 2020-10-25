#pragma once
#include <vector>
#include <list>
#include <map>
#include <ngf/Graphics/Font.hpp>

namespace ngf {
// original code from:
// - http://www.angelcode.com/products/bmfont/
// - https://github.com/Zylann/zCraft/blob/master/src/engine/bmfont/Font.cpp
// -
// https://www.gamedev.net/forums/topic/330742-quick-tutorial-variable-width-bitmap-fonts/
// the code has been adapted to work with SFML
struct FontInfo {
  struct Padding {
    int up;
    int right;
    int down;
    int left;

    Padding() : up(0), right(0), down(0), left(0) {
    }
  };

  struct Spacing {
    int horizontal;
    int vertical;

    Spacing() : horizontal(0), vertical(0) {
    }
  };

  unsigned int size;     // The size of the true type font.
  bool bold;             // The font is bold.
  bool italic;           // The font is italic.
  bool unicode;          // Set to 1 if it is the unicode charset.
  unsigned int stretchH; // The font height stretch in percentage. 100% means
  // no stretch.
  bool smooth;           // Set to 1 if smoothing was turned on.
  unsigned int
      aa; // The supersampling level used. 1 means no supersampling was used.
  unsigned int outline; // The outline thickness for the characters.

  FontInfo()
      : size(0), bold(false), italic(false), unicode(false), stretchH(0),
        smooth(false), aa(1), outline(0) {
  }
};

struct Kerning {
  short first;  // The first character id.
  short second; // The second character id.
  short amount;

  Kerning() : first(0), second(0), amount(0) {
  }
};

class CharSet {
public:
  void addKerning(Kerning k);
  [[nodiscard]] short getKerning(int first, int second) const;

  void addChar(int id, Glyph &cd);
  [[nodiscard]] const Glyph &getChar(int id) const;

  std::vector<std::string> pages; // [id] = file

  // This is the distance in pixels between each line of text.
  unsigned short lineHeight;

  // The number of pixels from the absolute top of the line to the
  // base of the characters.
  unsigned short base;

  // The size of the texture, normally used to scale the coordinates of
  // the character image.
  unsigned short scaleW;
  unsigned short scaleH;

  // Set to 1 if the monochrome characters have been packed into each of
  // the texture channels. In this case alphaChnl describes what is
  // stored in each channel.
  unsigned short packed;

  // For each color :
  // Set to 0 if the channel holds the glyph data, 1 if it holds the
  // outline, 2 if it holds the glyph and the outline, 3 if its set to
  // zero, and 4 if its set to one.
  unsigned short alphaChnl;
  unsigned short redChnl;
  unsigned short greenChnl;
  unsigned short blueChnl;

  CharSet()
      : lineHeight(0), base(0), scaleW(0), scaleH(0), packed(false),
        alphaChnl(0), redChnl(4), greenChnl(4), blueChnl(4) {
  }

private:
  typedef std::map<int, Glyph>
      GlyphTable; ///< Table mapping a codepoint to its glyph
  GlyphTable m_chars;
  std::list<Kerning> m_kernings;
};

class FntFont : public Font {
private:
  CharSet m_chars;
  std::vector<Texture> m_textures;

public:
  ~FntFont() override;
  void load(const std::string &name, std::istream &input);
  void loadFromFile(const std::string &path);

  [[nodiscard]] const Glyph &getGlyph(unsigned int codePoint) const override;
  [[nodiscard]] float getKerning(unsigned int first, unsigned int second,
                                 unsigned int characterSize) const override;
  [[nodiscard]] const Texture &getTexture(
      unsigned int characterSize) const override;

private:
  bool parse(const std::string &path, std::istream &input);
};
}
