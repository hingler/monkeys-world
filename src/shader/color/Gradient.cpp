#include <shader/color/Gradient.hpp>

namespace monkeysworld {
namespace shader {
namespace color {

Gradient::Gradient() {}

void Gradient::AddStop(glm::vec4 col, float stop) {
  stop = std::max(std::min(stop, 1.0f), 0.0f);
  gradient_set_.insert({ col, stop });
}

void Gradient::ClearStops() {
  gradient_set_.clear();
}

std::vector<GradientStop> Gradient::GetStops() {
  std::vector<GradientStop> res;
  if (gradient_set_.size() == 0) {
    res.push_back({ glm::vec4(1.0), 0.5f });
    return res;
  }

  for (const auto& stop : gradient_set_) {
    res.push_back(stop);
  }

  return res;
}

}
}
}