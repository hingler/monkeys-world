#include <critter/ui/UIGroup.hpp>

#include <utils/ObjectGraph.hpp>
#include <critter/ui/layout/BoundingBox.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

using engine::Context;
using utils::ObjectGraph;
using namespace layout;

typedef std::shared_ptr<UIObject> child_ptr;

UIGroup::UIGroup(Context* ctx) : UIObject(ctx), mat_(ctx) { 
  max_textures_ = -1;
}

std::shared_ptr<Object> UIGroup::GetChild(uint64_t id) {
  if (id == GetId()) {
    return shared_from_this();
  }

  for (auto child : children_) {
    if (id == child->GetId()) {
      return child;
    }
  }

  for (auto child : children_) {
    if (auto res = child->GetChild(id)) {
      return res;
    }
  }

  return std::shared_ptr<Object>(nullptr);
}

std::vector<std::shared_ptr<Object>> UIGroup::GetChildren() {
  std::vector<std::shared_ptr<Object>> res;
  for (auto child : children_) {
    res.push_back(child);
  }

  return res;
}

void UIGroup::AddChild(std::shared_ptr<UIObject> obj) {
  if (obj->GetChild(this->GetId()) != NULL) {
    // bad nesting!
    return;
  } else if (obj.get() == this) {
    // no self nesting!
    return;
  }

  if (auto parent = obj->GetParent()) {
    auto ui_parent = std::static_pointer_cast<UIGroup>(parent);
    ui_parent->RemoveChild(obj->GetId());
  }

  obj->parent_ = std::weak_ptr<UIObject>(this->shared_from_this());
  children_.push_back(obj);
}

void UIGroup::RemoveChild(uint64_t id) {
  for (auto ptr = children_.begin(); ptr != children_.end(); ptr++) {
    if ((*ptr)->GetId() == id) {
      children_.erase(ptr);
      return;
    }
  }

  for (auto child : children_) {
    if (auto group = std::dynamic_pointer_cast<UIGroup>(child)) {
      group->RemoveChild(id);
      return;
    }
  }
}

void UIGroup::Layout(glm::vec2 size) {
  ObjectGraph o;
  
  for (auto child : children_) {
    layout::UILayoutParams param = child->GetLayoutParams();

    int i = 0;
    for (layout::Margin* m = reinterpret_cast<layout::Margin*>(&param); i < 4; i++, m++) {
      if (m->anchor_id != 0) {
        // if we encounter a vertex which is not a child, break out.
        o.AddEdge(m->anchor_id, child->GetId());
      }
    }
  }

  std::vector<uint64_t> sort = o.TopoSort();

  std::unordered_map<uint64_t, layout::BoundingBox> bounding_boxes;
  glm::vec2 dims_local = GetDimensions();
  // working box
  layout::BoundingBox b;
  // represents the containing group
  b.left = 0;
  b.right = dims_local.x;
  b.top = 0;
  b.bottom = dims_local.y;
  bounding_boxes.insert(std::make_pair(GetId(), b));
  // handle invalid orderings below
  //  - if we encounter a node in our topo sort which is not a child, or not the group,
  //    then break run some fallback method which just reads positions + dimensions.
  //  - only once we have a bounding box for every node will we adjust positions + dimensions.
  for (auto id : sort) {
    if (id == GetId()) {
      // avoid laying out myself
      continue;
    }
    auto child = std::dynamic_pointer_cast<UIObject>(GetChild(id));
    if (!child) {
      BOOST_LOG_TRIVIAL(error) << "current layout invalid -- child with ID " << id << " not found in UIObject ID " << GetId() << "!";
      return;
    }

    auto params = child->GetLayoutParams();

    glm::vec2 child_dims = child->GetDimensions();
    glm::vec2 child_pos = child->GetPosition();

    // insert default values
    b.top = child_pos.y;
    b.left = child_pos.x;
    b.bottom = b.top + child_dims.y;
    b.right = b.left + child_dims.x;
    
    // top/bottom
    if (params.top.anchor_id != 0) {
      const layout::BoundingBox& box = bounding_boxes.at(params.top.anchor_id);
      switch (params.top.anchor_face) {
        case Face::BOTTOM:
          b.top = box.bottom;
          break;
        case Face::TOP:
          b.top = box.top;
          break;
        default:
          BOOST_LOG_TRIVIAL(error) << "Invalid face provided for ID " << id << "'s top margin -- ignoring...";
          return;
        // all others are invalid
      }

      if (params.bottom.anchor_id == 0) {
        b.bottom = b.top + child_dims.y;
      }
    }

    if (params.bottom.anchor_id != 0) {
      const layout::BoundingBox& box = bounding_boxes.at(params.bottom.anchor_id);
      switch (params.bottom.anchor_face) {
        case Face::BOTTOM:
          b.bottom = box.bottom;
          break;
        case Face::TOP:
          b.bottom = box.top;
          break;
        default:
          BOOST_LOG_TRIVIAL(error) << "Invalid face provided for ID " << id << "'s bottom margin -- ignoring...";
          return;
      }

      if (params.top.anchor_id == 0) {
        b.top = b.bottom - child_dims.y;
      }
    }

    // left/right
    if (params.left.anchor_id != 0) {
      const layout::BoundingBox& box = bounding_boxes.at(params.left.anchor_id);
      switch (params.left.anchor_face) {
        case Face::LEFT:
          b.left = box.left;
          break;
        case Face::RIGHT:
          b.left = box.right;
          break;
        default:
          BOOST_LOG_TRIVIAL(error) << "Invalid face provided for ID " << id << "'s left margin -- ignoring...";
          return;
      }

      if (params.right.anchor_id == 0) {
        b.right = b.left + child_dims.x;
      }
    }

    if (params.right.anchor_id != 0) {
      const layout::BoundingBox& box = bounding_boxes.at(params.right.anchor_id);
      switch (params.right.anchor_face) {
        case Face::LEFT:
          b.right = box.left;
          break;
        case Face::RIGHT:
          b.right = box.right;
          break;
        default:
          BOOST_LOG_TRIVIAL(error) << "Invalid face provided for ID " << id << "'s right margin -- ignoring...";
          return;
      }

      if (params.left.anchor_id == 0) {
        b.left = b.right - child_dims.x;
      }
    }

    // handle autos
    if (params.top.margin.type == MarginType::AUTO || params.bottom.margin.type == MarginType::AUTO) {
      // total range
      float y_range = b.bottom - b.top;
      // margin shrink on both sides
      float y_squeeze = (y_range - child_dims.y) / 2;
      b.top += y_squeeze;
      b.bottom -= y_squeeze;
    } else {
      if (params.top.anchor_id != 0) {
        b.top += params.top.margin.dist;
        if (params.bottom.anchor_id == 0) {
          b.bottom += params.top.margin.dist;
        }
      }

      if (params.bottom.anchor_id != 0) {
        b.bottom -= params.bottom.margin.dist;
        if (params.top.anchor_id == 0) {
          b.top -= params.bottom.margin.dist;
        }
      }
    }

    // BOOST_LOG_TRIVIAL(trace) << b.top << ", " << b.bottom << ", " << b.left << ", " << b.right;

    if (params.left.margin.type == MarginType::AUTO || params.right.margin.type == MarginType::AUTO) {
      float x_range = b.right - b.left;
      float x_squeeze = (x_range - child_dims.x) / 2;
      b.left += x_squeeze;
      b.right -= x_squeeze;
    } else {
      if (params.right.anchor_id != 0) {
        b.right -= params.right.margin.dist;
        if (params.left.anchor_id == 0) {
          b.left = b.right - child_dims.x;
        }
      }

      if (params.left.anchor_id != 0) {
        b.left += params.left.margin.dist;
        if (params.right.anchor_id == 0) {
          b.right += params.left.margin.dist;
        }
      }
    }

      if (params.right.anchor_id != 0) {
        b.right -= params.right.margin.dist;
        if (params.left.anchor_id == 0) {
          b.left -= params.right.margin.dist;
        }
      }
    }

    b.top = std::round(b.top);
    b.bottom = std::round(b.bottom);
    b.left = std::round(b.left);
    b.right = std::round(b.right);
    bounding_boxes.insert(std::make_pair(id, b));
    // BOOST_LOG_TRIVIAL(trace) << b.top << ", " << b.bottom << ", " << b.left << ", " << b.right;
  }


  // once this has run for all components, our bounding boxes are defined for every component.
  // now we can lay them out!
  for (const auto& e : bounding_boxes) {
    auto child = std::dynamic_pointer_cast<UIObject>(GetChild(e.first));
    auto bb = e.second;
    // we ought to round to the nearest int for these bounding boxes
    child->SetPosition(glm::vec2(bb.left, bb.top));
    child->SetDimensions(glm::vec2(bb.right - bb.left, bb.bottom - bb.top));
  }
}

void UIGroup::DrawUI(glm::vec2 min, glm::vec2 max, shader::Canvas canvas) {
  // note: framebuffer is bound if this is being called
  // plus, all of its children have already been drawn
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_textures_);
  UIGroupPacket p;
  std::shared_ptr<UIObject> child;
  glm::vec2 group_dims = GetDimensions();
  GLuint textures[4];
  float opacities[4];
  int current_child = 0;
  int index = 0;
  mesh_.Clear();
  
  // maintain sorted z-index order for children+
  std::sort(children_.begin(), children_.end(), [&](child_ptr a, child_ptr b) {
    return (a->z_index > b->z_index);
  });
  
  while (current_child < children_.size()) {
    // add shit to our mesh as we go
    // once we hit 4: draw it
    // if we're done, and index > 0: draw it
    child = children_[current_child];
    glm::vec2 min_coord = child->GetPosition();
    glm::vec2 max_coord = min_coord + child->GetDimensions();
    if (min_coord.x > group_dims.x || min_coord.y > group_dims.y || max_coord.x < 0 || max_coord.y < 0) {
      current_child++;
      continue;
    }

    textures[index] = child->GetFramebufferColor();
    opacities[index] = child->GetOpacity();
    p.index = static_cast<float>(index);

    glm::vec2 pos = child->GetPosition();
    glm::vec2 dims = GetDimensions();

    auto dims_child = child->GetDimensions();

    // top left
    p.pos.x = (pos.x / dims.x) * 2 - 1;
    p.pos.y = 1 - (pos.y / dims.y) * 2;


    p.texcoord = glm::vec2(0, 1);
    mesh_.AddVertex(p);

    // bottom left
    p.pos.y -= (dims_child.y / dims.y) * 2;
    p.texcoord = glm::vec2(0, 0);
    mesh_.AddVertex(p);

    // bottom right
    p.pos.x += (dims_child.x / dims.x) * 2;
    p.texcoord = glm::vec2(1, 0);
    mesh_.AddVertex(p);

    // top right
    p.pos.y += (dims_child.y / dims.y) * 2;
    p.texcoord = glm::vec2(1, 1);
    mesh_.AddVertex(p);

    mesh_.AddPolygon(4 * index, 4 * index + 1, 4 * index + 2);
    mesh_.AddPolygon(4 * index, 4 * index + 2, 4 * index + 3);

    current_child++;
    index++;

    if (index == 4) {
      mat_.SetTextures(textures);
      mat_.SetOpacity(opacities);
      mesh_.PointToVertexAttribs();
      mat_.UseMaterial();
      glDrawElements(GL_TRIANGLES, static_cast<int>(mesh_.GetIndexCount()), GL_UNSIGNED_INT, (void*)0);

      index = 0;
    }
  }

  if (index > 0) {
    mat_.SetTextures(textures);
    mat_.SetOpacity(opacities);
    mesh_.PointToVertexAttribs();
    mat_.UseMaterial();
    glDrawElements(GL_TRIANGLES, static_cast<int>(mesh_.GetIndexCount()), GL_UNSIGNED_INT, (void*)0);
  }
}

void UIGroup::UIGroupPacket::Bind() {
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIGroupPacket), (void*)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIGroupPacket), (void*)(sizeof(glm::vec2)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(UIGroupPacket), (void*)(2 * sizeof(glm::vec2)));
  glEnableVertexAttribArray(2);
}

}
}
}