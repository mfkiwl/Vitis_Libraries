/*
 * Copyright 2021 Xilinx, Inc.
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

#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <common/xf_aie_const.hpp>

// tile dimensions are normally computed by tiler but we need to
// hardcode these values to set the graph window sizes
static constexpr int NUM_TILES = 1;
using DATA_TYPE = uint8_t;
static constexpr int CHANNELS_IN = 4;
static constexpr int CHANNELS_OUT = 1;
static constexpr int TILE_WIDTH = 64; // in terms of pixels
static constexpr int TILE_HEIGHT = 16;
static constexpr int TILE_ELEMENTS = (TILE_WIDTH * TILE_HEIGHT);
static constexpr int TILE_WINDOW_SIZE_OUT = TILE_ELEMENTS * sizeof(DATA_TYPE);
static constexpr int ELEM_WITH_METADATA_IN =
    CHANNELS_IN * TILE_ELEMENTS + (xf::cv::aie::METADATA_SIZE / sizeof(DATA_TYPE));
static constexpr int ELEM_WITH_METADATA_OUT =
    CHANNELS_OUT * TILE_ELEMENTS + (xf::cv::aie::METADATA_SIZE / sizeof(DATA_TYPE));

/* Graph specific configuration */
static constexpr int VECTORIZATION_FACTOR = 32;

static constexpr int __X86_DEVICE__ = 0;
#endif //__CONFIG_H_