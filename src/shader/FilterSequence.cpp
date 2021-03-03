#include <shader/FilterSequence.hpp>

#include <algorithm>

namespace monkeysworld {
namespace shader {

FilterSequence::FilterSequence() {

}

void FilterSequence::AddHSLFilter(float hue, float sat, float light) {
  std::shared_ptr<filter_hsl> filter = std::make_shared<filter_hsl>();
  filter->hue = hue;
  filter->sat = std::max(std::min(1.0f, sat), -1.0f);
  filter->light = std::max(std::min(1.0f, sat), -1.0f);
  filters_.push_back(filter);
}

std::shared_ptr<const filter_data> FilterSequence::operator[](int index) const {
  return filters_[index];
}

int FilterSequence::GetFilterCount() const {
  return static_cast<int>(filters_.size());
}

}
}