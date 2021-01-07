// test for game engine methods. creates a basic scene

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>

#include <engine/BaseEngine.hpp>
#include <engine/Scene.hpp>
#include <engine/RenderContext.hpp>

#include <engine/Context.hpp>
#include <critter/Empty.hpp>
#include <critter/GameCamera.hpp>
#include <critter/GameObject.hpp>
#include <critter/Model.hpp>

#include <font/TextObject.hpp>

#include <shader/light/SpotLight.hpp>

#include <shader/materials/MatteMaterial.hpp>

// create everything inline -- its pretty lazy

using ::monkeysworld::engine::Scene;
using ::monkeysworld::engine::RenderContext;
using namespace ::monkeysworld::engine::baseengine;

using ::monkeysworld::engine::Context;
using ::monkeysworld::critter::Empty;
using ::monkeysworld::critter::GameCamera;
using ::monkeysworld::critter::GameObject;
using ::monkeysworld::critter::Object;
using ::monkeysworld::critter::Model;

using ::monkeysworld::critter::camera_info;

using ::monkeysworld::shader::light::SpotLight;

using ::monkeysworld::shader::light::spotlight_info;

using ::monkeysworld::shader::materials::MatteMaterial;

using ::monkeysworld::audio::AudioFiletype;

using ::monkeysworld::font::TextObject;

class RatModel : public Model {
 public:
  RatModel(Context* ctx) : Model(ctx), rot_(0), m(ctx) {
    SetMesh(Model::FromObjFile(ctx, "resources/test/rat/Rat.obj"));
    ctx->GetAudioManager()->AddFileToBuffer("resources/chamberofreflection.ogg", AudioFiletype::OGG);
    // create a key listener which accomplishes rat motion
    // or just rotate consistently with time
  }

  void Update() override {
    rot_ += rot_inc_ * (GetContext()->GetDeltaTime());
    SetRotation(glm::vec3(0.0, rot_, 0.0));
    auto gc = std::dynamic_pointer_cast<GameCamera>(GetActiveCamera());
    gc->SetRotation(glm::vec3(0, 3.14, 0));
  }

  void RenderMaterial(const RenderContext& rc) override {
    glm::mat4 tf_matrix = GetTransformationMatrix();
    camera_info cam = rc.GetActiveCamera();
    // matte material doesn't accept spotlights!
    // TODO: modify material to accept different types of lights
    m.SetSpotlights(rc.GetSpotlights());
    spotlight_info i = rc.GetSpotlights()[0];
    m.SetModelTransforms(tf_matrix);
    m.SetCameraTransforms(cam.view_matrix);
    m.SetSurfaceColor(glm::vec4(1.0, 0.6, 0.0, 1.0));
    m.UseMaterial();
    Draw();
  }

  
 private:
  const float rot_inc_ = 1.0f;
  float rot_;
  MatteMaterial m;
};

class FrameText : public TextObject {
 public:
  FrameText(Context* ctx) : TextObject(ctx, "resources/montserrat-light.ttf") { a = 0; }
  void Update() override {
    a += GetContext()->GetDeltaTime();
    SetText(std::to_string(a));
    SetRotation(glm::vec3(0, a / 2.5, 0));
  }
 private:
  float a;
};

/**
 *  Simple test scene
 */ 
class TestScene : public Scene {
 public:
  TestScene(Context* ctx) {
    this->ctx = ctx;
  }
  void Initialize() override {
    // TODO: USE THE RAT! https://sketchfab.com/3d-models/rat-847629266c0f442da74fb132f46f3baf
    // just the rat model for now
    // subclass so that we can add some custom behavior
    game_object_root_ = std::make_shared<Empty>(ctx);
    auto cam = std::make_shared<GameCamera>(ctx);
    game_object_root_->AddChild(cam);
    cam->SetPosition(glm::vec3(0, 0, -5));
    cam->SetRotation(glm::vec3(0, 1.6, 0));
    cam->SetFov(45.0f);
    cam->SetActive(true);
    auto rat = std::make_shared<RatModel>(ctx);
    rat->SetPosition(glm::vec3(0, 0, 3));
    game_object_root_->AddChild(rat);

    auto light = std::make_shared<SpotLight>(ctx);
    light->SetPosition(glm::vec3(1, 4, -2));
    light->SetDiffuseIntensity(1.0);
    game_object_root_->AddChild(light);

    auto t = std::make_shared<FrameText>(ctx);
    t->SetTextColor(glm::vec4(1.0, 0.5, 1.0, 1.0));
    t->SetTextSize(384.0f);
    t->SetPosition(glm::vec3(2, 0, 0));
    rat->AddChild(t);
  }

  std::shared_ptr<Object> GetGameObjectRoot() {
    return game_object_root_;
  }
 private:
  Context* ctx;
  std::shared_ptr<GameObject> game_object_root_;  // game object root lol
};

int main(int argc, char** argv) {
  GLFWwindow* main_win = InitializeGLFW(1280, 720, "and he never stoped playing, he always was keep beliving");
  auto ctx = std::make_shared<Context>(main_win);
  auto scene = std::make_shared<TestScene>(ctx.get());
  GameLoop(scene, ctx, main_win);

  glfwDestroyWindow(main_win);
  glfwTerminate();
  return 0;
}
