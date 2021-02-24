#pragma once
#include <sol/sol.hpp>
#include <ngf/Application.h>
#include <ngf/Graphics/FntFont.h>
#include <TextEditor.h>
#include "Graphics.h"

class DemoApplication final : public ngf::Application {
public:
  DemoApplication();

  static DemoApplication *getApp();
  std::shared_ptr<ngf::Font> getDefaultFont();
  void setImage(Image* image) { m_image = image; }
  [[nodiscard]] Image* getImage() const { return m_image; }
  void setColor(const ngf::Color &color) { m_color = color; }
  [[nodiscard]] ngf::Color getColor() const { return m_color; }

private:
  void onInit() override;
  void onUpdate(const ngf::TimeSpan &elapsed) override;
  void onRender(ngf::RenderTarget &target) override;
  void onImGuiRender() override;

private:
  ngf::Color m_color{ngf::Colors::White};
  Image* m_image{nullptr};
  std::shared_ptr<ngf::FntFont> m_font;
  sol::state lua;
  TextEditor m_editor;
  static DemoApplication *m_app;
  void registerGraphics();
};