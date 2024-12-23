// Copyright 2011 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
//  simple command line calling the WebPEncode function.
//  Encodes a raw .YUV into WebP bitstream
//
// Author: Skal (pascal.massimino@gmail.com)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>

#ifdef HAVE_CONFIG_H
#include "webp/config.h"
#endif

#ifdef WEBP_USE_THREAD
#include <sys/sysinfo.h>
#endif

#include "xf_utils_sw/logger.hpp"

#include "webp/encode.h"

#include "./example_util.h"
#include "./metadata.h"
#include "./stopwatch.h"
#include "./create_kernel.h"

#include "./jpegdec.h"
#include "./pngdec.h"
#include "./webpdec.h"
#include "./wicdec.h"
#include "../src/utils/profiling.h"

#include "vp8_AsyncConfig.h"

#ifndef WEBP_DLL
#ifdef __cplusplus
extern "C" {
#endif

extern void* VP8GetCPUInfo; // opaque forward declaration.

#ifdef __cplusplus
} // extern "C"
#endif
#endif // WEBP_DLL

#define goto_Error                                                                                     \
    using namespace xf::common::utils_sw;                                                              \
    Logger logger(std::cout, std::cerr);                                                               \
    WebPMemoryWriterClear(&memory_writer);                                                             \
    MetadataFree(&metadata);                                                                           \
    return_value = 1;                                                                                  \
    return_value ? logger.error(Logger::Message::TEST_FAIL) : logger.info(Logger::Message::TEST_PASS); \
    return return_value;
//------------------------------------------------------------------------------

static int verbose = 0;

static int ReadYUV(const uint8_t* const data, size_t data_size, WebPPicture* const pic) {
    int y;
    const int use_argb = pic->use_argb;
    const int uv_width = (pic->width + 1) / 2;
    const int uv_height = (pic->height + 1) / 2;
    const int uv_plane_size = uv_width * uv_height;
    const size_t expected_data_size = pic->width * pic->height + 2 * uv_plane_size;

    if (data_size != expected_data_size) {
        fprintf(stderr, "input data doesn't have the expected size (%d instead of %d)\n", (int)data_size,
                (int)expected_data_size);
        return 0;
    }

    pic->use_argb = 0;
    if (!WebPPictureAlloc(pic)) return 0;

    for (y = 0; y < pic->height; ++y) {
        memcpy(pic->y + y * pic->y_stride, data + y * pic->width, pic->width * sizeof(*pic->y));
    }
    for (y = 0; y < uv_height; ++y) {
        const uint8_t* const uv_data = data + pic->height * pic->y_stride;
        memcpy(pic->u + y * pic->uv_stride, uv_data + y * uv_width, uv_width * sizeof(*uv_data));
        memcpy(pic->v + y * pic->uv_stride, uv_data + y * uv_width + uv_plane_size, uv_width * sizeof(*uv_data));
    }
    return use_argb ? WebPPictureYUVAToARGB(pic) : 1;
}

#ifdef HAVE_WINCODEC_H

static int ReadPicture(const char* const filename, WebPPicture* const pic, int keep_alpha, Metadata* const metadata) {
    int ok = 0;
    const uint8_t* data = NULL;
    size_t data_size = 0;
    if (pic->width != 0 && pic->height != 0) {
        ok = ExUtilReadFile(filename, &data, &data_size);
        ok = ok && ReadYUV(data, data_size, pic);
    } else {
        // If no size specified, try to decode it using WIC.
        ok = ReadPictureWithWIC(filename, pic, keep_alpha, metadata);
        if (!ok) {
            ok = ExUtilReadFile(filename, &data, &data_size);
            ok = ok && ReadWebP(data, data_size, pic, keep_alpha, metadata);
        }
    }
    if (!ok) {
        fprintf(stderr, "Error! Could not process file %s\n", filename);
    }
    free((void*)data);
    return ok;
}

#else // !HAVE_WINCODEC_H

typedef enum {
    PNG_ = 0,
    JPEG_,
    TIFF_, // 'TIFF' clashes with libtiff
    WEBP_,
    UNSUPPORTED
} InputFileFormat;

static uint32_t GetBE32(const uint8_t buf[]) {
    return ((uint32_t)buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

static InputFileFormat GuessImageType(const uint8_t buf[12]) {
    InputFileFormat format = UNSUPPORTED;
    const uint32_t magic1 = GetBE32(buf + 0);
    const uint32_t magic2 = GetBE32(buf + 8);
    if (magic1 == 0x89504E47U) {
        format = PNG_;
    } else if (magic1 >= 0xFFD8FF00U && magic1 <= 0xFFD8FFFFU) {
        format = JPEG_;
    } else if (magic1 == 0x49492A00 || magic1 == 0x4D4D002A) {
        format = TIFF_;
    } else if (magic1 == 0x52494646 && magic2 == 0x57454250) {
        format = WEBP_;
    }
    return format;
}

static int ReadPicture(const char* const filename, WebPPicture* const pic, int keep_alpha, Metadata* const metadata) {
    const uint8_t* data = NULL;
    size_t data_size = 0;
    int ok = 0;

    ok = ExUtilReadFile(filename, &data, &data_size);
    if (!ok) goto End;

    if (pic->width == 0 || pic->height == 0) {
        ok = 0;
        if (data_size >= 12) {
            const InputFileFormat format = GuessImageType(data);
            if (format == PNG_) {
                ok = ReadPNG(data, data_size, pic, keep_alpha, metadata);
            } else if (format == JPEG_) {
                ok = ReadJPEG(data, data_size, pic, metadata);
            } else if (format == TIFF_) {
                ; // ok = ReadTIFF(data, data_size, pic, keep_alpha, metadata);
            } else if (format == WEBP_) {
                ok = ReadWebP(data, data_size, pic, keep_alpha, metadata);
            }
        }
    } else {
        // If image size is specified, infer it as YUV format.
        ok = ReadYUV(data, data_size, pic);
    }
End:
    if (!ok) {
        fprintf(stderr, "Error! Could not process file %s\n", filename);
    }
    free((void*)data);
    return ok;
}

#endif // !HAVE_WINCODEC_H

static void AllocExtraInfo(WebPPicture* const pic) {
    const int mb_w = (pic->width + 15) / 16;
    const int mb_h = (pic->height + 15) / 16;
    pic->extra_info = (uint8_t*)malloc(mb_w * mb_h * sizeof(*pic->extra_info));
}

static void PrintByteCount(const int bytes[4], int total_size, int* const totals) {
    int s;
    int total = 0;
    for (s = 0; s < 4; ++s) {
        fprintf(stderr, "| %7d ", bytes[s]);
        total += bytes[s];
        if (totals) totals[s] += bytes[s];
    }
    fprintf(stderr, "| %7d  (%.1f%%)\n", total, 100.f * total / total_size);
}

static void PrintPercents(const int counts[4], int total) {
    int s;
    for (s = 0; s < 4; ++s) {
        fprintf(stderr, "|      %2d%%", 100 * counts[s] / total);
    }
    fprintf(stderr, "| %7d\n", total);
}

static void PrintValues(const int values[4]) {
    int s;
    for (s = 0; s < 4; ++s) {
        fprintf(stderr, "| %7d ", values[s]);
    }
    fprintf(stderr, "|\n");
}

static void PrintFullLosslessInfo(const WebPAuxStats* const stats, const char* const description) {
    fprintf(stderr, "Lossless-%s compressed size: %d bytes\n", description, stats->lossless_size);
    fprintf(stderr, "  * Header size: %d bytes, image data size: %d\n", stats->lossless_hdr_size,
            stats->lossless_data_size);
    if (stats->lossless_features) {
        fprintf(stderr, "  * Lossless features used:");
        if (stats->lossless_features & 1) fprintf(stderr, " PREDICTION");
        if (stats->lossless_features & 2) fprintf(stderr, " CROSS-COLOR-TRANSFORM");
        if (stats->lossless_features & 4) fprintf(stderr, " SUBTRACT-GREEN");
        if (stats->lossless_features & 8) fprintf(stderr, " PALETTE");
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "  * Precision Bits: histogram=%d transform=%d cache=%d\n", stats->histogram_bits,
            stats->transform_bits, stats->cache_bits);
    if (stats->palette_size > 0) {
        fprintf(stderr, "  * Palette size:   %d\n", stats->palette_size);
    }
}

static void PrintExtraInfoLossless(const WebPPicture* const pic, int short_output, const char* const file_name) {
    const WebPAuxStats* const stats = pic->stats;
    if (short_output) {
        fprintf(stderr, "%7d %2.2f\n", stats->coded_size, stats->PSNR[3]);
    } else {
        fprintf(stderr, "File:      %s\n", file_name);
        fprintf(stderr, "Dimension: %d x %d\n", pic->width, pic->height);
        fprintf(stderr, "Output:    %d bytes\n", stats->coded_size);
        PrintFullLosslessInfo(stats, "ARGB");
    }
}

static void PrintExtraInfoLossy(const WebPPicture* const pic,
                                int short_output,
                                int full_details,
                                const char* const file_name) {
    const WebPAuxStats* const stats = pic->stats;
    if (short_output) {
        fprintf(stderr, "%7d %2.2f\n", stats->coded_size, stats->PSNR[3]);
    } else {
        const int num_i4 = stats->block_count[0];
        const int num_i16 = stats->block_count[1];
        const int num_skip = stats->block_count[2];
        const int total = num_i4 + num_i16;
        fprintf(stderr, "File:      %s\n", file_name);
        fprintf(stderr, "Dimension: %d x %d%s", pic->width, pic->height, stats->alpha_data_size ? " (with alpha)" : "");
        fprintf(stderr,
                "Output:    "
                "%d bytes Y-U-V-All-PSNR %2.2f %2.2f %2.2f   %2.2f dB\n",
                stats->coded_size, stats->PSNR[0], stats->PSNR[1], stats->PSNR[2], stats->PSNR[3]);
        if (total > 0) {
            int totals[4] = {0, 0, 0, 0};
            fprintf(stderr,
                    "block count:  intra4: %d\n"
                    "              intra16: %d  (-> %.2f%%)\n",
                    num_i4, num_i16, 100.f * num_i16 / total);
            fprintf(stderr, "              skipped block: %d (%.2f%%)\n", num_skip, 100.f * num_skip / total);
            fprintf(stderr,
                    "bytes used:  header:         %6d  (%.1f%%)\n"
                    "             mode-partition: %6d  (%.1f%%)\n",
                    stats->header_bytes[0], 100.f * stats->header_bytes[0] / stats->coded_size, stats->header_bytes[1],
                    100.f * stats->header_bytes[1] / stats->coded_size);
            if (stats->alpha_data_size > 0) {
                fprintf(stderr, "             transparency:   %6d (%.1f dB)\n", stats->alpha_data_size, stats->PSNR[4]);
            }
            fprintf(stderr,
                    " Residuals bytes  "
                    "|segment 1|segment 2|segment 3"
                    "|segment 4|  total\n");
            if (full_details) {
                fprintf(stderr, "  intra4-coeffs:  ");
                PrintByteCount(stats->residual_bytes[0], stats->coded_size, totals);
                fprintf(stderr, " intra16-coeffs:  ");
                PrintByteCount(stats->residual_bytes[1], stats->coded_size, totals);
                fprintf(stderr, "  chroma coeffs:  ");
                PrintByteCount(stats->residual_bytes[2], stats->coded_size, totals);
            }
            fprintf(stderr, "    macroblocks:  ");
            PrintPercents(stats->segment_size, total);
            fprintf(stderr, "      quantizer:  ");
            PrintValues(stats->segment_quant);
            fprintf(stderr, "   filter level:  ");
            PrintValues(stats->segment_level);
            if (full_details) {
                fprintf(stderr, "------------------+---------");
                fprintf(stderr, "+---------+---------+---------+-----------------\n");
                fprintf(stderr, " segments total:  ");
                PrintByteCount(totals, stats->coded_size, NULL);
            }
        }
        if (stats->lossless_size > 0) {
            PrintFullLosslessInfo(stats, "alpha");
        }
    }
}

static void PrintMapInfo(const WebPPicture* const pic) {
    if (pic->extra_info != NULL) {
        const int mb_w = (pic->width + 15) / 16;
        const int mb_h = (pic->height + 15) / 16;
        const int type = pic->extra_info_type;
        int x, y;
        for (y = 0; y < mb_h; ++y) {
            for (x = 0; x < mb_w; ++x) {
                const int c = pic->extra_info[x + y * mb_w];
                if (type == 1) { // intra4/intra16
                    fprintf(stderr, "%c", "+."[c]);
                } else if (type == 2) { // segments
                    fprintf(stderr, "%c", ".-*X"[c]);
                } else if (type == 3) { // quantizers
                    fprintf(stderr, "%.2d ", c);
                } else if (type == 6 || type == 7) {
                    fprintf(stderr, "%3d ", c);
                } else {
                    fprintf(stderr, "0x%.2x ", c);
                }
            }
            fprintf(stderr, "\n");
        }
    }
}

//------------------------------------------------------------------------------

static int MyWriter(const uint8_t* data, size_t data_size, const WebPPicture* const pic) {
    FILE* const out = (FILE*)pic->custom_ptr;
    return data_size ? (fwrite(data, data_size, 1, out) == 1) : 1;
}

// Dumps a picture as a PGM file using the IMC4 layout.
static int DumpPicture(const WebPPicture* const picture, const char* PGM_name) {
    int y;
    const int uv_width = (picture->width + 1) / 2;
    const int uv_height = (picture->height + 1) / 2;
    const int stride = (picture->width + 1) & ~1;
    const int alpha_height = WebPPictureHasTransparency(picture) ? picture->height : 0;
    const int height = picture->height + uv_height + alpha_height;
    FILE* const f = fopen(PGM_name, "wb");
    if (f == NULL) return 0;
    fprintf(f, "P5\n%d %d\n255\n", stride, height);
    for (y = 0; y < picture->height; ++y) {
        if (fwrite(picture->y + y * picture->y_stride, picture->width, 1, f) != 1) return 0;
        if (picture->width & 1) fputc(0, f); // pad
    }
    for (y = 0; y < uv_height; ++y) {
        if (fwrite(picture->u + y * picture->uv_stride, uv_width, 1, f) != 1) return 0;
        if (fwrite(picture->v + y * picture->uv_stride, uv_width, 1, f) != 1) return 0;
    }
    for (y = 0; y < alpha_height; ++y) {
        if (fwrite(picture->a + y * picture->a_stride, picture->width, 1, f) != 1) return 0;
        if (picture->width & 1) fputc(0, f); // pad
    }
    fclose(f);
    return 1;
}

// -----------------------------------------------------------------------------
// Metadata writing.

enum {
    METADATA_EXIF = (1 << 0),
    METADATA_ICC = (1 << 1),
    METADATA_XMP = (1 << 2),
    METADATA_ALL = METADATA_EXIF | METADATA_ICC | METADATA_XMP
};

static const int kChunkHeaderSize = 8;
static const int kTagSize = 4;

static void PrintMetadataInfo(const Metadata* const metadata, int metadata_written) {
    if (metadata == NULL || metadata_written == 0) return;

    fprintf(stderr, "Metadata:\n");
    if (metadata_written & METADATA_ICC) {
        fprintf(stderr, "  * ICC profile:  %6d bytes\n", (int)metadata->iccp.size);
    }
    if (metadata_written & METADATA_EXIF) {
        fprintf(stderr, "  * EXIF data:    %6d bytes\n", (int)metadata->exif.size);
    }
    if (metadata_written & METADATA_XMP) {
        fprintf(stderr, "  * XMP data:     %6d bytes\n", (int)metadata->xmp.size);
    }
}

// Outputs, in little endian, 'num' bytes from 'val' to 'out'.
static int WriteLE(FILE* const out, uint32_t val, int num) {
    uint8_t buf[4];
    int i;
    for (i = 0; i < num; ++i) {
        buf[i] = (uint8_t)(val & 0xff);
        val >>= 8;
    }
    return (fwrite(buf, num, 1, out) == 1);
}

static int WriteLE24(FILE* const out, uint32_t val) {
    return WriteLE(out, val, 3);
}

static int WriteLE32(FILE* const out, uint32_t val) {
    return WriteLE(out, val, 4);
}

static int WriteMetadataChunk(FILE* const out, const char fourcc[4], const MetadataPayload* const payload) {
    const uint8_t zero = 0;
    const size_t need_padding = payload->size & 1;
    int ok = (fwrite(fourcc, kTagSize, 1, out) == 1);
    ok = ok && WriteLE32(out, (uint32_t)payload->size);
    ok = ok && (fwrite(payload->bytes, payload->size, 1, out) == 1);
    return ok && (fwrite(&zero, need_padding, need_padding, out) == need_padding);
}

// Sets 'flag' in 'vp8x_flags' and updates 'metadata_size' with the size of the
// chunk if there is metadata and 'keep' is true.
static int UpdateFlagsAndSize(
    const MetadataPayload* const payload, int keep, int flag, uint32_t* vp8x_flags, uint64_t* metadata_size) {
    if (keep && payload->bytes != NULL && payload->size > 0) {
        *vp8x_flags |= flag;
        *metadata_size += kChunkHeaderSize + payload->size + (payload->size & 1);
        return 1;
    }
    return 0;
}

// Writes a WebP file using the image contained in 'memory_writer' and the
// metadata from 'metadata'. Metadata is controlled by 'keep_metadata' and the
// availability in 'metadata'. Returns true on success.
// For details see doc/webp-container-spec.txt#extended-file-format.
static int WriteWebPWithMetadata(FILE* const out,
                                 const WebPPicture* const picture,
                                 const WebPMemoryWriter* const memory_writer,
                                 const Metadata* const metadata,
                                 int keep_metadata,
                                 int* const metadata_written) {
    const char kVP8XHeader[] = "VP8X\x0a\x00\x00\x00";
    const int kAlphaFlag = 0x10;
    const int kEXIFFlag = 0x08;
    const int kICCPFlag = 0x20;
    const int kXMPFlag = 0x04;
    const size_t kRiffHeaderSize = 12;
    const size_t kMaxChunkPayload = ~0 - kChunkHeaderSize - 1;
    const size_t kMinSize = kRiffHeaderSize + kChunkHeaderSize;
    uint32_t flags = 0;
    uint64_t metadata_size = 0;
    const int write_exif =
        UpdateFlagsAndSize(&metadata->exif, !!(keep_metadata & METADATA_EXIF), kEXIFFlag, &flags, &metadata_size);
    const int write_iccp =
        UpdateFlagsAndSize(&metadata->iccp, !!(keep_metadata & METADATA_ICC), kICCPFlag, &flags, &metadata_size);
    const int write_xmp =
        UpdateFlagsAndSize(&metadata->xmp, !!(keep_metadata & METADATA_XMP), kXMPFlag, &flags, &metadata_size);
    uint8_t* webp = memory_writer->mem;
    size_t webp_size = memory_writer->size;

    *metadata_written = 0;

    if (webp_size < kMinSize) return 0;
    if (webp_size - kChunkHeaderSize + metadata_size > kMaxChunkPayload) {
        fprintf(stderr,
                "Error! Addition of metadata would exceed "
                "container size limit.\n");
        return 0;
    }

    if (metadata_size > 0) {
        const int kVP8XChunkSize = 18;
        const int has_vp8x = !memcmp(webp + kRiffHeaderSize, "VP8X", kTagSize);
        const uint32_t riff_size =
            (uint32_t)(webp_size - kChunkHeaderSize + (has_vp8x ? 0 : kVP8XChunkSize) + metadata_size);
        // RIFF
        int ok = (fwrite(webp, kTagSize, 1, out) == 1);
        // RIFF size (file header size is not recorded)
        ok = ok && WriteLE32(out, riff_size);
        webp += kChunkHeaderSize;
        webp_size -= kChunkHeaderSize;
        // WEBP
        ok = ok && (fwrite(webp, kTagSize, 1, out) == 1);
        webp += kTagSize;
        webp_size -= kTagSize;
        if (has_vp8x) { // update the existing VP8X flags
            webp[kChunkHeaderSize] |= (uint8_t)(flags & 0xff);
            ok = ok && (fwrite(webp, kVP8XChunkSize, 1, out) == 1);
            webp += kVP8XChunkSize;
            webp_size -= kVP8XChunkSize;
        } else {
            const int is_lossless = !memcmp(webp, "VP8L", kTagSize);
            if (is_lossless) {
                // Presence of alpha is stored in the 29th bit of VP8L data.
                if (webp[kChunkHeaderSize + 3] & (1 << 5)) flags |= kAlphaFlag;
            }
            ok = ok && (fwrite(kVP8XHeader, kChunkHeaderSize, 1, out) == 1);
            ok = ok && WriteLE32(out, flags);
            ok = ok && WriteLE24(out, picture->width - 1);
            ok = ok && WriteLE24(out, picture->height - 1);
        }
        if (write_iccp) {
            ok = ok && WriteMetadataChunk(out, "ICCP", &metadata->iccp);
            *metadata_written |= METADATA_ICC;
        }
        // Image
        ok = ok && (fwrite(webp, webp_size, 1, out) == 1);
        if (write_exif) {
            ok = ok && WriteMetadataChunk(out, "EXIF", &metadata->exif);
            *metadata_written |= METADATA_EXIF;
        }
        if (write_xmp) {
            ok = ok && WriteMetadataChunk(out, "XMP ", &metadata->xmp);
            *metadata_written |= METADATA_XMP;
        }
        return ok;
    } else {
        // No metadata, just write the original image file.
        return (fwrite(webp, webp_size, 1, out) == 1);
    }
}

//------------------------------------------------------------------------------

static int ProgressReport(int percent, const WebPPicture* const picture) {
    fprintf(stderr, "[%s]: %3d %%      \r", (char*)picture->user_data, percent);
    return 1; // all ok
}

//------------------------------------------------------------------------------

static void HelpShort(void) {
    printf("Usage:\n\n");
    printf("   cwebp [options] -q quality input.png -o output.webp\n\n");
    printf("where quality is between 0 (poor) to 100 (very good).\n");
    printf("Typical value is around 80.\n\n");
    printf("Try -longhelp for an exhaustive list of advanced options.\n");
}

static void HelpLong(void) {
    printf("Usage:\n");
    printf(" cwebp [-preset <...>] [options] in_file [-o out_file]\n\n");
    printf(
        "If input size (-s) for an image is not specified, it is\n"
        "assumed to be a PNG, JPEG, TIFF or WebP file.\n");
#ifdef HAVE_WINCODEC_H
    printf("Windows builds can take as input any of the files handled by WIC.\n");
#endif
    printf("\nOptions:\n");
    printf("  -h / -help  ............ short help\n");
    printf("  -H / -longhelp  ........ long help\n");
    printf("  -q <float> ............. quality factor (0:small..100:big)\n");
    printf(
        "  -alpha_q <int> ......... transparency-compression quality "
        "(0..100)\n");
    printf("  -preset <string> ....... preset setting, one of:\n");
    printf("                            default, photo, picture,\n");
    printf("                            drawing, icon, text\n");
    printf("     -preset must come first, as it overwrites other parameters\n");
    printf(
        "  -z <int> ............... activates lossless preset with given\n"
        "                           level in [0:fast, ..., 9:slowest]\n");
    printf("\n");
    printf("  -m <int> ............... compression method (0=fast, 6=slowest)\n");
    printf("  -segments <int> ........ number of segments to use (1..4)\n");
    printf("  -size <int> ............ target size (in bytes)\n");
    printf("  -psnr <float> .......... target PSNR (in dB. typically: 42)\n");
    printf("\n");
    printf("  -s <int> <int> ......... input size (width x height) for YUV\n");
    printf("  -sns <int> ............. spatial noise shaping (0:off, 100:max)\n");
    printf("  -f <int> ............... filter strength (0=off..100)\n");
    printf(
        "  -sharpness <int> ....... "
        "filter sharpness (0:most .. 7:least sharp)\n");
    printf(
        "  -strong ................ use strong filter instead "
        "of simple (default)\n");
    printf("  -nostrong .............. use simple filter instead of strong\n");
    printf("  -partition_limit <int> . limit quality to fit the 512k limit on\n");
    printf(
        "                           "
        "the first partition (0=no degradation ... 100=full)\n");
    printf("  -pass <int> ............ analysis pass number (1..10)\n");
    printf("  -crop <x> <y> <w> <h> .. crop picture with the given rectangle\n");
    printf("  -resize <w> <h> ........ resize picture (after any cropping)\n");
    printf("  -mt .................... use multi-threading if available\n");
    printf("  -low_memory ............ reduce memory usage (slower encoding)\n");
    printf("  -map <int> ............. print map of extra info\n");
    printf("  -print_psnr ............ prints averaged PSNR distortion\n");
    printf("  -print_ssim ............ prints averaged SSIM distortion\n");
    printf("  -print_lsim ............ prints local-similarity distortion\n");
    printf("  -d <file.pgm> .......... dump the compressed output (PGM file)\n");
    printf("  -alpha_method <int> .... transparency-compression method (0..1)\n");
    printf("  -alpha_filter <string> . predictive filtering for alpha plane,\n");
    printf("                           one of: none, fast (default) or best\n");
    printf(
        "  -exact ................. preserve RGB values in transparent area"
        "\n");
    printf(
        "  -blend_alpha <hex> ..... blend colors against background color\n"
        "                           expressed as RGB values written in\n"
        "                           hexadecimal, e.g. 0xc0e0d0 for red=0xc0\n"
        "                           green=0xe0 and blue=0xd0\n");
    printf("  -noalpha ............... discard any transparency information\n");
    printf("  -lossless .............. encode image losslessly\n");
    printf(
        "  -near_lossless <int> ... use near-lossless image\n"
        "                           preprocessing (0..100=off)\n");
#ifdef WEBP_EXPERIMENTAL_FEATURES
    printf("  -delta_palettization ... use delta palettization\n");
#endif // WEBP_EXPERIMENTAL_FEATURES
    printf("  -hint <string> ......... specify image characteristics hint,\n");
    printf("                           one of: photo, picture or graph\n");

    printf("\n");
    printf("  -metadata <string> ..... comma separated list of metadata to\n");
    printf("                           ");
    printf("copy from the input to the output if present.\n");
    printf(
        "                           "
        "Valid values: all, none (default), exif, icc, xmp\n");

    printf("\n");
    printf("  -short ................. condense printed message\n");
    printf("  -quiet ................. don't print anything\n");
    printf("  -version ............... print version number and exit\n");
#ifndef WEBP_DLL
    printf("  -noasm ................. disable all assembly optimizations\n");
#endif
    printf(
        "  -v ..................... verbose, e.g. print encoding/decoding "
        "times\n");
    printf("  -progress .............. report encoding progress\n");
    printf("\n");
    printf("Experimental Options:\n");
    printf("  -jpeg_like ............. roughly match expected JPEG size\n");
    printf("  -af .................... auto-adjust filter strength\n");
    printf("  -pre <int> ............. pre-processing filter\n");
    printf("\n");
}

//------------------------------------------------------------------------------
// Error messages

static const char* const kErrorMessages[VP8_ENC_ERROR_LAST] = {
    "OK", "OUT_OF_MEMORY: Out of memory allocating objects",
    "BITSTREAM_OUT_OF_MEMORY: Out of memory re-allocating byte buffer",
    "NULL_PARAMETER: NULL parameter passed to function", "INVALID_CONFIGURATION: configuration is invalid",
    "BAD_DIMENSION: Bad picture dimension. Maximum width and height "
    "allowed is 16383 pixels.",
    "PARTITION0_OVERFLOW: Partition #0 is too big to fit 512k.\n"
    "To reduce the size of this partition, try using less segments "
    "with the -segments option, and eventually reduce the number of "
    "header bits using -partition_limit. More details are available "
    "in the manual (`man cwebp`)",
    "PARTITION_OVERFLOW: Partition is too big to fit 16M", "BAD_WRITE: Picture writer returned an I/O error",
    "FILE_TOO_BIG: File would be too big to fit in 4G", "USER_ABORT: encoding abort requested by user"};

//------------------------------------------------------------------------------
// #define RUN_XINLINX_HW

#ifdef RUN_XINLINX_HW

#define TEST_CASE_NUM 1
#define TEST_PARA_NUM 10
char* test_argv[TEST_CASE_NUM][TEST_PARA_NUM] = {
    {
        "cwebp.exe", "-near_lossless", "0", "-use_ocl", "-lossless", "../../1920x1080.png", "-o", "../../lossless.webp",
        NULL,
    } //,
      //{
      //  "cwebp.exe",
      //  "-q",
      //  "75",
      //  "../../1920x1080.png",
      //  "-o",
      //  "../../q75.webp",
      //  NULL,
      //},
      //{
      //  "cwebp.exe",
      //  "-m",
      //  "0",
      //  "-q",
      //  "75",
      //  "../../1920x1080.png",
      //  "-o",
      //  "../../q75m0.webp",
      //  NULL,
      //}
};

int org_main(int argc, const char* argv[]);

int main(int argc, const char* argv[]) {
    int ret = 0;
    int index_case = 0;
    for (index_case = 0; index_case < TEST_CASE_NUM; index_case++) {
        int i = 0;
        int _argc = 0;

        while (test_argv[index_case][++_argc] != NULL) {
        }
        // fprintf(stderr, "_argc:%d\n", _argc);
        ret = org_main(_argc, test_argv[index_case]);
        if (ret != 0) {
            return ret;
        }
        ResetEncodeProfilingData();
    }

    return 0;
}

int org_main(int argc, const char* argv[]) {
    int i = 0;
    for (i = 0; i < argc; i++) {
        fprintf(stderr, "%s ", argv[i]);
    }
    fprintf(stderr, "\n");
#else
int main(int argc, const char* argv[]) {
#endif
    for (int i = 0; i < argc; i++) printf("%s \n", argv[i]);
    int return_value = -1;
    const char *in_file = NULL, *out_dir = NULL, *dump_file = NULL;
    WebPPicture* picture = NULL;
    std::string* PicPoolList = NULL;
    std::ifstream PicRead1, PicRead2;
    int sTotal = 0;
    std::string stmp;
    FILE** out = NULL;
    int c;
    int short_output = 0;
    int quiet = 0;
    int keep_alpha = 1;
    int blend_alpha = 0;
    uint32_t background_color = 0xffffffu;
    int crop = 0, crop_x = 0, crop_y = 0, crop_w = 0, crop_h = 0;
    int resize_w = 0, resize_h = 0;
    int lossless_preset = 6;
    int use_lossless_preset = -1; // -1=unset, 0=don't use, 1=use it
    int show_progress = 0;
    int keep_metadata = 0;
    int metadata_written = 0;

    int print_distortion = -1; // -1=off, 0=PSNR, 1=SSIM, 2=LSIM

    int Numpic = 1;
    int Numbatch = 1;

    WebPPicture original_picture; // when PSNR or SSIM is requested

    WebPConfig config;
    WebPAuxStats stats;
    WebPMemoryWriter memory_writer;

    Metadata metadata;
    Stopwatch stop_watch;

    int kernel_status = 0;
    const char* xclbinpath;

    StopProfilingWatch watch;
    double watch_time;
    int watch_count;

    MetadataInit(&metadata);
    WebPMemoryWriterInit(&memory_writer);
    WebPConfigInit(&config);

    if (argc == 1) {
        HelpShort();
        return 0;
    }

// configure default number of threads
#ifdef WEBP_USE_THREAD
    config.thread_number = 0; // get_nprocs() > 16 ? 16 : get_nprocs();
#endif

    for (c = 1; c < argc; ++c) {
        int parse_error = 0;
        if (!strcmp(argv[c], "-h") || !strcmp(argv[c], "-help")) {
            HelpShort();
            return 0;
        } else if (!strcmp(argv[c], "-H") || !strcmp(argv[c], "-longhelp")) {
            HelpLong();
            return 0;
        } else if (!strcmp(argv[c], "-o") && c < argc - 1) {
            out_dir = argv[++c];
        } else if (!strcmp(argv[c], "-d") && c < argc - 1) {
            dump_file = argv[++c];
            config.show_compressed = 1;
        } else if (!strcmp(argv[c], "-print_psnr")) {
            config.show_compressed = 1;
            print_distortion = 0;
        } else if (!strcmp(argv[c], "-print_ssim")) {
            config.show_compressed = 1;
            print_distortion = 1;
        } else if (!strcmp(argv[c], "-print_lsim")) {
            config.show_compressed = 1;
            print_distortion = 2;
        } else if (!strcmp(argv[c], "-short")) {
            ++short_output;
        } else if (!strcmp(argv[c], "-s") && c < argc - 2) {
            /* picture[0].width = ExUtilGetInt(argv[++c], 0, &parse_error); */
            /* picture[0].height = ExUtilGetInt(argv[++c], 0, &parse_error); */
            /* if (picture[0].width > WEBP_MAX_DIMENSION || picture[0].width < 0 || */
            /*  picture[0].height > WEBP_MAX_DIMENSION ||  picture[0].height < 0) { */
            /*   fprintf(stderr, */
            /*        "Specified dimension (%d x %d) is out of range.\n", */
            /*        picture.width, picture.height); */
            /*   goto Error; */
            /* } */
        } else if (!strcmp(argv[c], "-m") && c < argc - 1) {
            config.method = ExUtilGetInt(argv[++c], 0, &parse_error);
            use_lossless_preset = 0; // disable -z option
        } else if (!strcmp(argv[c], "-n") && c < argc - 1) {
#ifdef WEBP_USE_THREAD
            config.thread_number = ExUtilGetInt(argv[++c], 0, &parse_error);
            config.thread_number = get_nprocs() > config.thread_number ? config.thread_number : get_nprocs();
#endif
        } else if (!strcmp(argv[c], "-q") && c < argc - 1) {
            config.quality = ExUtilGetFloat(argv[++c], &parse_error);
            use_lossless_preset = 0; // disable -z option
        } else if (!strcmp(argv[c], "-numpic") && c < argc - 1) {
            Numpic = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-numbatch") && c < argc - 1) {
            Numbatch = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-z") && c < argc - 1) {
            lossless_preset = ExUtilGetInt(argv[++c], 0, &parse_error);
            if (use_lossless_preset != 0) use_lossless_preset = 1;
        } else if (!strcmp(argv[c], "-alpha_q") && c < argc - 1) {
            config.alpha_quality = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-alpha_method") && c < argc - 1) {
            config.alpha_compression = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-alpha_cleanup")) {
            // This flag is obsolete, does opposite of -exact.
            config.exact = 0;
        } else if (!strcmp(argv[c], "-exact")) {
            config.exact = 1;
        } else if (!strcmp(argv[c], "-xclbin")) {
            xclbinpath = argv[++c];
        } else if (!strcmp(argv[c], "-blend_alpha") && c < argc - 1) {
            blend_alpha = 1;
            // background color is given in hex with an optional '0x' prefix
            background_color = ExUtilGetInt(argv[++c], 16, &parse_error);
            background_color = background_color & 0x00ffffffu;
        } else if (!strcmp(argv[c], "-alpha_filter") && c < argc - 1) {
            ++c;
            if (!strcmp(argv[c], "none")) {
                config.alpha_filtering = 0;
            } else if (!strcmp(argv[c], "fast")) {
                config.alpha_filtering = 1;
            } else if (!strcmp(argv[c], "best")) {
                config.alpha_filtering = 2;
            } else {
                fprintf(stderr, "Error! Unrecognized alpha filter: %s\n", argv[c]);
                goto_Error;
            }
        } else if (!strcmp(argv[c], "-noalpha")) {
            keep_alpha = 0;
        } else if (!strcmp(argv[c], "-lossless")) {
            config.lossless = 1;
        } else if (!strcmp(argv[c], "-near_lossless") && c < argc - 1) {
            config.near_lossless = ExUtilGetInt(argv[++c], 0, &parse_error);
            config.lossless = 1; // use near-lossless only with lossless
#ifdef WEBP_EXPERIMENTAL_FEATURES
        } else if (!strcmp(argv[c], "-delta_palettization")) {
            config.delta_palettization = 1;
            config.lossless = 1; // use delta-palettization only with lossless
#endif                           // WEBP_EXPERIMENTAL_FEATURES
        } else if (!strcmp(argv[c], "-hint") && c < argc - 1) {
            ++c;
            if (!strcmp(argv[c], "photo")) {
                config.image_hint = WEBP_HINT_PHOTO;
            } else if (!strcmp(argv[c], "picture")) {
                config.image_hint = WEBP_HINT_PICTURE;
            } else if (!strcmp(argv[c], "graph")) {
                config.image_hint = WEBP_HINT_GRAPH;
            } else {
                fprintf(stderr, "Error! Unrecognized image hint: %s\n", argv[c]);
                goto_Error;
            }
        } else if (!strcmp(argv[c], "-size") && c < argc - 1) {
            config.target_size = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-psnr") && c < argc - 1) {
            config.target_PSNR = ExUtilGetFloat(argv[++c], &parse_error);
        } else if (!strcmp(argv[c], "-sns") && c < argc - 1) {
            config.sns_strength = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-f") && c < argc - 1) {
            config.filter_strength = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-af")) {
            config.autofilter = 1;
        } else if (!strcmp(argv[c], "-jpeg_like")) {
            config.emulate_jpeg_size = 1;
        } else if (!strcmp(argv[c], "-mt")) {
            ++config.thread_level; // increase thread level
        } else if (!strcmp(argv[c], "-low_memory")) {
            config.low_memory = 1;
        } else if (!strcmp(argv[c], "-strong")) {
            config.filter_type = 1;
        } else if (!strcmp(argv[c], "-nostrong")) {
            config.filter_type = 0;
        } else if (!strcmp(argv[c], "-sharpness") && c < argc - 1) {
            config.filter_sharpness = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-pass") && c < argc - 1) {
            config.pass = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-pre") && c < argc - 1) {
            config.preprocessing = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-segments") && c < argc - 1) {
            config.segments = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-partition_limit") && c < argc - 1) {
            config.partition_limit = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-use_ocl")) {
            config.use_ocl = 1;
        } else if (!strcmp(argv[c], "-map") && c < argc - 1) {
            // picture.extra_info_type = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-crop") && c < argc - 4) {
            crop = 1;
            crop_x = ExUtilGetInt(argv[++c], 0, &parse_error);
            crop_y = ExUtilGetInt(argv[++c], 0, &parse_error);
            crop_w = ExUtilGetInt(argv[++c], 0, &parse_error);
            crop_h = ExUtilGetInt(argv[++c], 0, &parse_error);
        } else if (!strcmp(argv[c], "-resize") && c < argc - 2) {
            resize_w = ExUtilGetInt(argv[++c], 0, &parse_error);
            resize_h = ExUtilGetInt(argv[++c], 0, &parse_error);
#ifndef WEBP_DLL
        } else if (!strcmp(argv[c], "-noasm")) {
            VP8GetCPUInfo = NULL;
#endif
        } else if (!strcmp(argv[c], "-version")) {
            const int version = WebPGetEncoderVersion();
            printf("%d.%d.%d\n", (version >> 16) & 0xff, (version >> 8) & 0xff, version & 0xff);
            return 0;
        } else if (!strcmp(argv[c], "-progress")) {
            show_progress = 1;
        } else if (!strcmp(argv[c], "-quiet")) {
            quiet = 1;
        } else if (!strcmp(argv[c], "-preset") && c < argc - 1) {
            WebPPreset preset;
            ++c;
            if (!strcmp(argv[c], "default")) {
                preset = WEBP_PRESET_DEFAULT;
            } else if (!strcmp(argv[c], "photo")) {
                preset = WEBP_PRESET_PHOTO;
            } else if (!strcmp(argv[c], "picture")) {
                preset = WEBP_PRESET_PICTURE;
            } else if (!strcmp(argv[c], "drawing")) {
                preset = WEBP_PRESET_DRAWING;
            } else if (!strcmp(argv[c], "icon")) {
                preset = WEBP_PRESET_ICON;
            } else if (!strcmp(argv[c], "text")) {
                preset = WEBP_PRESET_TEXT;
            } else {
                fprintf(stderr, "Error! Unrecognized preset: %s\n", argv[c]);
                goto_Error;
            }
            if (!WebPConfigPreset(&config, preset, config.quality)) {
                fprintf(stderr, "Error! Could initialize configuration with preset.\n");
                goto_Error;
            }
        } else if (!strcmp(argv[c], "-metadata") && c < argc - 1) {
            static const struct {
                const char* option;
                int flag;
            } kTokens[] = {
                {"all", METADATA_ALL}, {"none", 0},           {"exif", METADATA_EXIF},
                {"icc", METADATA_ICC}, {"xmp", METADATA_XMP},
            };
            const size_t kNumTokens = sizeof(kTokens) / sizeof(kTokens[0]);
            const char* start = argv[++c];
            const char* const end = start + strlen(start);

            while (start < end) {
                size_t i;
                const char* token = strchr(start, ',');
                if (token == NULL) token = end;

                for (i = 0; i < kNumTokens; ++i) {
                    if ((size_t)(token - start) == strlen(kTokens[i].option) &&
                        !strncmp(start, kTokens[i].option, strlen(kTokens[i].option))) {
                        if (kTokens[i].flag != 0) {
                            keep_metadata |= kTokens[i].flag;
                        } else {
                            keep_metadata = 0;
                        }
                        break;
                    }
                }
                if (i == kNumTokens) {
                    fprintf(stderr, "Error! Unknown metadata type '%.*s'\n", (int)(token - start), start);
                    HelpLong();
                    return -1;
                }
                start = token + 1;
            }
#ifdef HAVE_WINCODEC_H
            if (keep_metadata != 0 && keep_metadata != METADATA_ICC) {
                // TODO(jzern): remove when -metadata is supported on all platforms.
                fprintf(stderr,
                        "Warning: only ICC profile extraction is currently"
                        " supported on this platform!\n");
            }
#endif
        } else if (!strcmp(argv[c], "-v")) {
            verbose = 1;
        } else if (!strcmp(argv[c], "--")) {
            if (c < argc - 1) in_file = argv[++c];
            break;
        } else if (argv[c][0] == '-') {
            fprintf(stderr, "Error! Unknown option '%s'\n", argv[c]);
            HelpLong();
            return -1;
        } else {
            in_file = argv[c];
        }

        if (parse_error) {
            HelpLong();
            return -1;
        }
    }

    if (in_file == NULL) {
        fprintf(stderr, "No input file specified!\n");
        HelpShort();
        goto_Error;
    }

    picture = new WebPPicture[Numpic];

    // Init pictures
    for (int i = 0; i < Numpic; i++) {
        if (!WebPPictureInit(&picture[i]) || !WebPPictureInit(&original_picture)) {
            fprintf(stderr, "Error! Version mismatch!\n");
            return -1;
        }
    };

    // Read pictures name list from file to PicPoolList
    PicPoolList = new std::string[Numpic];

    PicRead1 = std::ifstream(in_file); // Get number of pictures in list "in_file"

    while (!PicRead1.eof()) {
        getline(PicRead1, stmp);
        sTotal++;
    }
    PicRead1.close();

    if (sTotal < Numpic) {
        fprintf(stderr, "ERROR: There is not enought pictures in %s\n", in_file);
        return 0;
    }

    PicRead2 = std::ifstream(in_file); // Get pictures names from list "in_file"
    for (int i = 0; i < Numpic; i++) {
        std::getline(PicRead2, PicPoolList[i]);
    };
    PicRead2.close();

    for (int i = 0; i < Numpic; i++) std::cout << PicPoolList[i] << std::endl;

    if (use_lossless_preset == 1) {
        if (!WebPConfigLosslessPreset(&config, lossless_preset)) {
            fprintf(stderr, "Invalid lossless preset (-z %d)\n", lossless_preset);
            goto_Error;
        }
    }

    // Check for unsupported command line options for lossless mode and log
    // warning for such options.
    if (!quiet && config.lossless == 1) {
        if (config.target_size > 0 || config.target_PSNR > 0) {
            fprintf(stderr,
                    "Encoding for specified size or PSNR is not supported"
                    " for lossless encoding. Ignoring such option(s)!\n");
        }
        if (config.partition_limit > 0) {
            fprintf(stderr,
                    "Partition limit option is not required for lossless"
                    " encoding. Ignoring this option!\n");
        }
    }

    if (!WebPValidateConfig(&config)) {
        fprintf(stderr, "Error! Invalid configuration.\n");
        goto_Error;
    }

    // Read the input. We need to decide if we prefer ARGB or YUVA
    // samples, depending on the expected compression mode (this saves
    // some conversion steps).
    for (int i = 0; i < Numpic; i++) {
        picture[i].use_argb = (config.lossless || config.preprocessing > 0 || crop || (resize_w | resize_h) > 0);
        if (verbose) {
            StopwatchReset(&stop_watch);
        }
    }

    // Read pictures
    for (int i = 0; i < Numpic; i++) {
        int ok = ReadPicture(PicPoolList[i].c_str(), &picture[i], keep_alpha, (keep_metadata == 0) ? NULL : &metadata);
        if (!ok) {
            fprintf(stderr, "Error! Cannot read input picture file '%s'\n", in_file);
            goto_Error;
        }

        picture[i].progress_hook = (show_progress && !quiet) ? ProgressReport : NULL;

        if (blend_alpha) {
            WebPBlendAlpha(&picture[i], background_color);
        }
    };

    if (verbose) {
        const double read_time = StopwatchReadAndReset(&stop_watch);
        fprintf(stderr, "Time to read input: %.3fs\n", read_time);
    }

    // Open the output
    if (out_dir != NULL) {
        out = new FILE*[Numpic];

        /* const int use_stdout = !strcmp(out_file, "-"); */
        /* out = use_stdout ? ExUtilSetBinaryMode(stdout) : fopen(out_file, "wb"); */

        for (int i = 0; i < Numpic; i++) {
            // output file name
            int sl = PicPoolList[i].find_last_of("/\\");
            int spng = PicPoolList[i].find(".png");
            std::string out_file = std::string(out_dir) + "/" + PicPoolList[i].substr(sl + 1, spng - sl - 1) + ".webp";

            // std::cout << out_file << std::endl;

            strcpy(picture[i].custom_ptr_name, out_file.c_str());

            /* out[i] = fopen(out_file.c_str(),"wb"); */
            /* if (out[i] == NULL) { */
            /*   fprintf(stderr, "Error! Cannot open output file '%s'\n", out_file.c_str()); */
            /*   goto Error; */
            /* }  */
            /* else { */
            /*   if (!short_output && !quiet) { */
            /*  fprintf(stderr, "INFO: output file is '%s'\n", out_file.c_str()); */
            /*   } */
            /* } */

            if (keep_metadata == 0) {
                for (int i = 0; i < Numpic; i++) {
                    picture[i].writer = MyWriter;
                    // picture[i].custom_ptr = (void*)out[i];
                };
            } else {
                for (int i = 0; i < Numpic; i++) {
                    picture[i].writer = WebPMemoryWrite;
                    picture[i].custom_ptr = (void*)&memory_writer;
                };
            }
        }; // end of loop
    } else {
        out = NULL;
        if (!quiet && !short_output) {
            fprintf(stderr, "No output dir specified (no -o flag). Encoding will\n");
            fprintf(stderr, "be performed, but its results discarded.\n\n");
        }
    }

    if (!quiet) {
        for (int i = 0; i < Numpic; i++) {
            picture[i].stats = &stats;
            picture[i].user_data = (void*)PicPoolList[i].c_str();
        }
    }

    /* // Crop & resize. */
    /* if (verbose) { */
    /*   StopwatchReset(&stop_watch); */
    /* } */
    /* if (crop != 0) { */
    /*   // We use self-cropping using a view. */
    /*   if (!WebPPictureView(&picture, crop_x, crop_y, crop_w, crop_h, &picture)) { */
    /*  fprintf(stderr, "Error! Cannot crop picture\n"); */
    /*  goto Error; */
    /*   } */
    /* } */
    /* if ((resize_w | resize_h) > 0) { */
    /*   if (!WebPPictureRescale(&picture, resize_w, resize_h)) { */
    /*  fprintf(stderr, "Error! Cannot resize picture\n"); */
    /*  goto Error; */
    /*   } */
    /* } */
    /* if (verbose && (crop != 0 || (resize_w | resize_h) > 0)) { */
    /*   const double preproc_time = StopwatchReadAndReset(&stop_watch); */
    /*   fprintf(stderr, "Time to crop/resize picture: %.3fs\n", preproc_time); */
    /* } */

    /* if (picture.extra_info_type > 0) { */
    /*   AllocExtraInfo(&picture); */
    /* } */
    /* if (print_distortion >= 0) {  // Save original picture for later comparison */
    /*   WebPPictureCopy(&picture, &original_picture); */
    /* } */

    /* // Compress. */
    /* if (verbose) { */
    /*   StopwatchReset(&stop_watch); */
    /* } */

    // *********************** create buffer device, in ./create_kernel.c and ./create_kernel.h

    // If using opencl, create kernel first.
    if (config.use_ocl) {
        kernel_status = CreateKernel(xclbinpath); // create_kernel.c
        if (kernel_status) {
            fprintf(stderr, "Create kernel1 failed!\n");
            goto_Error;
        }
    }

    // If using opencl, set kernel arguments first.
    if (config.use_ocl) {
        kernel_status = CreateDeviceBuffers(Numbatch);
        if (kernel_status) {
            fprintf(stderr, "Create buffers failed!\n");
            goto_Error;
        }
    }

    //
    fprintf(stderr, "INFO: WebPEncodeAsync Starts... \n");
    StartProfiling(&watch);

    /*int ok = */ WebPEncodeAsync(Numpic, Numbatch, &config, &picture);
    /* if (ok) {  */
    /*   /\* fprintf(stderr, "Error! Cannot encode picture as WebP\n"); *\/ */
    /*   /\* fprintf(stderr, "Error code: %d (%s)\n", *\/ */
    /*   /\*          picture.error_code, kErrorMessages[picture.error_code]); *\/ */
    /*   /\* goto Error; *\/ */
    /* } */

    watch_time = 0.0;
    StopProfiling(&watch, &watch_time, &watch_count);
    fprintf(stderr, "INFO: WebPEncodeAsync Finished. Computation time is %f (ms) \n\n", watch_time);

    // ********************************************************************************************************************
    // //

    ReleaseDeviceBuffers();

    if (config.use_ocl) {
        releaseSoftware(software);
        releaseHardware(hardware);
    }

    ReleaseKernel();

    // display all profiling data
    // DisplayEncodeProfilingResult();

    // fprintf(stderr, "stop profiling.......\n");

    /* if (verbose) { */
    /*   const double encode_time = StopwatchReadAndReset(&stop_watch); */
    /*   fprintf(stderr, "Time to encode picture: %.3fs\n", encode_time); */
    /* } */

    /* // Write info */
    /* if (dump_file) { */
    /*   if (picture.use_argb) { */
    /*  fprintf(stderr, "Warning: can't dump file (-d option) in lossless mode."); */
    /*   } else if (!DumpPicture(&picture, dump_file)) { */
    /*  fprintf(stderr, "Warning, couldn't dump picture %s\n", dump_file); */
    /*   } */
    /* } */

    /* if (keep_metadata != 0) { */
    /*   if (out != NULL) { */
    /*  if (!WriteWebPWithMetadata(out, &picture, &memory_writer, */
    /*                             &metadata, keep_metadata, &metadata_written)) { */
    /*    fprintf(stderr, "Error writing WebP file with metadata!\n"); */
    /*    goto Error; */
    /*  } */
    /*   } else {  // output is disabled, just display the metadata stats. */
    /*  const struct { */
    /*    const MetadataPayload* const payload; */
    /*    int flag; */
    /*  } *iter, info[] = { */
    /*    { &metadata.exif, METADATA_EXIF }, */
    /*    { &metadata.iccp, METADATA_ICC }, */
    /*    { &metadata.xmp, METADATA_XMP }, */
    /*    { NULL, 0 } */
    /*  }; */
    /*  uint32_t unused1 = 0; */
    /*  uint64_t unused2 = 0; */

    /*  for (iter = info; iter->payload != NULL; ++iter) { */
    /*    if (UpdateFlagsAndSize(iter->payload, !!(keep_metadata & iter->flag), */
    /*                           0, &unused1, &unused2)) { */
    /*      metadata_written |= iter->flag; */
    /*    } */
    /*  } */
    /*   } */
    /* } */

    /* if (!quiet) { */
    /*   if (!short_output || print_distortion < 0) { */
    /*  if (config.lossless) { */
    /*    PrintExtraInfoLossless(&picture, short_output, in_file); */
    /*  } else { */
    /*    PrintExtraInfoLossy(&picture, short_output, config.low_memory, in_file); */
    /*  } */
    /*   } */
    /*   if (!short_output && picture.extra_info_type > 0) { */
    /*  PrintMapInfo(&picture); */
    /*   } */
    /*   if (print_distortion >= 0) {    // print distortion */
    /*  static const char* distortion_names[] = { "PSNR", "SSIM", "LSIM" }; */
    /*  float values[5]; */
    /*  if (picture.use_argb != original_picture.use_argb) { */
    /*    // Somehow, the WebPEncode() call converted the original picture. */
    /*    // We need to make both match before calling WebPPictureDistortion(). */
    /*    int ok = 0; */
    /*    if (picture.use_argb) { */
    /*      ok = WebPPictureYUVAToARGB(&original_picture); */
    /*    } else { */
    /*      ok = WebPPictureARGBToYUVA(&original_picture, WEBP_YUV420A); */
    /*    } */
    /*    if (!ok) { */
    /*      fprintf(stderr, "Error while converting original picture.\n"); */
    /*      goto Error; */
    /*    } */
    /*  } */
    /*  if (!WebPPictureDistortion(&picture, &original_picture, */
    /*                             print_distortion, values)) { */
    /*    fprintf(stderr, "Error while computing the distortion.\n"); */
    /*    goto Error; */
    /*  } */
    /*  if (!short_output) { */
    /*    fprintf(stderr, "%s: ", distortion_names[print_distortion]); */
    /*    if (picture.use_argb) { */
    /*      fprintf(stderr, "B:%.2f G:%.2f R:%.2f A:%.2f  Total:%.2f\n", */
    /*              values[0], values[1], values[2], values[3], values[4]); */
    /*    } else { */
    /*      fprintf(stderr, "Y:%.2f U:%.2f V:%.2f A:%.2f  Total:%.2f\n", */
    /*              values[0], values[1], values[2], values[3], values[4]); */
    /*    } */
    /*  } else { */
    /*    fprintf(stderr, "%7d %.4f\n", picture.stats->coded_size, values[4]); */
    /*  } */
    /*   } */
    /*   if (!short_output) { */
    /*  PrintMetadataInfo(&metadata, metadata_written); */
    /*   } */
    /* } */
    /* return_value = 0; */

    using namespace xf::common::utils_sw;
    Logger logger(std::cout, std::cerr);
    return_value = 0;
    return_value ? logger.error(Logger::Message::TEST_FAIL) : logger.info(Logger::Message::TEST_PASS);
    return return_value;

Error:
    WebPMemoryWriterClear(&memory_writer);
    // free(picture.extra_info);
    MetadataFree(&metadata);
    /* WebPPictureFree(&picture); */
    /* WebPPictureFree(&original_picture); */
    /* if (out != NULL && out != stdout) { */
    /*   fclose(out); */
    /* } */

    return_value = 1;
    return_value ? logger.error(Logger::Message::TEST_FAIL) : logger.info(Logger::Message::TEST_PASS);
    return return_value;
}

//------------------------------------------------------------------------------
