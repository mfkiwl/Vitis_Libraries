/*
 * Copyright 2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/xf_headers.hpp"
#include <stdlib.h>
#include <ap_int.h>
#include <stdio.h>
#include "xf_flip_tb_config.h"
#include "xcl2.hpp"
#include "xf_opencl_wrap.hpp"

using namespace std;
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <INPUT IMAGE PATH > \n", argv[0]);
        return EXIT_FAILURE;
    }

    cv::Mat in_img, out_img, diff;
    cv::Mat in_gray, out_hls;

// Reading in the images:
#if GRAY
    in_gray = cv::imread(argv[1], 0);
#else
    in_gray = cv::imread(argv[1], 1);
#endif

    if (in_gray.data == NULL) {
        printf("ERROR: Cannot open image %s\n ", argv[1]);
        return EXIT_FAILURE;
    }

    int direction = -1;

// Allocate memory for the output images:
#if GRAY
    out_img.create(in_gray.rows, in_gray.cols, CV_OUT_TYPE);
    out_hls.create(in_gray.rows, in_gray.cols, CV_OUT_TYPE);
#else
    out_img.create(in_gray.rows, in_gray.cols, CV_OUT_TYPE);
    out_hls.create(in_gray.rows, in_gray.cols, CV_OUT_TYPE);
#endif

    int height = in_gray.rows;
    int width = in_gray.cols;
    std::cout << "Input image height : " << height << std::endl;
    std::cout << "Input image width  : " << width << std::endl;

// Opencv Flip Function
#if HOR
#if VER
    // Both Horizontal and vertical flip
    cv::flip(in_gray, out_img, -1);
#else
    // horizontal flip
    cv::flip(in_gray, out_img, 1);
    direction = 1;
#endif
#else
    // vertical flip
    cv::flip(in_gray, out_img, 0);
    direction = 0;
#endif

    std::cout << "Input Image Bit Depth:" << XF_DTPIXELDEPTH(IN_TYPE, NPPCX) << std::endl;
    std::cout << "Input Image Channels:" << XF_CHANNELS(IN_TYPE, NPPCX) << std::endl;
    std::cout << "NPPC:" << NPPCX << std::endl;

    ////////////////////	HLS TOP function call	/////////////////

    /////////////////////////////////////// CL ////////////////////////
    (void)cl_kernel_mgr::registerKernel("flip_accel", "krnl_flip", XCLIN(in_gray), XCLOUT(out_hls), XCLIN(height),
                                        XCLIN(width), XCLIN(direction));
    cl_kernel_mgr::exec_all();
    /////////////////////////////////////// end of CL ////////////////////////

    // Compute absolute difference image
    cv::absdiff(out_img, out_hls, diff);
    // Save the difference image
    cv::imwrite("diff.jpg", diff);

    imwrite("out_img.jpg", out_img);
    imwrite("out_hls.jpg", out_hls);

    printf("Error analysis");
    float err_per;
    xf::cv::analyzeDiff(diff, 0, err_per);

    if (err_per > 0.0f) {
        fprintf(stderr, "ERROR: Test Failed.\n ");
        return 1;
    }
    std::cout << "Test Passed " << std::endl;
    return 0;
}
