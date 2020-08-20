/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_TEST_UTILS_H_
#define MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_TEST_UTILS_H_

#include <algorithm>
#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "api/array_view.h"
#include "rtc_base/checks.h"

namespace webrtc {
namespace test {

constexpr float kFloatMin = std::numeric_limits<float>::min();

// Fail for every pair from two equally sized rtc::ArrayView<float> views such
// that their absolute error is above a given threshold.
void ExpectNearAbsolute(rtc::ArrayView<const float> expected,
                        rtc::ArrayView<const float> computed,
                        const float tolerance);

// Reader for binary files consisting of an arbitrary long sequence of elements
// having type T. It is possible to read and cast to another type D at once.
template <typename T, typename D = T>
class BinaryFileReader {
 public:
  explicit BinaryFileReader(const std::string& file_path,
                            const size_t chunk_size = 1)
      : is_(file_path, std::ios::binary | std::ios::ate),
        data_length_(is_.tellg() / sizeof(T)),
        chunk_size_(chunk_size) {
    RTC_CHECK_LT(0, chunk_size_);
    RTC_CHECK(is_);
    SeekBeginning();
    buf_.resize(chunk_size_);
  }
  BinaryFileReader(const BinaryFileReader&) = delete;
  BinaryFileReader& operator=(const BinaryFileReader&) = delete;
  ~BinaryFileReader() = default;
  size_t data_length() const { return data_length_; }
  bool ReadValue(D* dst) {
    if (std::is_same<T, D>::value) {
      is_.read(reinterpret_cast<char*>(dst), sizeof(T));
    } else {
      T v;
      is_.read(reinterpret_cast<char*>(&v), sizeof(T));
      *dst = static_cast<D>(v);
    }
    return is_.gcount() == sizeof(T);
  }
  bool ReadChunk(rtc::ArrayView<D> dst) {
    RTC_DCHECK_EQ(chunk_size_, dst.size());
    const std::streamsize bytes_to_read = chunk_size_ * sizeof(T);
    if (std::is_same<T, D>::value) {
      is_.read(reinterpret_cast<char*>(dst.data()), bytes_to_read);
    } else {
      is_.read(reinterpret_cast<char*>(buf_.data()), bytes_to_read);
      std::transform(buf_.begin(), buf_.end(), dst.begin(),
                     [](const T& v) -> D { return static_cast<D>(v); });
    }
    return is_.gcount() == bytes_to_read;
  }
  void SeekForward(size_t items) { is_.seekg(items * sizeof(T), is_.cur); }
  void SeekBeginning() { is_.seekg(0, is_.beg); }

 private:
  std::ifstream is_;
  const size_t data_length_;
  const size_t chunk_size_;
  std::vector<T> buf_;
};

// Factories for resource file readers; the functions below return a pair where
// the first item is a reader unique pointer and the second the number of chunks
// that can be read from the file.

// Creates a reader for the pitch buffer content at 24 kHz.
std::pair<std::unique_ptr<BinaryFileReader<float>>, const size_t>
CreatePitchBuffer24kHzReader();
// Creates a reader for the the LP residual coefficients and the pitch period
// and gain values.
std::pair<std::unique_ptr<BinaryFileReader<float>>, const size_t>
CreateLpResidualAndPitchPeriodGainReader();

}  // namespace test
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_TEST_UTILS_H_
