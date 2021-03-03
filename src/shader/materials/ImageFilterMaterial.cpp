#include <shader/materials/ImageFilterMaterial.hpp>

#include <shader/ShaderProgramBuilder.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

ImageFilterMaterial::ImageFilterMaterial() {
  prog_ = ShaderProgramBuilder()
            .WithVertexShader("resources/glsl/image-filter/image-filter-material.vert")
            .WithFragmentShader("resources/glsl/image-filter/image-filter-material.frag")
            .Build();
  filter_count_ = 0;
  hsl_filters_count_ = 0;
}

void ImageFilterMaterial::AddHSLFilter(const filter_hsl& filter) {
  if (hsl_filters_count_ < FILTER_COUNT && filter_count_ < MAX_FILTERS) {
    hsl_filters_[hsl_filters_count_++] = filter;
    filter_count_++;
  }
}

void ImageFilterMaterial::SetTexture(GLuint tex) {
  tex_ = tex;
}

void ImageFilterMaterial::ClearFilters() {
  filter_count_ = 0;
  hsl_filters_count_ = 0;
}

void ImageFilterMaterial::UseMaterial() {
  glUseProgram(prog_.GetProgramDescriptor());

  for (int i = 0; i < FILTER_COUNT; i++) {
    glUniform1f(3 * i, hsl_filters_[i].hue);
    glUniform1f(3 * i + 1, hsl_filters_[i].sat);
    glUniform1f(3 * i + 2, hsl_filters_[i].light);
  }

  glUniform1iv(18, 16, index_list_);
  glUniform1i(34, filter_count_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_);
  glUniform1i(35, 0);
}

}
}
}