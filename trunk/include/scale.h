


#ifndef LIBYUV_INCLUDE_SCALE_H_
#define LIBYUV_INCLUDE_SCALE_H_


#include "basic_types.h"

#if defined(_MSC_VER)
#define ALIGN16(var) __declspec(align(16)) var
#else
#define ALIGN16(var) var __attribute__((aligned(16)))
#endif


namespace libyuv
{

class YuvScaler {
 public:
  // Scales a YUV 4:2:0 image from the input width and height to the
  // output width and height. If outh_offset is nonzero, the image is
  // offset by that many pixels and stretched to (outh - outh_offset * 2)
  // pixels high, instead of outh.
  // If interpolate is not set, a simple nearest-neighbor algorithm is
  // used. This produces basic (blocky) quality at the fastest speed.
  // If interpolate is set, interpolation is used to produce a better
  // quality image, at the expense of speed.
  // Returns true if successful.
  static bool Scale(const uint8 *in, int32 inw, int32 inh,
                    uint8 *out, int32 outw, int32 outh, int32 outh_offset,
                    bool interpolate);

  // Same, but specified in terms of each plane location and stride.
  static bool Scale(const uint8 *inY, const uint8 *inU, const uint8 *inV,
                    int32 istrideY, int32 istrideU, int32 istrideV,
                    int32 iwidth, int32 iheight,
                    uint8 *outY, uint8 *outU, uint8 *outV,
                    int32 ostrideY, int32 ostrideU, int32 ostrideV,
                    int32 owidth, int32 oheight,
                    bool interpolate);

  // For testing, allow disabling of optimizations.
  static void SetUseReferenceImpl(bool use) { use_reference_impl_ = use; }

 private:

  static bool use_reference_impl_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(YuvScaler);
};

} // namespace libyuv

#endif // LIBYUV_INCLUDE_SCALE_H_
