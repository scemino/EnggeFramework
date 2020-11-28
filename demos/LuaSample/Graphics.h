#pragma once

struct Image {
public:
  explicit Image(const std::string& path);

  ngf::Texture &getTexture() { return m_texture; }

private:
  ngf::Texture m_texture;
};

struct Graphics {
  static void clear(float r, float g, float b, float a);
  static void setImage(Image* image);
  static void setColor(float r, float g, float b, float a);
  static void circle(float x, float y, float r, int segments);
  static void polygon(const std::vector<glm::vec2>& vertices);
  static void rectangle(float x, float y, float w, float h);
  static void print(std::string_view str, float x, float y, float ox, float oy, float r, float sx, float sy);
};
