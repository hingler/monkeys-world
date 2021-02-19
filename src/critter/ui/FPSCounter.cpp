#include <critter/ui/FPSCounter.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

FPSCounter::FPSCounter(engine::Context* ctx, const std::string& font_path) : UITextObject(ctx, font_path) {
  current_time = 0.0f;
  offset = 0;
  for (int i = 0; i < FRAME_CACHE_SIZE; i++) {
    frame_times[i] = 0.1f;
  }
}

void FPSCounter::Update() {
  double delta = GetContext()->GetDeltaTime();
  frame_times[offset] = delta;
  offset++;
  if (offset >= FRAME_CACHE_SIZE) {
    offset -= FRAME_CACHE_SIZE;
  }
  current_time += delta;
  if (current_time > 1.0) {
    current_time -= 1.0;
    SetText(std::to_string(CalculateFPS()) + " FPS");
    Invalidate();
  }
}

int FPSCounter::CalculateFPS() {
  double frame_sum = 0.0;
  for (int i = 0; i < FRAME_CACHE_SIZE; i++) {
    frame_sum += frame_times[i];
  }

  frame_sum /= FRAME_CACHE_SIZE;
  return static_cast<int>(1.0 / frame_sum);
}

}
}
}