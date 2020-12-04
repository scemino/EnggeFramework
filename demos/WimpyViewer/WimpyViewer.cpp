#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/RectangleShape.h>
#include <ngf/IO/MemoryStream.h>
#include <imgui.h>
#include <utility>
#include "SpriteSheetItem.h"
#include "Camera.h"
#include "Room.h"
#include "RoomEditor.h"
#include "CameraControl.h"
#include "Base85.h"

const char *handle_green =
    R"(=pxq9_8v7&0C/2'K(HH=4hFJ(4hFJ(15###i><u9t%###1EHr9809Gf)9+##=<Pt8WcGP(9U&%;>GnZcY&px%t?kj;`=T'=M`qGil1vV$k.bs-Q%EI-r^@v,As:p/Ta8OPlUaW-(@Ah3qRoe`8V$t+mnvCn_,qc5$.O]Jp#47INZ8FiBR7cdOA(YdjOYo6O)*5.?'Xa)ZP95TwHLc'8;hJ6>N%3vTj.In4BFK:oD$>b=A+,10OtlHWn@wjH'^&ar$ST(FXI:Mb];J.4S)M%EV?Z4ncJsXml_M]*_nu87Ha0e&j5CR'hpJ<Jf<bFRe(hBXY;,5^RQ.-Ya'o.SrgW7isjdn;h)%'Hx+@Jq$^B$CU)wQ$0tYN5Cr_(KYveN+hojDx0'a5exZu5OT:Fep@YAmCrqF'pj_RO#x8I7M*wBd1Ju9$IX@5>5AiCY2>P2W<aB;d9<Hicob'g[$.84)>nbP0bO.h++$05/QkZJOav+IdOmRG<`&X7aBO1SPT2)]pB's_$%(dgInSa^`w9Q@)r>m0#,e0&$7:%2#/6?sC0Q/VZOG0A7Gm2%*h$###Ub90<d$bsAP$###)";

const char *handle_red =
    R"(=pxq9_8v7&0C/2'K(HH=4hFJ(4hFJ(15###i><u9t%###1EHr9809Gf)9+##+[Ot8WcGP(+VQ#;'Lp*NVB7V)3f0u..f@O8ND.Bcrlu*6&[_psN-R6(&cw;o3/Pu7quSGQ(Z4L?2n;_ichP]4[G2-GSFb<nfaZ+v0#su07jxeRY[tvf_,*5Hd0f(m8`WV?H7*CbIxCEn-<HSXI8S_pHXJ,$3]O`nYN3s_O?slL8RJLIO9D,W>PR3_;Mc.']Pl;8cq[t.ZMYRKb]--l0U:,s`nMI-4X'vp#($%^?<[ac)aS6bYXMs'>;T8<%0oYVDM.jOp8I]C7i36XL;L^bN4L):0d0E[^n_A)q<Bk++M-T;N4&q__)b<54dAQX(<f)%dRq)M8(Ebp1_doE6B3L_l1Rh@ReEg,H;h2q4`L])>;6[I9[n<P*jG.^qEF>QJpm;JPp<P(s6t?'[q(r>1#I3JoJ'E3+0DQe'QKpfe-t.@-+T3-['K<%1>ew.ZTf)q:jrR&<uG9eL]Y&F205^aMrwE1V_TDj6:FHCwtQ##mb:J:T%f#[HVq0#)";

const char *handle_blue =
    R"(=pxq9_8v7&0C/2'K(HH=4hFJ(4hFJ(15###i><u9t%###1EHr9809Gf)9+##/hOt8WcGP(EH>&;QMp*N$A7Tcb$SdP>W%wWe/J:M>9Y3:4&sC$]u8IO0psS[qDWN.HD;V.aE*RiTTML2'5XM@8M>EnJCrp3ZOnshfJ<xt4`<iW--9A.'0@,$Qma/mE+=S'+)='2<g&B?)-,'?C<QVgH2t;Ks50D+HMnh9$(3HoIV>U2Lfd<g>dp$m9j0IV>in.1aFIxR^6#DlT^+_,i_?TFWuNkhr5;/Rm*NkG8x3d`o@L.ZYB4>s>MR.NE>Cj-k@(/dTl]Hh&`VcF9HEJh#>J2.ocog>B]/6r%B:<`w11KhfunA#5awt),LdKb5g#76ge`n&)Kp`nb56)3G_P.++j%`(W$Gvlb4mm1Q&KKN4+k:bQqk5nm+k$Yv@:0&Wj/ffk>+gi-)YW)0KS0L%2f6L?MEfmuSJiWD:WUIR+E7Q:(,bKHj>6$[(RNQ9IE<p$ow'ioE8cnwdtR.S<%qfQ1Jlunu0nXAo[_kRLe=7s&e0#mb:J:T%f#[HVq0#)";

class WimpyViewerApplication final : public ngf::Application {
public:
  explicit WimpyViewerApplication(std::filesystem::path path)
      : m_path(std::move(path)), m_roomEditor(*this, m_room), m_cameraControl(m_room) {
    m_roomEditor.onSelectedObjectChanged([](auto pObj) {
      if (pObj)
        pObj->pause();
    });
  }

private:
  void onInit() override {
    m_window.init({"WimpyViewer", {1024, 768}});
    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    m_room.loadRoom(m_path);

    m_greenHandle = decodeTexture(handle_green);
    m_redHandle = decodeTexture(handle_red);
    m_blueHandle = decodeTexture(handle_blue);
  }

  static std::shared_ptr<ngf::Texture> decodeTexture(const char *src) {
    auto data = ngf::Base85::decode(src);
    auto texture = std::make_shared<ngf::Texture>();
    ngf::MemoryStream ms(data.data(), data.data() + data.size());
    texture->loadFromStream(ms);
    return texture;
  }

  void onEvent(ngf::Event &event) override {
    switch (event.type) {
    case ngf::EventType::MouseMoved: {
      m_mousePos = event.mouseMoved.position;
      const auto scale = ngf::Window::getDpiScale();
      m_worldPos = getRenderTarget()->mapPixelToCoords(m_mousePos) * scale;
      m_worldPos.y = m_room.getSize().y - m_worldPos.y;
      m_worldPos += m_room.getCamera().position;
      if (m_walkboxVertex.walkbox) {
        auto it = m_walkboxVertex.walkbox->begin() + m_walkboxVertex.vertexIndex;
        (*it) = m_worldPos;
      } else if (m_objectHitTest.pObj) {
        switch (m_objectHitTest.hit) {
        case ObjectHit::Position:m_objectHitTest.pObj->pos = {m_worldPos.x, m_room.getSize().y - m_worldPos.y};
          break;
        case ObjectHit::UsePosition:
          m_objectHitTest.pObj->usePos =
              {m_worldPos.x - m_objectHitTest.pObj->pos.x,
               m_worldPos.y - m_room.getSize().y + m_objectHitTest.pObj->pos.y};
          break;
        case ObjectHit::Hotspot: {
          auto delta = m_worldPos - m_objectHitTest.startPos;
          auto hotspot = m_objectHitTest.startHotspot;
          m_objectHitTest.pObj->hotspot =
              ngf::irect::fromPositionSize({hotspot.getPosition().x + delta.x, hotspot.getPosition().y - delta.y},
                                           hotspot.getSize());
        }
          break;
        case ObjectHit::HotspotVertex: {
          auto delta = m_worldPos - m_objectHitTest.startPos;
          auto hotspot = m_objectHitTest.startHotspot;
          switch (m_objectHitTest.vertexIndex) {
          case 0:hotspot.min.x += delta.x;
            hotspot.min.y -= delta.y;
            break;
          case 1:hotspot.max.x += delta.x;
            hotspot.min.y -= delta.y;
            break;
          case 2:hotspot.max.x += delta.x;
            hotspot.max.y -= delta.y;
            break;
          case 3:hotspot.min.x += delta.x;
            hotspot.max.y -= delta.y;
            break;
          }
          m_objectHitTest.pObj->hotspot = hotspot;
        }
          break;
        case ObjectHit::Zsort: {
          auto delta = m_worldPos - m_objectHitTest.startPos;
          auto zsort = m_objectHitTest.startZsort;
          m_objectHitTest.pObj->zsort = zsort + delta.y;
        }
          break;
        }
      }
    }
      break;
    case ngf::EventType::MouseButtonPressed: {
      if (m_cameraControl.isPanOrZoomEnabled())
        break;

      // test if we selected a walkbox
      auto pWalkbox = getWalkboxAt(m_worldPos);
      if (pWalkbox) {
        m_roomEditor.setSelectedWalkbox(pWalkbox);
      }
      // test if we are near a walkbox vertex
      m_walkboxVertex = getWalkboxVertexAt(m_worldPos);

      // test if mouse right button is pressed near a walkbox
      if (event.mouseButton.button == 3 && !m_walkboxVertex.walkbox) {
        for (auto &wb : m_room.walkboxes()) {
          auto edgePoint = wb.getClosestPointOnEdge(m_worldPos);
          if (glm::distance(edgePoint, m_worldPos) < 5.f) {
            // yes, so add a vertex in this walkbox
            auto index = getIndexWhereToInsertPoint(wb, edgePoint);
            auto it = wb.cbegin() + index;
            wb.insert(it, edgePoint);
            break;
          }
        }
      }

      // test if we are near an object: position, hotspot or zsort
      auto pObj = m_roomEditor.getSelectedObject();
      if (pObj)
        objectHitTest(pObj);
    }
      break;
    case ngf::EventType::MouseButtonReleased: {
      m_walkboxVertex.walkbox = nullptr;
      m_objectHitTest.pObj = nullptr;
    }
      break;
    case ngf::EventType::KeyPressed: {
      auto pObj = m_roomEditor.getSelectedObject();
      if (pObj && !pObj->animations.empty() && event.key.scancode >= ngf::Scancode::D1
          && event.key.scancode <= ngf::Scancode::D0) {
        auto digit = static_cast<int>(event.key.scancode) - static_cast<int>(ngf::Scancode::D1);
        if (digit < pObj->animations.size()) {
          pObj->animationIndex = digit;
          pObj->play();
        }
      }
    }
      break;
    default:break;
    }
    m_cameraControl.onEvent(*this, event);
  }

  void objectHitTest(Object *pObj) {
    assert(pObj);

    m_objectHitTest.hit = ObjectHit::None;
    m_objectHitTest.startPos = m_worldPos;
    m_objectHitTest.startHotspot = pObj->hotspot;
    m_objectHitTest.startZsort = pObj->zsort;
    ObjectHit hit;
    ngf::frect rect;

    // if this a prop, we want to modify the position of the object
    if (pObj->type == ObjectType::Prop) {
      hit = ObjectHit::Position;
      rect = ngf::frect::fromCenterSize(
          {pObj->pos.x, m_room.getSize().y - pObj->pos.y},
          {5, 5});
    } else if (pObj->type != ObjectType::Trigger) {
      // else we want to modify the use position of the object (except for trigger)
      hit = ObjectHit::UsePosition;
      rect = ngf::frect::fromCenterSize(
          {pObj->pos.x + pObj->usePos.x,
           m_room.getSize().y - (pObj->pos.y - pObj->usePos.y)},
          {5, 5});
    }

    if (rect.contains(m_worldPos)) {
      m_objectHitTest.pObj = pObj;
      m_objectHitTest.hit = hit;
      return;
    }

    // test if mouse is over hotspot vertex
    glm::ivec2 pos = {pObj->pos.x, m_room.getSize().y - pObj->pos.y};
    auto hs = ngf::irect::fromMinMax({pObj->hotspot.min.x, -pObj->hotspot.max.y},
                                     {pObj->hotspot.max.x, -pObj->hotspot.min.y});
    auto hotspot = ngf::frect::fromPositionSize(hs.getPosition() + pos, hs.getSize());
    std::array<glm::vec2, 4>
        positions = {hotspot.getBottomLeft(), hotspot.getBottomRight(),
                     hotspot.getTopRight(), hotspot.getTopLeft()};
    for (auto i = 0; i < 4; i++) {
      auto vertexPos = positions[i];
      if (glm::distance(vertexPos, m_worldPos) < 5.f) {
        m_objectHitTest.pObj = pObj;
        m_objectHitTest.hit = ObjectHit::HotspotVertex;
        m_objectHitTest.vertexIndex = i;
        return;
      }
    }

    // test if mouse is over object's hotspot
    if (hotspot.contains(m_worldPos)) {
      m_objectHitTest.pObj = pObj;
      m_objectHitTest.hit = ObjectHit::Hotspot;
      return;
    }

    // test if mouse is over object's zsort
    if (fabs(m_worldPos.y - static_cast<float>(pObj->zsort)) < 5.f) {
      m_objectHitTest.pObj = pObj;
      m_objectHitTest.hit = ObjectHit::Zsort;
      return;
    }
  }

  void onUpdate(const ngf::TimeSpan &elapsed) override {
    m_room.update(elapsed);
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(m_roomEditor.getClearColor());

    // draw room
    m_room.draw(target, {});

    drawWalkboxes(target);

    // draw position, zsort line and hotspot if an object is selected
    auto selectedObject = m_roomEditor.getSelectedObject();
    if (selectedObject) {
      drawPosition(target, *selectedObject);
      drawZSort(target, *selectedObject);
      drawHotspot(target, *selectedObject);
    }

    Application::onRender(target);
  }

  void drawWalkboxes(ngf::RenderTarget &target) {
    const auto screenSize = m_room.getScreenSize();
    const auto offsetY = screenSize.y - m_room.getSize().y;
    const auto &camera = m_room.getCamera();
    ngf::RenderStates states;
    ngf::Transform t;
    t.setPosition({-camera.position.x, camera.position.y + offsetY});
    states.transform *= t.getTransform();
    for (const auto &walkbox: m_room.walkboxes()) {
      if (!walkbox.isVisible())
        continue;

      const auto color = m_roomEditor.getSelectedWalkbox() == &walkbox ? ngf::Colors::LimeGreen : ngf::Colors::Green;
      std::vector<ngf::Vertex> polygon;
      std::transform(walkbox.cbegin(), walkbox.cend(), std::back_inserter(polygon), [this, color](const auto &p) {
        return ngf::Vertex{{p.x, m_room.getSize().y - p.y}, color};
      });
      target.draw(ngf::PrimitiveType::LineLoop, polygon, states);
    }

    auto pSelectedWalkbox = m_roomEditor.getSelectedWalkbox();
    if (pSelectedWalkbox) {
      auto handle = createHandle(*m_greenHandle);
      for (auto it = pSelectedWalkbox->cbegin(); it != pSelectedWalkbox->cend(); it++) {
        glm::vec2 pos(it->x, m_room.getSize().y - it->y);
        handle.getTransform().setPosition(pos);
        handle.draw(target, states);
      }
    }
  }

  static int getIndexWhereToInsertPoint(const ngf::Walkbox &wb, glm::ivec2 edgePoint) {
    std::vector<float> dists(wb.size());
    for (int i = 0; i < wb.size(); i++) {
      dists[i] = ngf::Walkbox::distanceToSegment(edgePoint, wb.at(i), wb.at((i + 1) % wb.size()));
    }
    auto dIt = std::min_element(dists.cbegin(), dists.cend());
    return static_cast<int>((std::distance(dists.cbegin(), dIt) + 1) % wb.size());
  }

  static ngf::RectangleShape createHandle(const ngf::Texture &texture) {
    ngf::RectangleShape handle;
    handle.setSize({5, 5});
    handle.setAnchor(ngf::Anchor::Center);
    handle.setTexture(texture, false);
    return handle;
  }

  std::shared_ptr<ngf::Texture> getTexture(ObjectType type) const {
    switch (type) {
    case ObjectType::Prop: return m_blueHandle;
    case ObjectType::Spot:return m_greenHandle;
    case ObjectType::Trigger:
    case ObjectType::None:return m_redHandle;
    default:assert(false);
    }
  }

  ngf::RectangleShape createHandle(ObjectType type) const {
    return createHandle(*getTexture(type));
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Mouse pos (%d,%d)", m_mousePos.x, m_mousePos.y);
    ImGui::Text("World pos (%.f,%.f)", m_worldPos.x, m_worldPos.y);

    ImGui::DragFloat2("Position", glm::value_ptr(m_room.getCamera().position));
    ImGui::DragFloat("Zoom", &m_room.getCamera().zoom, 0.1f, 0.1f, 10.f);

    ImGui::End();

    m_roomEditor.draw();
  }

  ngf::Walkbox *getWalkboxAt(glm::vec2 pos) {
    for (auto &wb : m_room.walkboxes()) {
      if (wb.inside(pos)) {
        return &wb;
      }
    }
    return nullptr;
  }

  struct WalkboxVertexHitTest {
  public:
    ngf::Walkbox *walkbox{nullptr};
    int vertexIndex{-1};
  };

  enum class ObjectHit {
    None,
    Position,
    UsePosition,
    Hotspot,
    HotspotVertex,
    Zsort
  };

  struct ObjectHitTest {
  public:
    Object *pObj{nullptr};
    ObjectHit hit{ObjectHit::None};
    glm::vec2 startPos;
    ngf::irect startHotspot;
    int vertexIndex{0};
    int startZsort{0};
  };

  WalkboxVertexHitTest getWalkboxVertexAt(glm::vec2 pos) {
    for (auto &wb : m_room.walkboxes()) {
      for (auto it = wb.cbegin(); it != wb.cend(); it++) {
        auto rect = ngf::frect::fromCenterSize(*it, glm::vec2{5, 5});
        if (rect.contains(pos)) {
          const auto index = static_cast<int>(std::distance(wb.cbegin(), it));
          return {&wb, index};
        }
      }
    }
    return {};
  }

  [[nodiscard]] static ngf::Color getColor(ObjectType type) {
    switch (type) {
    case ObjectType::Prop:return ngf::Colors::Blue;
    case ObjectType::Spot:return ngf::Colors::Green;
    case ObjectType::Trigger:return ngf::Colors::Orange;
    default:return ngf::Colors::Red;
    }
  }

  void drawPosition(ngf::RenderTarget &target, const Object &object) const {
    if (object.type == ObjectType::Trigger)
      return;

    // draw position
    auto color = getColor(object.type);

    constexpr float length = 8.f;
    constexpr float length_2 = length / 2.f;
    constexpr float length_4 = length / 4.f;
    std::array<ngf::Vertex, 4> posVertices = {
        ngf::Vertex{{-length_2, 0.f}, color},
        ngf::Vertex{{length_2, 0.f}, color},
        ngf::Vertex{{0.f, -length_2}, color},
        ngf::Vertex{{0.f, length_2}, color}};

    ngf::RenderStates states;
    ngf::Transform tObj;
    if (object.type == ObjectType::Prop) {
      tObj.setPosition(object.pos);
    } else {
      tObj.setPosition({object.pos.x + object.usePos.x, object.pos.y - object.usePos.y});
    }
    states.transform = tObj.getTransform();
    target.draw(ngf::PrimitiveType::Lines, posVertices, states);

    if (object.useDir != Direction::None) {
      // draw direction arrow
      std::array<ngf::Vertex, 2> dirVertices;
      switch (object.useDir) {
      case Direction::Back:dirVertices[0] = {{-length_4, -length_4}};
        dirVertices[1] = {{length_4, -length_4}};
        break;
      case Direction::Front:dirVertices[0] = {{-length_4, length_4}};
        dirVertices[1] = {{length_4, length_4}};
        break;
      case Direction::Left:dirVertices[0] = {{-length_4, -length_4}};
        dirVertices[1] = {{-length_4, length_4}};
        break;
      case Direction::Right:dirVertices[0] = {{length_4, -length_4}};
        dirVertices[1] = {{length_4, length_4}};
        break;
      default:break;
      }
      dirVertices[0].color = color;
      dirVertices[1].color = color;
      target.draw(ngf::PrimitiveType::Lines, dirVertices, states);

      auto handle = createHandle(object.type);
      handle.draw(target, states);
    }
  }

  void drawHotspot(ngf::RenderTarget &target, const Object &object) const {
    if (object.type != ObjectType::None && object.type != ObjectType::Trigger)
      return;

    ngf::Transform tObj;
    tObj.setPosition(object.pos);

    auto color = getColor(object.type);
    std::array<ngf::Vertex, 4> hotspotVertices = {
        ngf::Vertex{object.hotspot.getTopLeft(), color},
        ngf::Vertex{object.hotspot.getBottomLeft(), color},
        ngf::Vertex{object.hotspot.getBottomRight(), color},
        ngf::Vertex{object.hotspot.getTopRight(), color}};

    // draw hotspot and position
    ngf::RenderStates states;
    states.transform = tObj.getTransform();
    target.draw(ngf::PrimitiveType::LineLoop, hotspotVertices, states);

    // draw handle on each vertex
    auto handle = createHandle(object.type);
    for (auto &vertex : hotspotVertices) {
      handle.getTransform().setPosition(vertex.pos);
      handle.draw(target, states);
    }
  }

  void drawZSort(ngf::RenderTarget &target, const Object &object) const {
    const auto color = getColor(object.type);
    const auto y = m_room.getSize().y - object.zsort;
    std::array<ngf::Vertex, 2> vertices;
    vertices[0] = {{0, y}, color};
    vertices[1] = {{target.getSize().x, y}, color};
    target.draw(ngf::PrimitiveType::Lines, vertices);

    auto handle = createHandle(object.type);
    handle.getTransform().setPosition({5.f, y});
    handle.draw(target, {});

    handle.getTransform().setPosition({target.getSize().x - 5.f, y});
    handle.draw(target, {});
  }

private:
  std::shared_ptr<ngf::Texture> m_greenHandle;
  std::shared_ptr<ngf::Texture> m_redHandle;
  std::shared_ptr<ngf::Texture> m_blueHandle;
  std::filesystem::path m_path;
  Room m_room;
  glm::ivec2 m_mousePos{};
  glm::vec2 m_worldPos{};
  RoomEditor m_roomEditor;
  CameraControl m_cameraControl;
  WalkboxVertexHitTest m_walkboxVertex;
  ObjectHitTest m_objectHitTest;
};

int main(int argc, char **argv) {
  if (argc >= 2) {
    WimpyViewerApplication app{argv[1]};
    app.run();
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}