/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/convert_from_argb.h"

#include "libyuv/basic_types.h"
#include "libyuv/cpu_id.h"
#include "libyuv/format_conversion.h"
#include "libyuv/planar_functions.h"
#include "libyuv/row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// ARGB little endian (bgra in memory) to I444
LIBYUV_API
int ARGBToI444(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height) {
  if (!src_argb || !dst_y || !dst_u || !dst_v || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToYRow)(const uint8* src_argb, uint8* dst_y, int pix) =
      ARGBToYRow_C;
  void (*ARGBToUV444Row)(const uint8* src_argb, uint8* dst_u, uint8* dst_v,
                         int pix) = ARGBToUV444Row_C;
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16) &&
          IS_ALIGNED(dst_y, 16) && IS_ALIGNED(dst_stride_y, 16)) {
        ARGBToYRow = ARGBToYRow_SSSE3;
      }
    }
  }
#elif defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    ARGBToUV444Row = ARGBToUV444Row_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
      ARGBToUV444Row = ARGBToUV444Row_NEON;
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToUV444Row(src_argb, dst_u, dst_v, width);
    ARGBToYRow(src_argb, dst_y, width);
    src_argb += src_stride_argb;
    dst_y += dst_stride_y;
    dst_u += dst_stride_u;
    dst_v += dst_stride_v;
  }
  return 0;
}

// ARGB little endian (bgra in memory) to I422
LIBYUV_API
int ARGBToI422(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height) {
  if (!src_argb || !dst_y || !dst_u || !dst_v || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToUV422Row)(const uint8* src_argb, uint8* dst_u, uint8* dst_v,
                         int pix) = ARGBToUV422Row_C;
#if defined(HAS_ARGBTOUV422ROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToUV422Row = ARGBToUV422Row_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUV422Row = ARGBToUV422Row_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16) &&
          IS_ALIGNED(dst_y, 16) && IS_ALIGNED(dst_stride_y, 16)) {
        ARGBToUV422Row = ARGBToUV422Row_SSSE3;
      }
    }
  }
#endif

  void (*ARGBToYRow)(const uint8* src_argb, uint8* dst_y, int pix) =
      ARGBToYRow_C;
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16) &&
          IS_ALIGNED(dst_y, 16) && IS_ALIGNED(dst_stride_y, 16)) {
        ARGBToYRow = ARGBToYRow_SSSE3;
      }
    }
  }
#elif defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
    }
    if (width >= 16) {
      ARGBToUV422Row = ARGBToUV422Row_Any_NEON;
      if (IS_ALIGNED(width, 16)) {
        ARGBToUV422Row = ARGBToUV422Row_NEON;
      }
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToUV422Row(src_argb, dst_u, dst_v, width);
    ARGBToYRow(src_argb, dst_y, width);
    src_argb += src_stride_argb;
    dst_y += dst_stride_y;
    dst_u += dst_stride_u;
    dst_v += dst_stride_v;
  }
  return 0;
}

// ARGB little endian (bgra in memory) to I411
LIBYUV_API
int ARGBToI411(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height) {
  if (!src_argb || !dst_y || !dst_u || !dst_v || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToUV411Row)(const uint8* src_argb, uint8* dst_u, uint8* dst_v,
                         int pix) = ARGBToUV411Row_C;
  void (*ARGBToYRow)(const uint8* src_argb, uint8* dst_y, int pix) =
      ARGBToYRow_C;
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16) &&
          IS_ALIGNED(dst_y, 16) && IS_ALIGNED(dst_stride_y, 16)) {
        ARGBToYRow = ARGBToYRow_SSSE3;
      }
    }
  }
#elif defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
    }
    if (width >= 32) {
      ARGBToUV411Row = ARGBToUV411Row_Any_NEON;
      if (IS_ALIGNED(width, 32)) {
        ARGBToUV411Row = ARGBToUV411Row_NEON;
      }
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToUV411Row(src_argb, dst_u, dst_v, width);
    ARGBToYRow(src_argb, dst_y, width);
    src_argb += src_stride_argb;
    dst_y += dst_stride_y;
    dst_u += dst_stride_u;
    dst_v += dst_stride_v;
  }
  return 0;
}

LIBYUV_API
int ARGBToNV12(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_uv, int dst_stride_uv,
               int width, int height) {
  if (!src_argb ||
      !dst_y || !dst_uv ||
      width <= 0 || height == 0 ||
      width > kMaxStride) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToUVRow)(const uint8* src_argb0, int src_stride_argb,
                      uint8* dst_u, uint8* dst_v, int width) = ARGBToUVRow_C;
  void (*ARGBToYRow)(const uint8* src_argb, uint8* dst_y, int pix) =
      ARGBToYRow_C;
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToUVRow = ARGBToUVRow_Any_SSSE3;
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_Unaligned_SSSE3;
      ARGBToYRow = ARGBToYRow_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16)) {
        ARGBToUVRow = ARGBToUVRow_SSSE3;
        if (IS_ALIGNED(dst_y, 16) && IS_ALIGNED(dst_stride_y, 16)) {
          ARGBToYRow = ARGBToYRow_SSSE3;
        }
      }
    }
  }
#elif defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
      if (IS_ALIGNED(width, 16)) {
        ARGBToUVRow = ARGBToUVRow_NEON;
      }
    }
  }
#endif
  int halfwidth = (width + 1) >> 1;
  void (*MergeUV)(const uint8* src_u, const uint8* src_v, uint8* dst_uv,
                  int width) = MergeUV_C;
#if defined(HAS_MERGEUV_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && halfwidth >= 16) {
    MergeUV = MergeUV_Any_SSE2;
    if (IS_ALIGNED(halfwidth, 16)) {
      MergeUV = MergeUV_Unaligned_SSE2;
      if (IS_ALIGNED(dst_uv, 16) && IS_ALIGNED(dst_stride_uv, 16)) {
        MergeUV = MergeUV_SSE2;
      }
    }
  }
#endif
#if defined(HAS_MERGEUV_AVX2)
  if (TestCpuFlag(kCpuHasAVX2) && halfwidth >= 32) {
    MergeUV = MergeUV_Any_AVX2;
    if (IS_ALIGNED(halfwidth, 32)) {
      MergeUV = MergeUV_Unaligned_AVX2;
      if (IS_ALIGNED(dst_uv, 32) && IS_ALIGNED(dst_stride_uv, 32)) {
        MergeUV = MergeUV_AVX2;
      }
    }
  }
#endif
#if defined(HAS_MERGEUV_NEON)
  if (TestCpuFlag(kCpuHasNEON) && halfwidth >= 16) {
    MergeUV = MergeUV_Any_NEON;
    if (IS_ALIGNED(halfwidth, 16)) {
      MergeUV = MergeUV_Unaligned_NEON;
      if (IS_ALIGNED(dst_uv, 16) && IS_ALIGNED(dst_stride_uv, 16)) {
        MergeUV = MergeUV_NEON;
      }
    }
  }
#endif

  SIMD_ALIGNED(uint8 row_u[kMaxStride / 2]);
  SIMD_ALIGNED(uint8 row_v[kMaxStride / 2]);

  for (int y = 0; y < height - 1; y += 2) {
    ARGBToUVRow(src_argb, src_stride_argb, row_u, row_v, width);
    MergeUV(row_u, row_v, dst_uv, halfwidth);
    ARGBToYRow(src_argb, dst_y, width);
    ARGBToYRow(src_argb + src_stride_argb, dst_y + dst_stride_y, width);
    src_argb += src_stride_argb * 2;
    dst_y += dst_stride_y * 2;
    dst_uv += dst_stride_uv;
  }
  if (height & 1) {
    ARGBToUVRow(src_argb, 0, row_u, row_v, width);
    MergeUV(row_u, row_v, dst_uv, halfwidth);
    ARGBToYRow(src_argb, dst_y, width);
    ARGBToYRow(src_argb + 0, dst_y + dst_stride_y, width);
  }
  return 0;
}

// Same as NV12 but U and V swapped.
LIBYUV_API
int ARGBToNV21(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_uv, int dst_stride_uv,
               int width, int height) {
  if (!src_argb ||
      !dst_y || !dst_uv ||
      width <= 0 || height == 0 ||
      width > kMaxStride) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToUVRow)(const uint8* src_argb0, int src_stride_argb,
                      uint8* dst_u, uint8* dst_v, int width) = ARGBToUVRow_C;
  void (*ARGBToYRow)(const uint8* src_argb, uint8* dst_y, int pix) =
      ARGBToYRow_C;
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToUVRow = ARGBToUVRow_Any_SSSE3;
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_Unaligned_SSSE3;
      ARGBToYRow = ARGBToYRow_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16)) {
        ARGBToUVRow = ARGBToUVRow_SSSE3;
        if (IS_ALIGNED(dst_y, 16) && IS_ALIGNED(dst_stride_y, 16)) {
          ARGBToYRow = ARGBToYRow_SSSE3;
        }
      }
    }
  }
#elif defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
      if (IS_ALIGNED(width, 16)) {
        ARGBToUVRow = ARGBToUVRow_NEON;
      }
    }
  }
#endif
  int halfwidth = (width + 1) >> 1;
  void (*MergeUV)(const uint8* src_u, const uint8* src_v, uint8* dst_uv,
                  int width) = MergeUV_C;
#if defined(HAS_MERGEUV_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && halfwidth >= 16) {
    MergeUV = MergeUV_Any_SSE2;
    if (IS_ALIGNED(halfwidth, 16)) {
      MergeUV = MergeUV_Unaligned_SSE2;
      if (IS_ALIGNED(dst_uv, 16) && IS_ALIGNED(dst_stride_uv, 16)) {
        MergeUV = MergeUV_SSE2;
      }
    }
  }
#endif
#if defined(HAS_MERGEUV_AVX2)
  if (TestCpuFlag(kCpuHasAVX2) && halfwidth >= 32) {
    MergeUV = MergeUV_Any_AVX2;
    if (IS_ALIGNED(halfwidth, 32)) {
      MergeUV = MergeUV_Unaligned_AVX2;
      if (IS_ALIGNED(dst_uv, 32) && IS_ALIGNED(dst_stride_uv, 32)) {
        MergeUV = MergeUV_AVX2;
      }
    }
  }
#endif
#if defined(HAS_MERGEUV_NEON)
  if (TestCpuFlag(kCpuHasNEON) && halfwidth >= 16) {
    MergeUV = MergeUV_Any_NEON;
    if (IS_ALIGNED(halfwidth, 16)) {
      MergeUV = MergeUV_Unaligned_NEON;
      if (IS_ALIGNED(dst_uv, 16) && IS_ALIGNED(dst_stride_uv, 16)) {
        MergeUV = MergeUV_NEON;
      }
    }
  }
#endif

  SIMD_ALIGNED(uint8 row_u[kMaxStride / 2]);
  SIMD_ALIGNED(uint8 row_v[kMaxStride / 2]);

  for (int y = 0; y < height - 1; y += 2) {
    ARGBToUVRow(src_argb, src_stride_argb, row_u, row_v, width);
    MergeUV(row_v, row_u, dst_uv, halfwidth);
    ARGBToYRow(src_argb, dst_y, width);
    ARGBToYRow(src_argb + src_stride_argb, dst_y + dst_stride_y, width);
    src_argb += src_stride_argb * 2;
    dst_y += dst_stride_y * 2;
    dst_uv += dst_stride_uv;
  }
  if (height & 1) {
    ARGBToUVRow(src_argb, 0, row_u, row_v, width);
    MergeUV(row_v, row_u, dst_uv, halfwidth);
    ARGBToYRow(src_argb, dst_y, width);
    ARGBToYRow(src_argb + 0, dst_y + dst_stride_y, width);
  }
  return 0;
}

// Convert ARGB to YUY2.
LIBYUV_API
int ARGBToYUY2(const uint8* src_argb, int src_stride_argb,
               uint8* dst_yuy2, int dst_stride_yuy2,
               int width, int height) {
  if (!src_argb || !dst_yuy2 ||
      width <= 0 || height == 0 ||
      width > kMaxStride) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_yuy2 = dst_yuy2 + (height - 1) * dst_stride_yuy2;
    dst_stride_yuy2 = -dst_stride_yuy2;
  }

  void (*ARGBToUVRow)(const uint8* src_argb0, int src_stride_argb,
                      uint8* dst_u, uint8* dst_v, int width) = ARGBToUVRow_C;
  void (*ARGBToYRow)(const uint8* src_argb, uint8* dst_y, int pix) =
      ARGBToYRow_C;
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToUVRow = ARGBToUVRow_Any_SSSE3;
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_Unaligned_SSSE3;
      ARGBToYRow = ARGBToYRow_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16)) {
        ARGBToUVRow = ARGBToUVRow_SSSE3;
        ARGBToYRow = ARGBToYRow_SSSE3;
      }
    }
  }
#elif defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
      if (IS_ALIGNED(width, 16)) {
        ARGBToUVRow = ARGBToUVRow_NEON;
      }
    }
  }
#endif

  void (*I422ToYUY2Row)(const uint8* src_y, const uint8* src_u,
                        const uint8* src_v, uint8* dst_yuy2, int width) =
      I422ToYUY2Row_C;
#if defined(HAS_I422TOYUY2ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && width >= 16 &&
      IS_ALIGNED(dst_yuy2, 16) && IS_ALIGNED(dst_stride_yuy2, 16)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_SSE2;
    }
  }
#elif defined(HAS_I422TOYUY2ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 16) {
    I422ToYUY2Row = I422ToYUY2Row_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_NEON;
    }
  }
#endif
  SIMD_ALIGNED(uint8 row_y[kMaxStride]);
  SIMD_ALIGNED(uint8 row_u[kMaxStride / 2]);
  SIMD_ALIGNED(uint8 row_v[kMaxStride / 2]);

  for (int y = 0; y < height; ++y) {
    ARGBToUVRow(src_argb, 0, row_u, row_v, width);
    ARGBToYRow(src_argb, row_y, width);
    I422ToYUY2Row(row_y, row_u, row_v, dst_yuy2, width);
    src_argb += src_stride_argb;
    dst_yuy2 += dst_stride_yuy2;
  }
  return 0;
}

// Convert ARGB to UYVY.
LIBYUV_API
int ARGBToUYVY(const uint8* src_argb, int src_stride_argb,
               uint8* dst_uyvy, int dst_stride_uyvy,
               int width, int height) {
  if (!src_argb || !dst_uyvy ||
      width <= 0 || height == 0 ||
      width > kMaxStride) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_uyvy = dst_uyvy + (height - 1) * dst_stride_uyvy;
    dst_stride_uyvy = -dst_stride_uyvy;
  }

  void (*ARGBToUVRow)(const uint8* src_argb0, int src_stride_argb,
                      uint8* dst_u, uint8* dst_v, int width) = ARGBToUVRow_C;
  void (*ARGBToYRow)(const uint8* src_argb, uint8* dst_y, int pix) =
      ARGBToYRow_C;
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToUVRow = ARGBToUVRow_Any_SSSE3;
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_Unaligned_SSSE3;
      ARGBToYRow = ARGBToYRow_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16)) {
        ARGBToUVRow = ARGBToUVRow_SSSE3;
        ARGBToYRow = ARGBToYRow_SSSE3;
      }
    }
  }
#elif defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
      if (IS_ALIGNED(width, 16)) {
        ARGBToUVRow = ARGBToUVRow_NEON;
      }
    }
  }
#endif

  void (*I422ToUYVYRow)(const uint8* src_y, const uint8* src_u,
                        const uint8* src_v, uint8* dst_uyvy, int width) =
      I422ToUYVYRow_C;
#if defined(HAS_I422TOUYVYROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && width >= 16 &&
      IS_ALIGNED(dst_uyvy, 16) && IS_ALIGNED(dst_stride_uyvy, 16)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_SSE2;
    }
  }
#elif defined(HAS_I422TOUYVYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 16) {
    I422ToUYVYRow = I422ToUYVYRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_NEON;
    }
  }
#endif
  SIMD_ALIGNED(uint8 row_y[kMaxStride]);
  SIMD_ALIGNED(uint8 row_u[kMaxStride / 2]);
  SIMD_ALIGNED(uint8 row_v[kMaxStride / 2]);

  for (int y = 0; y < height; ++y) {
    ARGBToUVRow(src_argb, 0, row_u, row_v, width);
    ARGBToYRow(src_argb, row_y, width);
    I422ToUYVYRow(row_y, row_u, row_v, dst_uyvy, width);
    src_argb += src_stride_argb;
    dst_uyvy += dst_stride_uyvy;
  }
  return 0;
}

// Convert ARGB to I400.
LIBYUV_API
int ARGBToI400(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               int width, int height) {
  if (!src_argb || !dst_y || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToYRow)(const uint8* src_argb, uint8* dst_y, int pix) =
      ARGBToYRow_C;
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16) {
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_Unaligned_SSSE3;
      if (IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16) &&
          IS_ALIGNED(dst_y, 16) && IS_ALIGNED(dst_stride_y, 16)) {
        ARGBToYRow = ARGBToYRow_SSSE3;
      }
    }
  }
#elif defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToYRow(src_argb, dst_y, width);
    src_argb += src_stride_argb;
    dst_y += dst_stride_y;
  }
  return 0;
}

// Convert ARGB to RGBA.
LIBYUV_API
int ARGBToRGBA(const uint8* src_argb, int src_stride_argb,
               uint8* dst_rgba, int dst_stride_rgba,
               int width, int height) {
  if (!src_argb || !dst_rgba ||
      width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToRGBARow)(const uint8* src_argb, uint8* dst_rgba, int pix) =
      ARGBToRGBARow_C;
#if defined(HAS_ARGBTORGBAROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) &&
      IS_ALIGNED(width, 4) &&
      IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16) &&
      IS_ALIGNED(dst_rgba, 16) && IS_ALIGNED(dst_stride_rgba, 16)) {
    ARGBToRGBARow = ARGBToRGBARow_SSSE3;
  }
#elif defined(HAS_ARGBTORGBAROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && IS_ALIGNED(width, 8)) {
    ARGBToRGBARow = ARGBToRGBARow_NEON;
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToRGBARow(src_argb, dst_rgba, width);
    src_argb += src_stride_argb;
    dst_rgba += dst_stride_rgba;
  }
  return 0;
}

// Convert ARGB To RGB24.
LIBYUV_API
int ARGBToRGB24(const uint8* src_argb, int src_stride_argb,
                uint8* dst_rgb24, int dst_stride_rgb24,
                int width, int height) {
  if (!src_argb || !dst_rgb24 || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToRGB24Row)(const uint8* src_argb, uint8* dst_rgb, int pix) =
      ARGBToRGB24Row_C;
#if defined(HAS_ARGBTORGB24ROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16 &&
      IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16) &&
      IS_ALIGNED(dst_rgb24, 16) && IS_ALIGNED(dst_stride_rgb24, 16)) {
    ARGBToRGB24Row = ARGBToRGB24Row_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToRGB24Row = ARGBToRGB24Row_SSSE3;
    }
  }
#elif defined(HAS_ARGBTORGB24ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToRGB24Row = ARGBToRGB24Row_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToRGB24Row = ARGBToRGB24Row_NEON;
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToRGB24Row(src_argb, dst_rgb24, width);
    src_argb += src_stride_argb;
    dst_rgb24 += dst_stride_rgb24;
  }
  return 0;
}

// Convert ARGB To RAW.
LIBYUV_API
int ARGBToRAW(const uint8* src_argb, int src_stride_argb,
              uint8* dst_raw, int dst_stride_raw,
              int width, int height) {
  if (!src_argb || !dst_raw || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToRAWRow)(const uint8* src_argb, uint8* dst_rgb, int pix) =
      ARGBToRAWRow_C;
#if defined(HAS_ARGBTORAWROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && width >= 16 &&
      IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16) &&
      IS_ALIGNED(dst_raw, 16) && IS_ALIGNED(dst_stride_raw, 16)) {
    ARGBToRAWRow = ARGBToRAWRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToRAWRow = ARGBToRAWRow_SSSE3;
    }
  }
#elif defined(HAS_ARGBTORAWROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToRAWRow = ARGBToRAWRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToRAWRow = ARGBToRAWRow_NEON;
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToRAWRow(src_argb, dst_raw, width);
    src_argb += src_stride_argb;
    dst_raw += dst_stride_raw;
  }
  return 0;
}

// Convert ARGB To RGB565.
LIBYUV_API
int ARGBToRGB565(const uint8* src_argb, int src_stride_argb,
                 uint8* dst_rgb565, int dst_stride_rgb565,
                 int width, int height) {
  if (!src_argb || !dst_rgb565 || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToRGB565Row)(const uint8* src_argb, uint8* dst_rgb, int pix) =
      ARGBToRGB565Row_C;
#if defined(HAS_ARGBTORGB565ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && width >= 4 &&
      IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16)) {
    ARGBToRGB565Row = ARGBToRGB565Row_Any_SSE2;
    if (IS_ALIGNED(width, 4)) {
      ARGBToRGB565Row = ARGBToRGB565Row_SSE2;
    }
  }
#elif defined(HAS_ARGBTORGB565ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToRGB565Row = ARGBToRGB565Row_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToRGB565Row = ARGBToRGB565Row_NEON;
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToRGB565Row(src_argb, dst_rgb565, width);
    src_argb += src_stride_argb;
    dst_rgb565 += dst_stride_rgb565;
  }
  return 0;
}

// Convert ARGB To ARGB1555.
LIBYUV_API
int ARGBToARGB1555(const uint8* src_argb, int src_stride_argb,
                   uint8* dst_argb1555, int dst_stride_argb1555,
                   int width, int height) {
  if (!src_argb || !dst_argb1555 || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToARGB1555Row)(const uint8* src_argb, uint8* dst_rgb, int pix) =
      ARGBToARGB1555Row_C;
#if defined(HAS_ARGBTOARGB1555ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && width >= 4 &&
      IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16)) {
    ARGBToARGB1555Row = ARGBToARGB1555Row_Any_SSE2;
    if (IS_ALIGNED(width, 4)) {
      ARGBToARGB1555Row = ARGBToARGB1555Row_SSE2;
    }
  }
#elif defined(HAS_ARGBTOARGB1555ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToARGB1555Row = ARGBToARGB1555Row_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToARGB1555Row = ARGBToARGB1555Row_NEON;
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToARGB1555Row(src_argb, dst_argb1555, width);
    src_argb += src_stride_argb;
    dst_argb1555 += dst_stride_argb1555;
  }
  return 0;
}

// Convert ARGB To ARGB4444.
LIBYUV_API
int ARGBToARGB4444(const uint8* src_argb, int src_stride_argb,
                   uint8* dst_argb4444, int dst_stride_argb4444,
                   int width, int height) {
  if (!src_argb || !dst_argb4444 || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  void (*ARGBToARGB4444Row)(const uint8* src_argb, uint8* dst_rgb, int pix) =
      ARGBToARGB4444Row_C;
#if defined(HAS_ARGBTOARGB4444ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && width >= 4 &&
      IS_ALIGNED(src_argb, 16) && IS_ALIGNED(src_stride_argb, 16)) {
    ARGBToARGB4444Row = ARGBToARGB4444Row_Any_SSE2;
    if (IS_ALIGNED(width, 4)) {
      ARGBToARGB4444Row = ARGBToARGB4444Row_SSE2;
    }
  }
#elif defined(HAS_ARGBTOARGB4444ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON) && width >= 8) {
    ARGBToARGB4444Row = ARGBToARGB4444Row_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToARGB4444Row = ARGBToARGB4444Row_NEON;
    }
  }
#endif

  for (int y = 0; y < height; ++y) {
    ARGBToARGB4444Row(src_argb, dst_argb4444, width);
    src_argb += src_stride_argb;
    dst_argb4444 += dst_stride_argb4444;
  }
  return 0;
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

