#include <critter/Visitor.hpp>

namespace monkeysworld {
namespace critter {

void Visitor::VisitChildren(std::shared_ptr<Object> o) {
  for (auto child : o->GetChildren()) {
    child->Accept(*this);
  }
}

}
}