/**
 *  Thread safe ID generator
 */ 

#ifndef ID_GENERATOR_H_
#define ID_GENERATOR_H_

#include <set>
#include <cinttypes>
#include <mutex>

namespace screenspacemanager {

class IDGenerator {

 public:
  /**
   *  Creates a new ID generator.
   */
  IDGenerator();
  
  /**
   *  Generates and returns a new unique ID.
   */ 
  uint64_t CreateUniqueID();

  /**
   *  Inserts a predetermined unique ID.
   */ 
  void InsertUniqueID(uint64_t id);

  /**
   *  Returns true if the ID does not exist in the internal counter.
   */ 
  bool IsIDUnique(uint64_t targetID);

 private:
  std::set<uint64_t> identifiers_;
  uint64_t counter_;
  std::mutex id_lock_;
};

};  // namespace screenspacemanager

#endif  // ID_GENERATOR_H_