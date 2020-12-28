#include <critter/Visitor.hpp>

namespace monkeysworld {
namespace critter {

void Visitor::VisitChildren(std::shared_ptr<Object> o) {
  for (auto child : o->GetChildren()) {
    std::shared_ptr<Object> child_ptr;
    if ((child_ptr = child.lock())) {
      child_ptr->Accept(*this);
    }
  }
}

}
}