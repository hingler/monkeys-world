 #include <audio/AudioBuffer.hpp>
 #include <audio/AudioBufferOgg.hpp>

 #include <_stb_libs/stb_vorbis.h>

 #include <gtest/gtest.h>

 using ::monkeysworld::audio::AudioBuffer;
 using ::monkeysworld::audio::AudioBufferOgg;

 #define EPS 0.000001

// TEST(OggBufferTest, CreateOggBuffer) {
//   AudioBufferOgg oggers(16384, "resources/flap_jack_scream.ogg");
//   ASSERT_EQ(8192, oggers.WriteFromFile(8192));
//   float* output_buffer_l = new float[8192];
//   float* output_buffer_r = new float[8192];
//   ASSERT_EQ(8192, oggers.Read(8192, output_buffer_l, output_buffer_r));

//   delete[] output_buffer_l;
//   delete[] output_buffer_r;
// }

// TEST(OggBufferTest, ReadAddInterleavedTest) {
//   AudioBufferOgg oggers(16384, "resources/flap_jack_scream.ogg");
//   ASSERT_EQ(8192, oggers.WriteFromFile(8192));
//   float* output_buffer = new float[16384];
//   for (int i = 0; i < 16384; i++) {
//     output_buffer[i] = 0.0f;
//   }
//   ASSERT_EQ(8192, oggers.ReadAddInterleaved(8192, output_buffer));
//   int err;
//   float* output_verify_left = new float[8192];
//   float* output_verify_right = new float[8192];
//   stb_vorbis* f = stb_vorbis_open_filename("resources/flap_jack_scream.ogg", &err, NULL);
//   float* buffer[2] = {output_verify_left, output_verify_right};
//   stb_vorbis_get_samples_float(f, 2, buffer, 8192);
//   for (int i = 0; i < 8192; i++) {
//     ASSERT_EQ(output_verify_left[i], output_buffer[2 * i]);
//     ASSERT_EQ(output_verify_right[i], output_buffer[2 * i + 1]);
//   }
//   delete[] output_buffer;
//   delete[] output_verify_left;
//   delete[] output_verify_right;
// }
 

TEST(OggBufferTest, CheckThreadFunc) {
  AudioBufferOgg oggers(4096, "resources/flap_jack_scream.ogg");
  float* output_l = new float[131072];
  float* output_r = new float[131072];
  oggers.StartWriteThread();
  int cur = 0;
  int targ = 0;
  while (true) {
    // the issue: when packet are returned, the write head is moved forward.
    // if we read before the writer writes, shit goes awry.

    // solution: build in a mechanism which ensures that write bytes are not advanced
    //           until we are ready.
    targ = oggers.Read(64, &output_l[cur], &output_r[cur]);
    if (targ > 0) {
      cur += targ;
    } else if (oggers.EndOfFile()) {
      break;
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
  ASSERT_GE(cur, stb_vorbis_stream_length_in_samples(file));
  stb_vorbis_get_samples_float(file, 2, test, cur);
  for (int i = 0; i < stb_vorbis_stream_length_in_samples(file); i++) {
    ASSERT_NEAR(truth_r[i], output_r[i], EPS);
    ASSERT_NEAR(truth_l[i], output_l[i], EPS);
  }

  delete[] output_l;
  delete[] output_r;
  delete[] truth_l;
  delete[] truth_r;
  stb_vorbis_close(file);
}

// TEST(OggBufferTest, WriteAndAutoSeek) {
//   AudioBufferOgg oggers(16384, "resources/flap_jack_scream.ogg");
//   float* test_buffer_l = new float[8192];
//   float* test_buffer_r = new float[8192];
//   // leave the garbage -- we're going to ignore it
//   ASSERT_EQ(8192, oggers.Write(8192, test_buffer_l, test_buffer_r));
//   ASSERT_EQ(8192, oggers.WriteFromFile(8192));

//   // 16384 samples -- 8192 garbage, then 8192+ from the ogg.
//   int err;
//   stb_vorbis* file = stb_vorbis_open_filename("resources/flap_jack_scream.ogg", &err, NULL);
//   if (file == NULL) {
//     std::cout << "what" << std::endl;
//   }
//   stb_vorbis_seek(file, 8192);
//   float* buffer[2] = {test_buffer_l, test_buffer_r};
//   // test buffers contain 8192 - 16384 from stb vorbis
//   stb_vorbis_get_samples_float(file, 2, buffer, 8192);
//   float* output_buffer_l = new float[8192];
//   float* output_buffer_r = new float[8192];
//   ASSERT_EQ(8192, oggers.Read(8192, output_buffer_l, output_buffer_r));
//   // output buffers contain ogg buffer's 8192 - 16384
//   ASSERT_EQ(8192, oggers.Read(8192, output_buffer_l, output_buffer_r));
//   for (int i = 0; i < 8192; i++) {
//     ASSERT_NEAR(output_buffer_l[i], test_buffer_l[i], EPS);
//     ASSERT_NEAR(output_buffer_r[i], test_buffer_r[i], EPS);
//   }

//   // ergo -- to write from cache, we just have to load the cache and write the samples,
//   // before starting the write thread.

//   // our write func will handle seeking and everything

//   delete[] test_buffer_l;
//   delete[] test_buffer_r;
//   delete[] output_buffer_l;
//   delete[] output_buffer_r;
//   stb_vorbis_close(file);
// }