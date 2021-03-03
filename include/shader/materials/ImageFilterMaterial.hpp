#ifndef IMAGE_FILTER_MATERIAL_H_
#define IMAGE_FILTER_MATERIAL_H_

#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>
#include <engine/Context.hpp>
#include <shader/FilterSequence.hpp>

#include <vector>

namespace monkeysworld {
namespace shader {
namespace materials {

/**
 *  ImageFilterMaterial represents a shader that applies filters to images.
 */ 
class ImageFilterMaterial : public Material {
 public:
  ImageFilterMaterial();

  /**
   *  Passes an HSL filter to the shader.
   *  This call is invalid if more than FILTER_COUNT HSL filters have been specified,
   *  or if more than MAX_FILTERS filters overall have been specified.
   *  @param filter - the filter being uploaded.
   */ 
  void AddHSLFilter(const filter_hsl& filter);

  void SetTexture(GLuint tex);

  /**
   *  Invalidates any filters that may have been applied in a previous call.
   */ 
  void ClearFilters();
  void UseMaterial() override;

  // max number of instances of a particular filter type.
  static const int FILTER_COUNT = 6;
  static const int MAX_FILTERS = 16;
 private:
  ShaderProgram prog_;
  
  int hsl_filters_count_;
  filter_hsl hsl_filters_[FILTER_COUNT];

  int filter_count_;
  int index_list_[MAX_FILTERS];

  GLuint tex_;

};

}
}
}

#endif