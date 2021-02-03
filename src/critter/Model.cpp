#include <critter/Model.hpp>
#include <critter/GameObject.hpp>

#include <file/CachedFileLoader.hpp>
#include <model/Mesh.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <unordered_map>

namespace monkeysworld {
namespace critter {

using storage::VertexPacket3D;
using file::CachedFileLoader;
using model::Mesh;
using boost::lexical_cast;

using engine::Context;

////////////////////////////////////////////////////////////////////////////////
//
// This code is being moved over to file/ModelLoader.cpp. Don't use it :)
//
////////////////////////////////////////////////////////////////////////////////

/**
 *  Hashable struct which refers to position, normal, tex indices when loading from obj.
 */ 
struct vnt_triplet {
  uint32_t v_index;
  uint32_t n_index;
  uint32_t t_index;
};

constexpr bool operator==(const vnt_triplet& lhs, const vnt_triplet& rhs) {
  return (lhs.v_index == rhs.v_index
       && lhs.n_index == rhs.n_index
       && lhs.t_index == rhs.t_index);
}

enum data_type {
  vertex_normal,
  vertex_position,
  texture_coord,
  ignore
};

/**
 *  Hash func for triplets.
 */ 
struct vnt_triplet_hash {
  std::size_t operator()(vnt_triplet const& triplet) const noexcept {
    std::size_t res = triplet.v_index | (((std::size_t)triplet.n_index) << 32);
    // whatever lol
    return res ^ (std::size_t)triplet.t_index;
  }
};

/**
 *  Inserts face indices found in `line` into `set`.
 *  @param map - map of triplets to indices.
 *  @param line - the line being parsed. Must contain OBJ data as specified by format.
 *  @param poly_data - reference to vector containing polygons by index.
 *  @param vert_count - number of vertices tallied so far.
 */ 
static void InsertFaceIndices(std::unordered_map<vnt_triplet, unsigned int, vnt_triplet_hash>& map,
                              std::string& line,
                              std::vector<std::vector<unsigned int>>& poly_data,
                              std::vector<vnt_triplet>& vert_triplets_ordered,
                              unsigned int* vert_count);

/**
 *  Trims down the header of an OBJ line.
 *  @param line - the line we are trimming.
 */ 
static void TrimHeader(std::string& line);
static data_type GetDataType(const std::string& line);

////////////////////////////////////////////////////////////////////////////////
//
// DEPRECATED CONTENT ENDS HERE
//
////////////////////////////////////////////////////////////////////////////////

Model::Model(Context* ctx) : GameObject(ctx) {}

void Model::SetMesh(const std::shared_ptr<const model::Mesh<>>& mesh) {
  mesh_ = mesh;
}

std::shared_ptr<const Mesh<>> Model::GetMesh() {
  return mesh_;
}

////////////////////////////////////////////////////////////////////////////////
//
// This code is being migrated over to file/ModelLoader.cpp. Don't use it :)
//
////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<Mesh<VertexPacket3D>> Model::FromObjFile(Context* ctx, const std::string& path) {
  return FromObjFile(path);
}

std::shared_ptr<Mesh<VertexPacket3D>> Model::FromObjFile(const std::string& path) {
  auto obj_stream = std::ifstream(path);
  if (!obj_stream.good()) {
    // invalid model location
    BOOST_LOG_TRIVIAL(error) << "File does not exist!";
    return std::shared_ptr<Mesh<>>();
  }
 
  std::string line_data;
  const int MAX_LINE_SIZE = 512;
  line_data.resize(MAX_LINE_SIZE);

  // maps unique triplets to their vert. address.
  std::unordered_map<vnt_triplet, unsigned int, vnt_triplet_hash> vert_triplets;

  // ordered list which backs up vert_triplets.
  std::vector<vnt_triplet> vert_triplets_ordered;

  // stores vectors representing polygons in the model.
  std::vector<std::vector<unsigned int>> poly_data;

  // stores position data.
  std::vector<glm::vec3> position_data;

  // stores texture coordinate data.
  std::vector<glm::vec2> texcoord_data;

  // stores normal data.
  std::vector<glm::vec3> normal_data;

  // used for tracking number of vertices thus far.
  unsigned int vert_count = 0;
  while (!obj_stream.eof()) {
    // fetch line
    std::getline(obj_stream, line_data);
    if (line_data[0] == 'f') {
      // face data -- call InsertFaceIndices to handle.
      InsertFaceIndices(vert_triplets, line_data, poly_data, vert_triplets_ordered, &vert_count);
    } else if (line_data[0] == 'v') {
      data_type type = GetDataType(line_data);
      TrimHeader(line_data);

      std::vector<std::string> vals;
      boost::split(vals, line_data, [](char c){ return std::isspace(c); }, boost::token_compress_on);
      glm::vec3 data(0.0);

      // read values from string to float!
      for (int i = 0; i < vals.size() && i < 3; i++) {
        if (vals[i].size() > 0) {
          try {
            data[i] = boost::lexical_cast<float>(vals[i]);
          }
          catch (boost::bad_lexical_cast e) {
            data[i] = 0.0f;
          }
        }
        else {
          data[i] = 0.0f;
        }
      }

      switch(type) {
        case vertex_position:
          position_data.push_back(data);
          break;
        case vertex_normal:
          normal_data.push_back(data);
          break;
        case texture_coord:
          texcoord_data.push_back(glm::vec2(data.x, data.y));
          break;
        // ignore default case
      }
    } else {
      // ignore (comment, line, mtl, etc)
      continue;
    }
  }
  
  std::shared_ptr<model::Mesh<>> mesh = std::make_shared<model::Mesh<>>();

  VertexPacket3D temp_data;
  BOOST_LOG_TRIVIAL(trace) << "logged " << position_data.size() << "pos, " << texcoord_data.size() << "tex, " << normal_data.size() << "norm.";
  BOOST_LOG_TRIVIAL(trace) << vert_triplets_ordered.size() << "vertices in total";
  for (auto vert_triplet : vert_triplets_ordered) {
    if (vert_triplet.v_index == 0) {
      temp_data.position = glm::vec3(0);
    } else {
      temp_data.position = position_data[vert_triplet.v_index - 1];
    }

    if (vert_triplet.t_index == 0) {
      temp_data.coords = glm::vec2(0);
    } else {
      temp_data.coords = texcoord_data[vert_triplet.t_index - 1];
    }

    if (vert_triplet.n_index == 0) {
      temp_data.normals = glm::vec3(1, 0, 0);
    } else {
      temp_data.normals = normal_data[vert_triplet.n_index - 1];
    }

    mesh->AddVertex(temp_data);
  }

  for (auto poly : poly_data) {
    mesh->AddPolygon(poly);
  }

  return mesh;
}

static void TrimHeader(std::string& line) {
  // int cur = 0;
  // while (line[cur] != '-'                       // negative number
  //   && !(line[cur] >= '0' && line[cur] <= '9')  // number
  //   &&   line[cur] != '/') {                    // face decl
  //   cur++;
  // }

  line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](char c) {
    return ((c >= '0' && c <= '9') || c == '-' || c == '/');
  }));
}

static void InsertFaceIndices(std::unordered_map<vnt_triplet, unsigned int, vnt_triplet_hash>& map,
                              std::string& line,
                              std::vector<std::vector<unsigned int>>& poly_data,
                              std::vector<vnt_triplet>& vert_triplets_ordered,
                              unsigned int* vert_count) {
  
  // regex to read 3+ face values (v/t/n, any missing)

  // stores face strings
  std::vector<std::string> verts;

  // stores the calculated indices for this polygon
  std::vector<unsigned int> ind_data;
  TrimHeader(line);
  boost::split(verts, line, [](char c){ return std::isspace(c); }, boost::token_compress_on);
  // each entry in `verts` now contains a single face decl
  for (auto vert : verts) {
    if (vert.size() == 0) {
      continue;
    }

    std::vector<std::string> coord_inds;
    boost::split(coord_inds, vert, [](char c){ return c == '/'; }, boost::token_compress_off);
    vnt_triplet t;

    // for each: if begin == end, empty string (slashes next to each other)
    if (coord_inds[0].begin() != coord_inds[0].end()) {
      t.v_index = lexical_cast<uint32_t>(coord_inds[0]);
    } else {
      t.v_index = 0;
    }

    if (coord_inds[1].begin() != coord_inds[1].end()) {
      t.t_index = lexical_cast<uint32_t>(coord_inds[1]);
    } else {
      t.t_index = 0;
    }

    if (coord_inds[2].begin() != coord_inds[2].end()) {
      t.n_index = lexical_cast<uint32_t>(coord_inds[2]);
    } else {
      t.n_index = 0;
    }

    auto triplet_entry = map.find(t);
    if (triplet_entry == map.end()) {
      // new vertex
      map.insert(std::make_pair(t, *vert_count));
      vert_triplets_ordered.push_back(t);
      ind_data.push_back(*vert_count);
      (*vert_count)++;
    } else {
      // use the old entry!
      ind_data.push_back(triplet_entry->second);
    }

  }

  poly_data.push_back(std::move(ind_data));
}

static data_type GetDataType(const std::string& line) {
  if (line[0] == 'v') {
    if (line[1] == 'n') {
      return data_type::vertex_normal;
    } else if (line[1] == 't') {
      return data_type::texture_coord;
    } else {
      return data_type::vertex_position;
    }
  } else {
    return data_type::ignore;
  }
}
////////////////////////////////////////////////////////////////////////////////
//
// DEPRECATED CONTENT ENDS HERE
//
////////////////////////////////////////////////////////////////////////////////

void Model::PrepareAttributes() {
  if (mesh_ != nullptr) {
    mesh_->PointToVertexAttribs();
  }
}

void Model::Draw() {
  glDrawElements(GL_TRIANGLES, static_cast<int>(mesh_->GetIndexCount()), GL_UNSIGNED_INT, (void*)0);
}

Model::Model(const Model& other) : GameObject(other) {
  mesh_ = other.mesh_;
}

Model::Model(Model&& other) : GameObject(other) {
  mesh_ = std::move(other.mesh_);
}

Model& Model::operator=(const Model& other) {
  GameObject::operator=(other);
  mesh_ = other.mesh_;
  return *this;
}

Model& Model::operator=(Model&& other) {
  GameObject::operator=(other);
  mesh_ = std::move(other.mesh_);
  return *this;
}

}
}