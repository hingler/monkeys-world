#include <file/CacheStreambuf.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <memory>

using ::screenspacemanager::file::CacheStreambuf;

TEST(StreambufTests, CreateStreambufFromArbVector) {
  std::shared_ptr<std::vector<char>> data = std::make_shared<std::vector<char>>();
  for (int i = 0; i < 1024; i++) {
    data->push_back((char)(i & 0xFF));
  }

  CacheStreambuf test_buf(data);
  for (int i = 0; i < 1024; i++) {
    ASSERT_EQ(data->operator[](i), (char)test_buf.sbumpc());
  }

  ASSERT_EQ((char)EOF, (char)test_buf.sbumpc());
}

TEST(StreambufTests, AttemptWrite) {
  std::shared_ptr<std::vector<char>> data = std::make_shared<std::vector<char>>();
  for (int i = 0; i < 1024; i++) {
    data->push_back((char)(i & 0xFF));
  }
  
  CacheStreambuf test_buf(data);

  for (int i = 0; i < 256; i++) {
    ASSERT_EQ(EOF, (char)test_buf.sputc((char)((0xFF - i) & 0xFF)));  
  }

  for (int i = 0; i < 1024; i++) {
    ASSERT_EQ(data->operator[](i), (char)test_buf.sbumpc());
  }
}

TEST(StreambufTests, VerifyIstreamBehavior) {
  std::shared_ptr<std::vector<char>> data = std::make_shared<std::vector<char>>();
  for (int i = 0; i < 1024; i++) {
    data->push_back((char)(i & 0xFF));
  }

  CacheStreambuf test_buf(data);
  std::istream test_stream(&test_buf);

  char data_actual[1024];
  test_stream.read(data_actual, 1024);
  for (int i = 0; i < 1024; i++) {
    ASSERT_EQ(data->operator[](i), data_actual[i]);
    data_actual[i] = 0;
  }

  test_stream.read(data_actual, 1);

  ASSERT_TRUE(test_stream.eof());
  ASSERT_TRUE(test_stream.fail());
  ASSERT_FALSE(test_stream.bad());

  test_stream.clear();
  test_stream.seekg(0, std::ios_base::beg);
  ASSERT_FALSE(test_stream.eof());
  ASSERT_FALSE(test_stream.fail());
  ASSERT_FALSE(test_stream.bad());
  test_stream.read(data_actual, 256);

  for (int i = 0; i < 256; i++) {
    ASSERT_EQ(data->operator[](i), data_actual[i]);
  }

  test_stream.seekg(128, std::ios_base::cur);
  test_stream.read(data_actual, 256);

  for (int i = 0; i < 256; i++) {
    ASSERT_EQ(data->operator[](i + 384), data_actual[i]);
  }

  test_stream.seekg(256, std::ios_base::end);
  test_stream.read(data_actual, 256);
  for (int i = 0; i < 256; i++) {
    ASSERT_EQ(data->operator[](i + 768), data_actual[i]);
  }
}