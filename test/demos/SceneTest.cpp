// test for game engine methods. creates a basic scene

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>

#include <engine/BaseEngine.hpp>
#include <engine/Scene.hpp>
#include <engine/RenderContext.hpp>

#include <font/TextFormat.hpp>

#include <engine/EngineContext.hpp>
#include <critter/Empty.hpp>
#include <critter/GameCamera.hpp>
#include <critter/GameObject.hpp>
#include <critter/ui/UIObject.hpp>
#include <critter/ui/UIGroup.hpp>
#include <critter/ui/UIButton.hpp>
#include <critter/ui/FPSCounter.hpp>
#include <critter/Model.hpp>

#include <input/MouseEvent.hpp>

#include <critter/Skybox.hpp>

#include <font/TextObject.hpp>
#include <font/UITextObject.hpp>

#include <shader/light/SpotLight.hpp>

#include <shader/materials/MatteMaterial.hpp>

#include <glm/gtx/euler_angles.hpp>

#include <functional>
#include <sstream>
#include <iomanip>

#define M_PI_ 3.1415926535897932384626
#define M_PI_2_ 1.57079632679489661923

// create everything inline -- its pretty lazy

using ::monkeysworld::engine::Scene;
using ::monkeysworld::engine::RenderContext;
using namespace ::monkeysworld::engine::baseengine;

using namespace std::placeholders;

using ::monkeysworld::font::UITextObject;

using namespace ::monkeysworld::font;


using ::monkeysworld::engine::Context;
using ::monkeysworld::engine::EngineContext;
using ::monkeysworld::critter::Empty;
using ::monkeysworld::critter::GameCamera;
using ::monkeysworld::critter::GameObject;
using ::monkeysworld::critter::Object;
using ::monkeysworld::critter::Model;
using ::monkeysworld::critter::Skybox;

using ::monkeysworld::critter::ui::UIGroup;
using ::monkeysworld::critter::ui::UIObject;
using ::monkeysworld::critter::ui::UIButton;
using ::monkeysworld::critter::ui::FPSCounter;

using ::monkeysworld::critter::camera_info;

using ::monkeysworld::shader::light::SpotLight;

using ::monkeysworld::shader::light::spotlight_info;

using ::monkeysworld::shader::materials::MatteMaterial;

using ::monkeysworld::audio::AudioFiletype;

using ::monkeysworld::font::TextObject;

using ::monkeysworld::input::MouseEvent;

class RatModel2 : public Model {
 public:
  RatModel2(Context* ctx) : Model(ctx), m(ctx) {
    SetMesh(ctx->GetCachedFileLoader()->LoadModel("resources/test/untitled4.obj"));
  }

  void RenderMaterial(const RenderContext& rc) override {
    glm::mat4 tf_matrix = GetTransformationMatrix();
    camera_info cam = rc.GetActiveCamera();
    m.SetSpotlights(rc.GetSpotlights());
    spotlight_info i = rc.GetSpotlights()[0];
    m.SetModelTransforms(tf_matrix);
    m.SetCameraTransforms(cam.vp_matrix);
    m.SetSurfaceColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
    m.UseMaterial();
    Draw();
  }
 private:
  MatteMaterial m;
};

class RatModel : public Model {
 public:
  RatModel(Context* ctx) : Model(ctx), rot_(0), m(ctx) {
    SetMesh(ctx->GetCachedFileLoader()->LoadModel("resources/test/rat/Rat.obj"));
    ctx->GetAudioManager()->AddFileToBuffer("resources/igor.ogg", AudioFiletype::OGG);
    // create a key listener which accomplishes rat motion
    // or just rotate consistently with time
  }

  void Update() override {
    rot_ += static_cast<float>(rot_inc_ * (GetContext()->GetDeltaTime()));
    SetRotation(glm::vec3(0.0, rot_, 0.0));
    auto gc = std::dynamic_pointer_cast<GameCamera>(GetActiveCamera());
  }

  void RenderMaterial(const RenderContext& rc) override {
    glm::mat4 tf_matrix = GetTransformationMatrix();
    camera_info cam = rc.GetActiveCamera();
    // matte material doesn't accept spotlights!
    // TODO: modify material to accept different types of lights
    m.SetSpotlights(rc.GetSpotlights());
    spotlight_info i = rc.GetSpotlights()[0];
    m.SetModelTransforms(tf_matrix);
    m.SetCameraTransforms(cam.vp_matrix);
    m.SetSurfaceColor(glm::vec4(1.0, 0.6, 0.0, 1.0));
    m.UseMaterial();
    Draw();
  }

  
 private:
  const float rot_inc_ = 1.0f;
  float rot_;
  MatteMaterial m;
};

class MovingCamera : public GameCamera {
 public:
  MovingCamera(Context* ctx) : GameCamera(ctx) {
    // create event which moves the camera
    motion_x = 0;
    motion_z = 0;

    rot_x = 0;
    rot_y = 0;

    cursor_cache_ = GetContext()->GetEventManager()->GetCursor()->GetCursorPosition();

    auto click_lambda = [&, this](MouseEvent e) {
      BOOST_LOG_TRIVIAL(trace) << "hello: x=" << e.absolute_pos.x << ", y=" << e.absolute_pos.y;
    };

    uint64_t hi = GetContext()->GetEventManager()->RegisterClickListener(click_lambda);

    // ideally: we always pass the context in instead of relying on *this*
    // note also: the creator is responsible for destroying this event handler or else shit will break
    auto event_lambda = [&, this](int key, int action, int mods) {
      int mod = 0;
      if (action == GLFW_PRESS) {
        mod = 5;
      } else if (action == GLFW_RELEASE) {
        mod = -5;
      }

      switch (key) {
        case GLFW_KEY_S:
          motion_z -= mod;
          break;
        case GLFW_KEY_W:
          motion_z += mod;
          break;
        case GLFW_KEY_A:
          motion_x += mod;
          break;
        case GLFW_KEY_D:
          motion_x -= mod;
      }
    };

    auto rotation_lambda = [&, this](int key, int action, int mods) {
      int mod = 0;
      if (action == GLFW_PRESS) {
        mod = 2;
      } else if (action == GLFW_RELEASE) {
        mod = -2;
      }

      switch (key) {
        case GLFW_KEY_DOWN:
          rot_x -= mod;
          break;
        case GLFW_KEY_UP:
          rot_x += mod;
          break;
        case GLFW_KEY_LEFT:
          rot_y += mod;
          break;
        case GLFW_KEY_RIGHT:
          rot_y -= mod;
      }
    };

    // need a way to register multiple events to a single id.
    w_event = GetContext()->GetEventManager()->RegisterKeyListener(GLFW_KEY_W, event_lambda);
    s_event = GetContext()->GetEventManager()->RegisterKeyListener(GLFW_KEY_S, event_lambda);
    a_event = GetContext()->GetEventManager()->RegisterKeyListener(GLFW_KEY_A, event_lambda);
    d_event = GetContext()->GetEventManager()->RegisterKeyListener(GLFW_KEY_D, event_lambda);
  }

  void Update() override {
    // tba: use cursor position to rotate
    auto w = GetPosition();
    double delta = GetContext()->GetDeltaTime();
    auto r = GetRotation();



    glm::mat4 rotation = glm::eulerAngleYXZ(r.y, r.x, r.z);
    glm::vec4 initial(0, 0, -1, 0);
    glm::vec4 initial_x(-1, 0, 0, 0);
    initial = rotation * initial;
    initial_x = rotation * initial_x;
    glm::dvec2 cur_new = GetContext()->GetEventManager()->GetCursor()->GetCursorPosition();
    SetPosition(w + glm::vec3(initial * static_cast<float>(motion_z * delta)) + glm::vec3(initial_x * static_cast<float>(motion_x * delta)));

    if (!GetContext()->GetEventManager()->GetCursor()->IsCursorLocked()) {
      cursor_cache_ = cur_new;
      return;
    }
    rot_y = r.y - delta * (cur_new.x - cursor_cache_.x) * 1.414f;
    rot_x = r.x - delta * (cur_new.y - cursor_cache_.y) * 1.414f;

    cursor_cache_ = cur_new;

    if (rot_x > M_PI_2_) {
      rot_x = M_PI_2_;
    } else if (rot_x < -M_PI_2_) {
      rot_x = -M_PI_2_;
    }


    SetRotation(glm::vec3(rot_x, rot_y, r.z));
  }

  ~MovingCamera() {
    // call from destroy so we can ensure the context is live
    // though it still should be here
    auto manager = GetContext()->GetEventManager();
    manager->RemoveKeyListener(a_event);
    manager->RemoveKeyListener(s_event);
    manager->RemoveKeyListener(w_event);
    manager->RemoveKeyListener(d_event);
  }
 private:
  uint64_t a_event;
  uint64_t d_event;
  uint64_t s_event;
  uint64_t w_event;

  uint64_t u_event;
  uint64_t do_event;
  uint64_t l_event;
  uint64_t r_event;

  uint64_t click_event;

  float motion_x;
  float motion_z;

  float rot_x;
  float rot_y;

  glm::dvec2 cursor_cache_;
};

#define FRAME_WINDOW 240

class FrameText : public TextObject {
 public:
  FrameText(Context* ctx) : TextObject(ctx, "resources/8bitoperator_jve.ttf") {
    a = 0;
    counter = 0;
    hue = 0;
    TextFormat format;
    format.horiz_align = AlignmentH::CENTER;
    format.char_spacing = 0;
    format.vert_align = AlignmentV::MIDDLE;
    SetTextFormat(format);
    // assume l = 0.5, s = 1 
    for (int i = 0; i < FRAME_WINDOW; i++) {
      frames[i] = 1.0f;
    }
  }
  void Update() override {
    SetRotation(glm::vec3(0, a / 2.5, 0));
    frames[counter++ % FRAME_WINDOW] = GetContext()->GetDeltaTime();
    a += GetContext()->GetDeltaTime();
    float avg = 0.0f;
    for (int i = 0; i < FRAME_WINDOW; i++) {
      avg += frames[i];
    }

    hue += GetContext()->GetDeltaTime() * 60;
    if (hue >= 360.0f) {
      hue -= 360.0f;
    }
    float x = 1 - glm::abs((glm::fract(hue / 120.0) * 2) - 1);
    glm::vec3 res;
    switch(static_cast<int>(glm::floor(hue / 60.0))) {
      case 0:
        res = glm::vec3(1.0, x, 0);
        break;
      case 1:
        res = glm::vec3(x, 1.0, 0);
        break;
      case 2:
        res = glm::vec3(0, 1.0, x);
        break;
      case 3:
        res = glm::vec3(0, x, 1.0);
        break;
      case 4:
        res = glm::vec3(x, 0, 1.0);
        break;
      case 5:
        res = glm::vec3(1.0, 0, x);
        break;
      default:
        res = glm::vec3(0);
    }

    SetTextColor(glm::vec4(res, 1.0));
    avg /= FRAME_WINDOW;
    avg = (1.0 / avg);
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << avg << " fps\n" << a << " sec\nmonkeysworld v0.0.1\n\n";
    SetText(stream.str());
  }
 private:
  float a;
  int counter;
  float frames[FRAME_WINDOW];
  float hue;
};

class DebugText : public UITextObject {
 public:
  DebugText(Context* ctx, const std::string& font_path) : UITextObject(ctx, font_path) {
    SetHorizontalAlign(CENTER);
    SetVerticalAlign(MIDDLE);
    a = 0;
    frame_ctr = 0;
    for (int i = 0; i < 144; i++) {
      fps_buffer[i] = 0.083f;
    }


    auto event_mgr = ctx->GetEventManager();
    event_mgr->GetCursor()->LockCursor();
    event_mgr->RegisterKeyListener(GLFW_KEY_F5, std::bind(&DebugText::keyfunc, this, _1, _2, _3));
    event_mgr->RegisterKeyListener(GLFW_KEY_LEFT_ALT, [=](int a, int b, int c) {
      auto cur = ctx->GetEventManager()->GetCursor();
      if (b != GLFW_PRESS) {
        return;
      }

      if (cur->IsCursorLocked()) {
        cur->UnlockCursor();
      } else {
        cur->LockCursor();
      }
    });

    SetTextColor(glm::vec4(0));
  }

  void keyfunc(int key, int action, int mod) {
    if (action != GLFW_PRESS) {
      return;
    }

    if (GetTextColor().a != 0.0) {
      SetTextColor(glm::vec4(1.0, 0.0, 0.0, 0.0));
    } else {
      SetTextColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
    }
  }


  void Update() override {
    // figure out the text
    frame_ctr++;
    std::stringstream debug_text;
    debug_text << "monkeysworld v0.0.1 (0.0.1 / vanilla)\n";
    a += GetContext()->GetDeltaTime();
    fps_buffer[frame_ctr % 144] = GetContext()->GetDeltaTime();
    if (a > 1.0f) {
      a -= 1.0f;
      float g = 0;
      for (int i = 0; i < 144; i++) {
        g += fps_buffer[i];
      }

      last_fps_polled = 144 / g;
    }
    
    SetOpacity(a);

    debug_text << std::to_string(last_fps_polled) << " FPS\n";

    debug_text << "Standalone client @ " << 6 << "ms ticks\n\n";

    debug_text << "XYZ: " << std::fixed << std::setprecision(4) << camera_pos_.x << " / " << camera_pos_.y << " / " << camera_pos_.z << "\n";
    for (int i = 0; i < 512; i++) {
      auto cur = GetContext()->GetEventManager()->GetCursor()->GetCursorPosition();
    }
    
    auto cur = GetContext()->GetEventManager()->GetCursor()->GetCursorPosition();
    debug_text << "looking at -- X:" << cur.x << " Y:" << cur.y << "\n";
    SetText(debug_text.str());
    // Invalidate();
  }

  void RenderMaterial(const RenderContext& rc) {
    camera_pos_ = rc.GetActiveCamera().position;
    UITextObject::RenderMaterial(rc);
  }

 private:
  float a;
  float fps_buffer[144];

  int last_fps_polled;
  int frame_ctr;

  glm::vec3 camera_pos_;


};

/**
 *  Simple test scene
 */ 
class TestScene : public Scene {
 public:
  TestScene() {

  }

  std::string GetSceneIdentifier() override {
    return "new-scene";
  }

  void Initialize(Context* ctx) override {
    // USE THE RAT! https://sketchfab.com/3d-models/rat-847629266c0f442da74fb132f46f3baf
    auto cam = std::make_shared<MovingCamera>(ctx);
    GetGameObjectRoot()->AddChild(cam);
    cam->SetPosition(glm::vec3(0, 0, -5));
    cam->SetRotation(glm::vec3(0, 3.14, 0));
    cam->SetFov(60.0f);
    cam->SetActive(true);
    auto rat = std::make_shared<RatModel>(ctx);
    rat->SetPosition(glm::vec3(0, 0, 3));
    GetGameObjectRoot()->AddChild(rat);

    auto light = std::make_shared<SpotLight>(ctx);
    light->SetPosition(glm::vec3(1, 4, -2));
    light->SetDiffuseIntensity(1.0);
    GetGameObjectRoot()->AddChild(light);

    auto rat_two = std::make_shared<RatModel2>(ctx);
    rat_two->SetScale(glm::vec3(0.5, 0.5, 0.5));
    rat_two->SetPosition(glm::vec3(0, 0, -1));

    auto t = std::make_shared<FrameText>(ctx);
    t->SetTextColor(glm::vec4(1.0, 0.5, 1.0, 1.0));
    t->SetTextSize(384.0f);
    t->SetPosition(glm::vec3(2, 0, 0));
    rat->AddChild(t);
    
    auto w = std::make_shared<Skybox>(ctx);
    std::string s = "resources/test/texturetest.png";
    auto res = ctx->GetCachedFileLoader()->LoadCubeMap(s, s, s, s, s, s);
    w->SetCubeMap(res);
    GetGameObjectRoot()->AddChild(w);
    t->AddChild(rat_two);

    auto tui_twoey = std::make_shared<DebugText>(ctx, "resources/8bitoperator_jve.ttf");
    tui_twoey->SetPosition(glm::vec2(100, 100));
    tui_twoey->SetDimensions(glm::vec2(800, 600));
    tui_twoey->SetTextSize(32.0f);

    auto tui = std::make_shared<UITextObject>(ctx, "resources/8bitoperator_jve.ttf");
    tui->SetPosition(glm::vec2(600, 600));
    tui->SetDimensions(glm::vec2(800, 400));
    tui->SetTextSize(32.0f);
    tui->SetTextColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
    tui->SetText("hello\nspongebob\ncleveland brown\ncomedy show");
    tui->SetOpacity(0.8f);
    BOOST_LOG_TRIVIAL(trace) << "new opac: " << tui->GetOpacity();

    auto but = std::make_shared<UIButton>(ctx, "resources/8bitoperator_jve.ttf");
    but->SetPosition(glm::vec2(300, 300));
    but->SetDimensions(glm::vec2(200, 70));
    but->border_radius = 5.0f;
    but->border_width = 2.0f;
    but->SetText("click me!");
    but->SetTextSize(64.0f);
    but->SetHorizontalAlign(CENTER);
    but->SetVerticalAlign(MIDDLE);

    auto counter = std::make_shared<FPSCounter>(ctx, "resources/8bitoperator_jve.ttf");
    counter->SetPosition(glm::vec2(5));
    counter->SetDimensions(glm::vec2(300, 60));
    counter->SetTextSize(48.0f);

    auto group = std::make_shared<UIGroup>(ctx);
    group->SetPosition(glm::vec2(0, 0));
    group->SetDimensions(glm::vec2(1600, 900));
    // create a group and add the text to that
    group->AddChild(tui);
    group->AddChild(tui_twoey);
    group->AddChild(but);
    group->AddChild(counter);
    GetWindow()->AddChild(group);
  }
 private:
  Context* ctx;
};

int main(int argc, char** argv) {
  GLFWwindow* main_win = InitializeGLFW(1280, 720, "and he never stoped playing, he always was keep beliving");
  // give the context ownership of the scene!
  auto scene = new TestScene();
  {
    auto ctx = std::make_shared<EngineContext>(main_win, scene);
    #ifdef DEBUG
    ctx->GetEventManager()->RegisterKeyListener(GLFW_KEY_Z, [&](int k, int a, int m) {
      if (a == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }

      if (a == GLFW_RELEASE) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
    });
    #endif
    while (true) {
      auto prog = ctx->GetCachedFileLoader()->GetLoaderProgress();
      BOOST_LOG_TRIVIAL(trace) << "loading progress: " << (((float)prog.bytes_read * 100.0f) / prog.bytes_sum);
      if (prog.bytes_read == prog.bytes_sum) {
        break;
      }
      
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    GameLoop(ctx, main_win);
  }
  glfwDestroyWindow(main_win);
  glfwTerminate();
  return 0;
}
