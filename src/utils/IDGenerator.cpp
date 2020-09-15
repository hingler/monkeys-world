#include "utils/IDGenerator.hpp"

IDGenerator::IDGenerator() : counter_(0) { };

uint64_t IDGenerator::CreateUniqueID() {
  std::lock_guard<std::mutex> lock(id_lock_);
  while (identifiers_.find(counter_) != identifiers_.end()) {
    ++counter_;
  }

  identifiers_.insert(counter_);
  return counter_++;
}

void IDGenerator::InsertUniqueID(uint64_t id) {
  identifiers_.insert(id);
}

bool IDGenerator::IsIDUnique(uint64_t targetID) {
  std::lock_guard<std::mutex> lock(id_lock_);
  return (identifiers_.find(targetID) != identifiers_.end());
}