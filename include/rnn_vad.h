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

#ifndef LIBRNNVAD_RNN_VAD_H
#define LIBRNNVAD_RNN_VAD_H

#include <cstddef>

namespace rnnvad {

    class RnnVadImpl;

    class RnnVad {
    public:
        RnnVad();

        ~RnnVad();

        /**
         * Sets the input sample rate in Hz for a VAD instance.
         *
         * @param sample_rate
         */
        void set_sample_rate(int sample_rate);

        /**
         * Calculates a VAD decision for an audio frame.
         *
         * @return probability of voice (range: [0.0, 1.0])
         */
        float process(const float *frame, size_t length);

    private:
        RnnVadImpl *impl;
    };

} // namespace rnnvad

#endif //LIBRNNVAD_RNN_VAD_H
