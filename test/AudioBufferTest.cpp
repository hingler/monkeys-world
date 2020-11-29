#include <audio/AudioBuffer.hpp>

#include <gtest/gtest.h>

#include <thread>

#define EPS 0.001

using ::monkeysworld::audio::AudioBuffer;

class DummyAudioBuffer : public AudioBuffer {
 public:
  DummyAudioBuffer(int capacity) : AudioBuffer(capacity) {}
  int WriteFromFile(int n) override {
    return 0;
  }

  bool EndOfFile() override {
    return false;
  }
};

TEST(AudioBufferTests, CreateAudioBuffer) {
  DummyAudioBuffer test(32);
  float test_buffer_l[32];
  float test_buffer_r[32];
  ASSERT_EQ(0, test.Read(16, test_buffer_l, test_buffer_r));
}

TEST(AudioBufferTests, ReadWriteTest) {
  DummyAudioBuffer test(32);
  float test_buffer_l[32];
  float test_buffer_r[32];
  for (int i = 0; i < 32; i++) {
    test_buffer_l[i] = i;
    test_buffer_r[i] = 32 - i;
  }

  ASSERT_EQ(test.Write(32, test_buffer_l, test_buffer_r), 32);
  float output_buffer_l[32];
  float output_buffer_r[32];
  ASSERT_EQ(test.Read(32, output_buffer_l, output_buffer_r), 32);

  for (int i = 0; i < 32; i++) {
    ASSERT_NEAR(test_buffer_l[i], output_buffer_l[i], EPS);
    ASSERT_NEAR(test_buffer_r[i], output_buffer_r[i], EPS);
  }
}

TEST(AudioBufferTests, TheRingPart) {
  DummyAudioBuffer test(32);
  float test_buffer_l[32];
  float test_buffer_r[32];
  for (int i = 0; i < 32; i++) {
    test_buffer_l[i] = i;
    test_buffer_r[i] = 32 - i;
  }

  ASSERT_EQ(test.Write(24, test_buffer_l, test_buffer_r), 24);
  ASSERT_EQ(test.Write(16, &test_buffer_l[24], &test_buffer_r[24]), 8);

  float output_buffer_l[32];
  float output_buffer_r[32];
  ASSERT_EQ(test.Read(16, output_buffer_l, output_buffer_r), 16);

  for (int i = 0; i < 16; i++) {
    ASSERT_NEAR(test_buffer_l[i], output_buffer_l[i], EPS);
    ASSERT_NEAR(test_buffer_r[i], output_buffer_r[i], EPS);
  }

  ASSERT_EQ(test.Write(32, test_buffer_l, test_buffer_r), 16);

  ASSERT_EQ(test.Read(32, output_buffer_l, output_buffer_r), 32);

  for (int i = 0; i < 16; i++) {
    ASSERT_NEAR(test_buffer_l[i + 16], output_buffer_l[i], EPS);
    ASSERT_NEAR(test_buffer_r[i + 16], output_buffer_r[i], EPS);
  }
  
  for (int i = 0; i < 16; i++) {
    ASSERT_NEAR(test_buffer_l[i], output_buffer_l[i + 16], EPS);
    ASSERT_NEAR(test_buffer_r[i], output_buffer_r[i + 16], EPS);
  }
  
}

TEST(AudioBufferTests, VerifyPeek) {
  DummyAudioBuffer test(256);
  float test_buffer_l[256];
  float test_buffer_r[256];
  for (int i = 0; i < 256; i++) {
    test_buffer_l[i] = i;
    test_buffer_r[i] = 256 - i;
  }

  ASSERT_EQ(test.Write(256, test_buffer_l, test_buffer_r), 256);

  float output_buffer_l[256];
  float output_buffer_r[256];
  ASSERT_EQ(test.Peek(128, output_buffer_l, output_buffer_r), 128);
  for (int i = 0; i < 128; i++) {
    ASSERT_NEAR(test_buffer_l[i], output_buffer_l[i], EPS);
    ASSERT_NEAR(test_buffer_r[i], output_buffer_r[i], EPS);
  }

  ASSERT_EQ(test.Peek(256, output_buffer_l, output_buffer_r), 256);
  for (int i = 0; i < 256; i++) {
    ASSERT_NEAR(test_buffer_l[i], output_buffer_l[i], EPS);
    ASSERT_NEAR(test_buffer_r[i], output_buffer_r[i], EPS);
  }
}

#define SIZE 65536

void WriteThread(DummyAudioBuffer* buffer_ptr) {
  float buffer_l[SIZE];
  float buffer_r[SIZE];
  for (int i = 0; i < SIZE; i++) {
    buffer_l[i] = i;
    buffer_r[i] = SIZE - i;
  }

  int ctr = 0;
  while (ctr < SIZE) {
    // attempt write forever >:)
    ctr += buffer_ptr->Write(256, &buffer_l[ctr], &buffer_r[ctr]);
  }
}

TEST(AudioBufferTests, ReadWriteThreads) {
  DummyAudioBuffer buffer(256);
  float output_l[SIZE];
  float output_r[SIZE];
  int ctr = 0;

  std::thread testoid(&WriteThread, &buffer);
  while (ctr < SIZE) {
    ctr += buffer.Read(256, &output_l[ctr], &output_r[ctr]);
  }

  testoid.join();
  for (int i = 0; i < SIZE; i++) {
    ASSERT_NEAR(output_l[i], i, EPS);
    ASSERT_NEAR(output_r[i], SIZE - i, EPS);
  }
}