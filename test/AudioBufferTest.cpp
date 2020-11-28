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
};

TEST(AudioBufferTests, CreateAudioBuffer) {
  DummyAudioBuffer test(32);
  float test_buffer[32];
  ASSERT_EQ(0, test.Read(16, test_buffer));
}

TEST(AudioBufferTests, ReadWriteTest) {
  DummyAudioBuffer test(32);
  float test_buffer[32];
  for (int i = 0; i < 32; i++) {
    test_buffer[i] = i;
  }

  ASSERT_EQ(test.Write(32, test_buffer), 32);
  float output_buffer[32];
  ASSERT_EQ(test.Read(32, output_buffer), 32);

  for (int i = 0; i < 32; i++) {
    ASSERT_NEAR(test_buffer[i], output_buffer[i], EPS);
  }
}

TEST(AudioBufferTests, TheRingPart) {
  DummyAudioBuffer test(32);
  float test_buffer[32];
  for (int i = 0; i < 32; i++) {
    test_buffer[i] = i;
  }

  ASSERT_EQ(test.Write(24, test_buffer), 24);
  ASSERT_EQ(test.Write(16, &test_buffer[24]), 8);

  float output_buffer[32];
  ASSERT_EQ(test.Read(16, output_buffer), 16);

  for (int i = 0; i < 16; i++) {
    ASSERT_NEAR(test_buffer[i], output_buffer[i], EPS);
  }

  ASSERT_EQ(test.Write(32, test_buffer), 16);

  ASSERT_EQ(test.Read(32, output_buffer), 32);

  for (int i = 0; i < 16; i++) {
    ASSERT_NEAR(test_buffer[i + 16], output_buffer[i], EPS);
  }
  
  for (int i = 0; i < 16; i++) {
    ASSERT_NEAR(test_buffer[i], output_buffer[i + 16], EPS);
  }
  
}

TEST(AudioBufferTests, VerifyPeek) {
  DummyAudioBuffer test(256);
  float test_buffer[256];
  for (int i = 0; i < 256; i++) {
    test_buffer[i] = i;
  }

  ASSERT_EQ(test.Write(256, test_buffer), 256);

  float output_buffer[256];
  ASSERT_EQ(test.Peek(128, output_buffer), 128);
  for (int i = 0; i < 128; i++) {
    ASSERT_NEAR(test_buffer[i], output_buffer[i], EPS);
  }

  ASSERT_EQ(test.Peek(256, output_buffer), 256);
  for (int i = 0; i < 256; i++) {
    ASSERT_NEAR(test_buffer[i], output_buffer[i], EPS);
  }
}

// tba: multithreading test :)

#define SIZE 131072

void WriteThread(DummyAudioBuffer* buffer_ptr) {
  float buffer[SIZE];
  for (int i = 0; i < SIZE; i++) {
    buffer[i] = i;
  }

  int ctr = 0;
  while (ctr < SIZE) {
    // attempt write forever >:)
    ctr += buffer_ptr->Write(256, &buffer[ctr]);
  }
}

TEST(AudioBufferTests, ReadWriteThreads) {
  DummyAudioBuffer buffer(256);
  float output[SIZE];
  int ctr = 0;

  std::thread testoid(&WriteThread, &buffer);
  while (ctr < SIZE) {
    ctr += buffer.Read(256, &output[ctr]);
  }

  testoid.join();
  for (int i = 0; i < SIZE; i++) {
    ASSERT_NEAR(output[i], i, EPS);
  }
}