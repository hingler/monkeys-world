#include <file/ModelLoader.hpp>
#include <critter/Model.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <cinttypes>
#include <fstream>

namespace monkeysworld {
namespace file {

using model::Mesh;
using storage::VertexPacket3D;
using boost::lexical_cast;

/**
 *  Static method used to load OBJ files.
 *  @param path - path to the model being loaded
 *  @param file_size - output param for file size
 */ 
static std::shared_ptr<Mesh<VertexPacket3D>> FromObjFile(const std::string& path, uint64_t* file_size);

ModelLoader::ModelLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
                         std::vector<cache_record> cache) {
  thread_pool_ = thread_pool;
  loader_.bytes_read = 0;
  loader_.bytes_sum = 0;
  for (auto record : cache) {
    // figure out how many bytes we need to read
    if (record.type == MODEL) {
      loader_.bytes_sum += record.file_size;
      LoadOBJToCache(record);
    }
  }
}

std::shared_ptr<model::Mesh<storage::VertexPacket3D>> ModelLoader::LoadOBJ(const std::string& path) {
  return LoadOBJFromFile(path);
}

std::future<std::shared_ptr<model::Mesh<>>> ModelLoader::LoadOBJAsync(const std::string& path) {
  
  // neat SO post about packaging up a mutable lambda: https://stackoverflow.com/questions/33436336/capture-stdpromise-in-a-lambda-c14
  // shared ptr to promise gives lambda access
  std::shared_ptr<std::promise<std::shared_ptr<model::Mesh<>>>> result
    = std::make_shared<std::promise<std::shared_ptr<model::Mesh<>>>>();

  // TBA: we could write a wrapper which allows us to store a mutable lambda
  auto lambda = [=] {
    // load sync to ptr
    // update promise when received
    auto value = LoadOBJFromFile(path);
    result->set_value(value);
  };

  thread_pool_->AddTaskToQueue(lambda);
  return std::move(result->get_future());
}

std::shared_ptr<model::Mesh<>> ModelLoader::LoadOBJFromFile(const std::string& path) {
  {
    std::unique_lock<std::mutex>(loader_mutex_);
    if (loader_.bytes_read != loader_.bytes_sum) {
      // more content to load
      // TODO: Write an exception to throw here instead of returning nullptr
      return std::shared_ptr<model::Mesh<>>();
    }
  }

  {
    // wait until the cache is done loading :)
    std::shared_lock<std::shared_timed_mutex>(cache_mutex_);
    auto i = model_cache_.find(path);
    if (i != model_cache_.end()) {
      return i->second.ptr;
    }
  }

  uint64_t size = 0;

  std::shared_ptr<model::Mesh<>> mesh = FromObjFile(path, &size);

  // insert old method here


  model_record record = {mesh, size};

  {
    std::unique_lock<std::shared_timed_mutex>(cache_mutex_);
    model_cache_.insert(std::make_pair(path, record));
  }

  return mesh;
}

std::vector<cache_record> ModelLoader::GetCache() {
  std::vector<cache_record> result;
  // store something which preserves file size
  {
    std::shared_lock<std::shared_timed_mutex>(cache_mutex_);
    cache_record record_temp;
    for (auto entry : model_cache_) {
      record_temp.file_size = entry.second.size;
      record_temp.path = entry.first;
      record_temp.type = CacheType::MODEL;
      result.push_back(record_temp);
    }
  }

  return result;
}

loader_progress ModelLoader::GetLoaderProgress() {
  std::lock_guard<std::mutex> lock(loader_mutex_);
  return loader_;
}

void ModelLoader::LoadOBJToCache(cache_record& record) {
  auto load_model = [=] {
    uint64_t file_size;
    auto result = FromObjFile(record.path, &file_size);

    // updates the file size if necessary
    model_record cache = {result, file_size};
    {
      std::unique_lock<std::shared_timed_mutex>(cache_mutex_);
      model_cache_.insert(std::make_pair(record.path, cache));
    }

    {
      std::unique_lock<std::mutex>(this->loader_mutex_);
      loader_.bytes_read += record.file_size;
    }

  };

  
  thread_pool_->AddTaskToQueue(load_model);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// OBJ LOADING CODE BELOW : )                                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

static std::shared_ptr<Mesh<VertexPacket3D>> FromObjFile(const std::string& path, uint64_t* file_size) {
  auto obj_stream = std::ifstream(path);
  if (!obj_stream.good()) {
    // invalid model location
    BOOST_LOG_TRIVIAL(error) << "File does not exist!";
    return std::shared_ptr<Mesh<>>();
  }

  obj_stream.seekg(0, std::ios_base::end);
  *file_size = obj_stream.tellg();
  obj_stream.seekg(0, std::ios_base::beg);
 
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

}
}