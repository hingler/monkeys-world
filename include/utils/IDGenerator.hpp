#ifndef ID_GENERATOR_H_
#define ID_GENERATOR_H_

#include <atomic>
#include <cinttypes>
#include <mutex>
#include <set>

namespace monkeysworld {
namespace utils {

/**
 *  (typically) singleton class which manages identifiers.
 */ 
class IDGenerator {
  // default ctor for id generator
 public:
  IDGenerator();

  /**
   *  Returns a new unique ID.
   */ 
  uint64_t GetUniqueId();

  /**
   *  Marks an ID as used.
   */ 
  void RegisterUniqueId(uint64_t new_id);



 private:
  // lock for creating/fetching IDs.
  std::mutex id_generation_lock_;

  // tracks the minimum ID which we can use
  std::atomic<uint64_t> id_min_;

  // set of all IDs currently in use within this generator's scope
  std::set<uint64_t> used_ids_;
};

} // namespace utils
} // namespace monkeysworld

#endif