#ifndef FPS_COUNTER_H_
#define FPS_COUNTER_H_

#include <font/UITextObject.hpp>

#define FRAME_CACHE_SIZE 144

namespace monkeysworld {
namespace critter {
namespace ui {

class FPSCounter : public font::UITextObject {

 public:
  FPSCounter(engine::Context* ctx, const std::string& font_path);
  void Update() override;
 private:
  int CalculateFPS();
  double frame_times[FRAME_CACHE_SIZE];
  int offset;
  double current_time;
};

}
}
}

#endif