/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <array>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include <sndfile.h>
#include <rnn_vad.h>

typedef struct { SNDFILE *file; } input_context_t;

static void process(SNDFILE *infile, SNDFILE *outfile[2], const int sample_rate, const double threshold) {
    int vadres, prev = -1;
    long frames[2] = {0, 0};
    long segments[2] = {0, 0};

    const size_t frame_size_10ms = sample_rate / 100;
    std::unique_ptr<float[]> samples_10ms(new float[frame_size_10ms]);

    rnnvad::RnnVad vad;
    vad.set_sample_rate(sample_rate);

    // Compute VAD probabilities.
    while (true) {
        // Read frame at the input sample rate.
        const auto read_samples = sf_readf_float(infile, samples_10ms.get(), frame_size_10ms);
        if (read_samples < frame_size_10ms) {
            break;  // EOF.
        }

        const float probability = vad.process(samples_10ms.get(), frame_size_10ms);
        vadres = probability >= threshold ? 1 : 0;

        if (outfile[vadres]) {
            sf_writef_float(outfile[vadres], samples_10ms.get(), frame_size_10ms);
        }

        frames[vadres]++;
        if (prev != vadres) segments[vadres]++;
        prev = vadres;
    }

    const long totalFrames = frames[0] + frames[1];
    std::cout << "voice detected in " << frames[1] << " of " << totalFrames << " frames (" \
        << (totalFrames ? 100.0 * (frames[1] / (double)totalFrames) : 0.0) << "%)" << std::endl;

    std::cout << segments[1] << " voice segments, average length " << (segments[1] ? (double)frames[1] / segments[1] : 0.0) << " frames" << std::endl;
    std::cout << segments[0] << " non-voice segments, average length " << (segments[0] ? (double)frames[0] / segments[0] : 0.0) << " frames" << std::endl;
}

int main(int argc, char *argv[]) {
    int retval;
    const char *in_fname, *out_fname[2] = {nullptr, nullptr};
    SNDFILE *in_sf, *out_sf[2] = {nullptr, nullptr};
    SF_INFO in_info = {0}, out_info[2] = {0, 0};
    double threshold = 0.8;

    /*
     * parse arguments
     */
    for (int ch; (ch = getopt(argc, argv, "o:n:t:h")) != -1;) {
        switch (ch) {
            case 'o':
                out_fname[1] = optarg;
                break;
            case 'n':
                out_fname[0] = optarg;
                break;
            case 't':
                threshold = std::max(std::min(atof(optarg), 1.0), 0.0);
                break;
            case 'h':
                printf(
                        "Usage: %s [OPTION]... FILE\n"
                        "Reads FILE in wav format and performs voice activity detection (VAD).\n"
                        "Options:\n"
                        "  -o FILE      write detected voice frames to FILE in wav format\n"
                        "  -n FILE      write detected non-voice frames to FILE in wav format\n"
                        "  -t threshold threshold of judging voice\n"
                        "  -h           display this help and exit\n",
                        argv[0]);
                goto success;

            default:
                goto argfail;
        }
    }

    if (optind >= argc) {
        std::cerr << "input file expected" << std::endl;
        goto argfail;
    }

    in_fname = argv[optind++];

    if (optind < argc) {
        std::cerr << "unexpected argument '" << argv[optind] << "'; only one input file expected" << std::endl;
        goto argfail;
    }

    /*
      * open and check input file
      */
    in_sf = sf_open(in_fname, SFM_READ, &in_info);
    if (!in_sf) {
        std::cerr << "Cannot open input file '" << in_fname << "': " << sf_strerror(nullptr) << std::endl;
        goto fail;
    }
    sf_command(in_sf, SFC_SET_NORM_FLOAT, nullptr, SF_FALSE);

    if (in_info.channels != 1) {
        std::cerr << "only single-channel wav files supported; input file has " << in_info.channels << " channels" << std::endl;
        goto fail;
    }

    /*
     * open required output files
     */
    for (int i = 0; i < 2; i++) {
        if (out_fname[i]) {
            out_info[i] = (SF_INFO) {
                    .samplerate = in_info.samplerate,
                    .channels = 1,
                    .format = in_info.format
            };
            out_sf[i] = sf_open(out_fname[i], SFM_WRITE, &out_info[i]);
            if (!out_sf[i]) {
                std::cerr << "Cannot open output file '" << out_fname[i] << "': " << sf_strerror(nullptr) << std::endl;
                goto fail;
            }
            sf_command(out_sf[i], SFC_SET_NORM_FLOAT, nullptr, SF_FALSE);
        }
    }

    process(in_sf, out_sf, in_info.samplerate, threshold);

    sf_close(in_sf);
    for (auto & i : out_sf) {
        if (i) {
            sf_close(i);
        }
    }

    success:
    retval = EXIT_SUCCESS;
    goto end;

    argfail:
    std::cerr << "Try '" << argv[0] << " -h' for more information." << std::endl;
    fail:
    retval = EXIT_FAILURE;

    end:

    return retval;
}
