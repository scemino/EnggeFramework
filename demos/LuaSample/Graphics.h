#pragma once

struct Image {
public:
  explicit Image(const std::string& path);

  ngf::Texture &getTexture() { return m_texture; }

private:
  ngf::Texture m_texture;
};

struct Graphics {
  static void clear();
  static void clearRgba(float r, float g, float b, float a);
  static void setImage(Image* image);
  static void setColor(float r, float g, float b, float a);
  static void drawCircle(float x, float y, float r);
  static void print(std::string_view text,
                    float x = 0,
                    float y = 0
  );
};
