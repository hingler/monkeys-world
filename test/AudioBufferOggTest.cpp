 #include <audio/AudioBuffer.hpp>
 #include <audio/AudioBufferOgg.hpp>

 #include <_stb_libs/stb_vorbis.h>

 #include <gtest/gtest.h>

 using ::monkeysworld::audio::AudioBuffer;
 using ::monkeysworld::audio::AudioBufferOgg;

 #define EPS 0.000001

 TEST(OggBufferTest, CreateOggBuffer) {
   AudioBufferOgg oggers(16384, "resources/flap_jack_scream.ogg");
   ASSERT_EQ(8192, oggers.WriteFromFile(8192));
   float* output_buffer_l = new float[8192];
   float* output_buffer_r = new float[8192];
   ASSERT_EQ(8192, oggers.Read(8192, output_buffer_l, output_buffer_r));

   delete[] output_buffer_l;
   delete[] output_buffer_r;
 }

TEST(OggBufferTest, CheckThreadFunc) {
  AudioBufferOgg oggers(4096, "resources/flap_jack_scream.ogg");
  float* output_l = new float[131072];
  float* output_r = new float[131072];
  oggers.StartWriteThread();
  int cur = 0;
  int targ = 0;
  while (!oggers.EndOfFile() || targ > 0) {
    targ = oggers.Read(1021, &output_l[cur], &output_r[cur]);
    if (targ > 0) {
      cur += targ;
    }
  }

  ASSERT_NE(-1, targ);
  std::cout << "read " << cur << " samples!";
  float* truth_l = new float[131072];
  float* truth_r = new float[131072];

  float* test[2] = {truth_l, truth_r};
  int err;
  stb_vorbis* file = stb_vorbis_open_filename("resources/flap_jack_scream.ogg", &err, NULL);
  if (file == NULL) {
    std::cout << "oops!!!!" << std::endl;
  }
  stb_vorbis_info info = stb_vorbis_get_info(file);
  ASSERT_EQ(cur, stb_vorbis_stream_length_in_samples(file));
  stb_vorbis_get_samples_float(file, 2, test, cur);
  for (int i = 0; i < cur; i++) {
    ASSERT_NEAR(truth_r[i], output_r[i], EPS);
    ASSERT_NEAR(truth_l[i], output_l[i], EPS);
  }

  delete[] output_l;
  delete[] output_r;
  delete[] truth_l;
  delete[] truth_r;
  stb_vorbis_close(file);
  std::cout << "ding ding" << std::endl;
}