// load a font sync/async and generate some geometry to ensure that its valid
#include <file/FontLoader.hpp>
#include <file/LoaderThreadPool.hpp>

#include <gtest/gtest.h>

using ::monkeysworld::file::LoaderThreadPool;
using ::monkeysworld::file::FontLoader;
using ::monkeysworld::file::cache_record;

TEST(FontLoaderTests, LoadFontSimple) {
  auto threads = std::make_shared<LoaderThreadPool>(2);
  FontLoader loader(threads, std::vector<cache_record>());
  auto font = loader.LoadFont("resources/Montserrat-Light.ttf");
  ASSERT_NE(nullptr, font.get());
  auto mesh = font->GetTextGeometry("mario", 32.0f);
  ASSERT_EQ(20, mesh->GetVertexCount());
  ASSERT_EQ(30, mesh->GetIndexCount());
  std::cout << "does this run?" << std::endl;
}