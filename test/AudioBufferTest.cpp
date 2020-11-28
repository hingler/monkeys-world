#include <audio/AudioBuffer.hpp>

#include <gtest/gtest.h>

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
    ASSERT_EQ(test_buffer[i], output_buffer[i]);
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
    ASSERT_EQ(test_buffer[i], output_buffer[i]);
  }

  ASSERT_EQ(test.Write(32, test_buffer), 16);

  ASSERT_EQ(test.Read(32, output_buffer), 32);

  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(test_buffer[i + 16], output_buffer[i]);
  }
  
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(test_buffer[i], output_buffer[i + 16]);
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
    ASSERT_EQ(test_buffer[i], output_buffer[i]);
  }

  ASSERT_EQ(test.Peek(256, output_buffer), 256);
  for (int i = 0; i < 256; i++) {
    ASSERT_EQ(test_buffer[i], output_buffer[i]);
  }
}

// tba: multithreading test :)