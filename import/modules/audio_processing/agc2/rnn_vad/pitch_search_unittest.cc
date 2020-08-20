/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_processing/agc2/rnn_vad/pitch_search.h"

#include <array>

#include "modules/audio_processing/agc2/rnn_vad/test_utils.h"
// TODO(bugs.webrtc.org/8948): Add when the issue is fixed.
// #include "test/fpe_observer.h"
#include "test/gtest.h"

namespace webrtc {
namespace rnn_vad {
namespace test {

// TODO(bugs.webrtc.org/9076): Remove when the issue is fixed.
TEST(RnnVadTest, PitchSearchBitExactness) {
  auto lp_residual_reader = CreateLpResidualAndPitchPeriodGainReader();
  const size_t num_frames = lp_residual_reader.second;
  std::array<float, 864> lp_residual;
  float expected_pitch_period, expected_pitch_gain;
  PitchInfo last_pitch;
  {
    // TODO(bugs.webrtc.org/8948): Add when the issue is fixed.
    // FloatingPointExceptionObserver fpe_observer;

    for (size_t i = 0; i < num_frames; ++i) {
      SCOPED_TRACE(i);
      lp_residual_reader.first->ReadChunk(
          {lp_residual.data(), lp_residual.size()});
      lp_residual_reader.first->ReadValue(&expected_pitch_period);
      lp_residual_reader.first->ReadValue(&expected_pitch_gain);
      last_pitch =
          PitchSearch({lp_residual.data(), lp_residual.size()}, last_pitch);
      EXPECT_EQ(static_cast<size_t>(expected_pitch_period), last_pitch.period);
      EXPECT_NEAR(expected_pitch_gain, last_pitch.gain, 1e-5f);
    }
  }
}

}  // namespace test
}  // namespace rnn_vad
}  // namespace webrtc
