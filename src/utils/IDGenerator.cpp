#include <utils/IDGenerator.hpp>

namespace monkeysworld {
namespace utils {

IDGenerator::IDGenerator() {
  id_min_.store(0, std::memory_order_release);
}

uint64_t IDGenerator::GetUniqueId() {
  std::lock_guard<std::mutex> lock(id_generation_lock_);
  while (used_ids_.find(++id_min_) != used_ids_.end());

  for (auto id : used_ids_) {
    if (id < id_min_) {
      used_ids_.erase(id);
    }
  }

  return id_min_.load(std::memory_order_acquire);
}

void IDGenerator::RegisterUniqueId(uint64_t new_id) {
  std::lock_guard<std::mutex> lock(id_generation_lock_);
  used_ids_.insert(new_id);
}

}
}