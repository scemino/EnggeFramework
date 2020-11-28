#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Colors.h>
#include <ngf/Graphics/CircleShape.h>
#include <ngf/Graphics/Text.h>
#include "Graphics.h"
#include "DemoApplication.h"

Image::Image(const std::string& path) {
  m_texture.load(path);
}

void Graphics::clear() {
  DemoApplication::getApp()->getRenderTarget()->clear(ngf::Colors::Black);
}

void Graphics::clearRgba(float r, float g, float b, float a) {
  DemoApplication::getApp()->getRenderTarget()->clear(ngf::Color(r, g, b, a));
}

void Graphics::setImage(Image* image){
  DemoApplication::getApp()->setImage(image);
}

void Graphics::setColor(float r, float g, float b, float a) {
  DemoApplication::getApp()->setColor(ngf::Color(r, g, b, a));
}

void Graphics::drawCircle(float x, float y, float r) {
  ngf::CircleShape shape(r);
  shape.getTransform().setPosition({x, y});
  auto img = DemoApplication::getApp()->getImage();
  if (img) {
    shape.setTexture(img->getTexture(), true);
  }
  shape.setColor(DemoApplication::getApp()->getColor());
  shape.draw(*DemoApplication::getApp()->getRenderTarget(), {});
}

void Graphics::print(std::string_view str, float x, float y) {
  auto font = DemoApplication::getApp()->getDefaultFont();
  ngf::Text text;
  auto &trsf = text.getTransform();
  text.setColor(DemoApplication::getApp()->getColor());
  text.setFont(*font);
  text.setCharacterSize(13);
  text.setString(str.data());
  trsf.setPosition({x, y});
  //trsf.setOrientation({ox, oy});
  //trsf.setRotation(r);
  //trsf.setScale({sx, sy});
  text.draw(*DemoApplication::getApp()->getRenderTarget(), {});
}

