/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at

 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "rnn_vad.h"

#include "rnn_vad/rnn.h"
#include "rnn_vad/features_extraction.h"
#include "resampler/push_resampler.h"

namespace rnnvad {

    class RnnVadImpl {
    public:
        RnnVadImpl() {
            set_sample_rate(8000);
        };

        ~RnnVadImpl() = default;

        void set_sample_rate(const int sample_rate) {
            resampler.InitializeIfNeeded(sample_rate, 24000, 1);
        }

        float process(const float *samples, const size_t length) {
            // Resample input.
            resampler.Resample(samples, length, samples_10ms_24kHz.data(), samples_10ms_24kHz.size());

            // Extract features and feed the RNN.
            const bool is_silence = features_extractor.CheckSilenceComputeFeatures(samples_10ms_24kHz, feature_vector);
            return vad.ComputeVadProbability(feature_vector, is_silence);
        }

    private:
        std::array<float, webrtc::rnn_vad::kFrameSize10ms24kHz> samples_10ms_24kHz{};
        std::array<float, webrtc::rnn_vad::kFeatureVectorSize> feature_vector{};
        webrtc::rnn_vad::FeaturesExtractor features_extractor;
        webrtc::PushResampler<float> resampler;
        webrtc::rnn_vad::RnnBasedVad vad;
    };

    RnnVad::RnnVad() : impl(new RnnVadImpl) {}

    RnnVad::~RnnVad() {
        delete impl;
    };

    void RnnVad::set_sample_rate(const int sample_rate) {
        impl->set_sample_rate(sample_rate);
    }

    float RnnVad::process(const float *frame, size_t length) {
        return impl->process(frame, length);
    }

} // namespace rnnvad