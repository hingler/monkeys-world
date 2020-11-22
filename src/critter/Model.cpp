#include <critter/Model.hpp>
#include <critter/GameObject.hpp>

#include <file/CachedFileLoader.hpp>
#include <model/Mesh.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <unordered_map>

namespace monkeysworld {
namespace critter {

using storage::VertexPacket3D;
using file::CachedFileLoader;
using model::Mesh;
using boost::lexical_cast;
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
                              const std::string& line,
                              std::vector<std::vector<unsigned int>>& poly_data,
                              std::vector<vnt_triplet>& vert_triplets_ordered,
                              unsigned int* vert_count);
static data_type GetDataType(boost::ssub_match match);

Model::Model(Context* ctx) : GameObject(ctx) {}

void Model::PrepareAttributes() {
  mesh_->PointToVertexAttribs();
}

void Model::SetMesh(const std::shared_ptr<model::Mesh<>>& mesh) {
  mesh_ = mesh;
}

std::shared_ptr<Model> Model::FromObjFile(Context* ctx, const std::string& path) {
  std::shared_ptr<CachedFileLoader> loader = ctx->GetCachedFileLoader();
  auto obj_reader = loader->LoadFile(path);
  if (obj_reader.get() == nullptr) {
    // invalid model location
    BOOST_LOG_TRIVIAL(error) << "File does not exist!";
    return std::shared_ptr<Model>();
  }

  std::istream obj_stream(obj_reader.get());  
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

  // used for incrementing vert_count -- starts at 1!
  unsigned int vert_count = 1;
  while (!obj_stream.eof()) {
    // fetch line
    obj_stream.getline(&line_data[0], MAX_LINE_SIZE);
    BOOST_LOG_TRIVIAL(trace) << line_data;
    if (line_data[0] == 'f') {
      // face data -- call InsertFaceIndices to handle.
      InsertFaceIndices(vert_triplets, line_data, poly_data, vert_triplets_ordered, &vert_count);
    } else if (line_data[0] == 'v') {
      // regex reads up to 3 float values from string, and data type
      // (extra would be ignored anyway so it doesnt matter)
      boost::regex re("(\w+)\s+((\d+(.|,)\d+\s+){,3})");
      boost::smatch result;
      if (!boost::regex_search(line_data, result, re, boost::match_default)) {
        BOOST_LOG_TRIVIAL(warning) << "Skipping line '" << line_data << "'";
        continue;
      }

      // group 2 contains all of our float values
      // split group 2 on whitespace
      std::vector<std::string> vals;
      boost::split(vals, result[2], boost::is_any_of(" \t"), boost::token_compress_on);
      glm::vec3 data(0.0);

      // read values from string to float!
      for (int i = 0; i < vals.size(); i++) {
        boost::trim(vals[i]);
        data[i] = boost::lexical_cast<float>(vals[i]);
      }

      switch(GetDataType(result[1])) {
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
  for (auto vert_triplet : vert_triplets_ordered) {
    if (vert_triplet.v_index == 0) {
      temp_data.position = glm::vec3(0);
    } else {
      temp_data.position = position_data[vert_triplet.v_index];
    }

    if (vert_triplet.t_index == 0) {
      temp_data.coords = glm::vec2(0);
    } else {
      temp_data.coords = texcoord_data[vert_triplet.t_index];
    }

    if (vert_triplet.n_index == 0) {
      temp_data.coords = glm::vec3(1, 0, 0);
    } else {
      temp_data.coords = normal_data[vert_triplet.n_index];
    }

    mesh->AddVertex(temp_data);
  }

  for (auto poly : poly_data) {
    mesh->AddPolygon(poly);
  }

  std::shared_ptr<Model> result = std::make_shared<Model>(ctx);
  result->SetMesh(mesh);
  return result;
}

static void InsertFaceIndices(std::unordered_map<vnt_triplet, unsigned int, vnt_triplet_hash>& map,
                              const std::string& line,
                              std::vector<std::vector<unsigned int>>& poly_data,
                              std::vector<vnt_triplet>& vert_triplets_ordered,
                              unsigned int* vert_count) {
  
  // regex to read 3+ face values (v/t/n, any missing)

  // stores face strings
  std::vector<std::string> verts;

  // stores the calculated indices for this polygon
  std::vector<unsigned int> ind_data;
  // TODO: use regex instead probably lol
  boost::regex face_tabs("f\s+((\d*/\d*/\d*\s*){3,})");
  boost::smatch face_match;
  if (!boost::regex_match(line, face_match, face_tabs)) {
    // face data does not match
    BOOST_LOG_TRIVIAL(error) << "Skipping line '" << line << "'";
  }

  // 1st capture group contains face decls
  boost::split(verts, face_match[1], boost::is_any_of("\t "), boost::token_compress_on);

  // each entry in `verts` now contains a single face decl
  for (auto vert : verts) {
    // also splits `f`, so account for that!
    if (vert[0] == 'f') {
      continue;
    }

    std::vector<std::string> coord_inds;
    boost::split(coord_inds, vert, [](char c){ return c == '/'; });
    vnt_triplet t;

    // for each: if begin == end, empty string (slashes next to each other)
    if (coord_inds[1].begin() != coord_inds[1].end()) {
      t.v_index = lexical_cast<uint32_t>(coord_inds[1]);
    } else {
      t.v_index = 0;
    }

    if (coord_inds[2].begin() != coord_inds[2].end()) {
      t.t_index = lexical_cast<uint32_t>(coord_inds[2]);
    } else {
      t.t_index = 0;
    }

    if (coord_inds[3].begin() != coord_inds[3].end()) {
      t.n_index = lexical_cast<uint32_t>(coord_inds[3]);
    } else {
      t.n_index = 0;
    }

    BOOST_LOG_TRIVIAL(trace) << "Read face: v=" << t.v_index << ", t=" << t.t_index << ", n=" << t.n_index;
    auto triplet_entry = map.find(t);
    if (triplet_entry == map.end()) {
      // new vertex
      map.insert(std::pair<vnt_triplet, int>(t, *vert_count));
      vert_triplets_ordered.push_back(t);
      ind_data.push_back(*vert_count);
      *vert_count++;
    } else {
      // use the old entry!
      ind_data.push_back(triplet_entry->second);
    }

  }

  poly_data.push_back(std::move(ind_data));
}

static data_type GetDataType(boost::ssub_match match) {
  if (!match.compare("v")) {
    return data_type::vertex_position;
  } else if (!match.compare("vt")) {
    return data_type::texture_coord;
  } else if (!match.compare("vn")) {
    return data_type::vertex_normal;
  } else {
    return data_type::ignore;
  }
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