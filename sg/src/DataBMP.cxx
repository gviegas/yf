//
// SG
// DataBMP.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cwchar>
#include <cstring>
#include <fstream>

#include "yf/Except.h"

#include "DataBMP.h"

#if defined(_DEFAULT_SOURCE)
# include <endian.h>
inline uint16_t htole(uint16_t v) { return htole16(v); }
inline int16_t  htole(int16_t v)  { return htole16(v); }
inline uint32_t htole(uint32_t v) { return htole32(v); }
inline int32_t  htole(int32_t v)  { return htole32(v); }
inline uint16_t letoh(uint16_t v) { return le16toh(v); }
inline int16_t  letoh(int16_t v)  { return le16toh(v); }
inline uint32_t letoh(uint32_t v) { return le32toh(v); }
inline int32_t  letoh(int32_t v)  { return le32toh(v); }
#else
// TODO
# error "Invalid platform"
#endif

using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

// ------------------------------------------------------------------------
// XXX
#define BMPFH_PRINT(fhPtr, path) do { \
  wprintf(L"\n#BMP (debug)#"); \
  wprintf(L"\npathname: %s", path); \
  wprintf(L"\nfh - type: 0x%02x (%c%c)", \
    (fhPtr)->type, (fhPtr)->type, (fhPtr)->type >> 8); \
  wprintf(L"\nfh - size: %u", (fhPtr)->size); \
  wprintf(L"\nfh - reserved1: %u", (fhPtr)->reserved1); \
  wprintf(L"\nfh - reserved2: %u", (fhPtr)->reserved2); \
  wprintf(L"\nfh - dataOffset: %u\n", (fhPtr)->dataOffset); \
} while (0)

#define BMPIH_PRINT(ihPtr, path) do { \
  wprintf(L"\n#BMP (debug)#"); \
  wprintf(L"\npathname: %s", path); \
  wprintf(L"\nih - size: %u", (ihPtr)->size); \
  wprintf(L"\nih - width: %d", (ihPtr)->width); \
  wprintf(L"\nih - height: %d", (ihPtr)->height); \
  wprintf(L"\nih - planes: %u", (ihPtr)->planes); \
  wprintf(L"\nih - bpp: %u", (ihPtr)->bpp); \
  wprintf(L"\nih - compression: %u", (ihPtr)->compression); \
  wprintf(L"\nih - imageSize: %u", (ihPtr)->imageSize); \
  wprintf(L"\nih - ppmX: %d", (ihPtr)->ppmX); \
  wprintf(L"\nih - ppmY: %d", (ihPtr)->ppmY); \
  wprintf(L"\nih - ciN: %u", (ihPtr)->ciN); \
  wprintf(L"\nih - ciImportant: %u\n", (ihPtr)->ciImportant); \
} while (0)

#define BMPV4_PRINT(v4Ptr, path) do { \
  wprintf(L"\n#BMP (debug)#"); \
  wprintf(L"\npathname: %s", path); \
  wprintf(L"\nv4 - size: %u", (v4Ptr)->size); \
  wprintf(L"\nv4 - width: %d", (v4Ptr)->width); \
  wprintf(L"\nv4 - height: %d", (v4Ptr)->height); \
  wprintf(L"\nv4 - planes: %u", (v4Ptr)->planes); \
  wprintf(L"\nv4 - bpp: %u", (v4Ptr)->bpp); \
  wprintf(L"\nv4- compression: %u", (v4Ptr)->compression); \
  wprintf(L"\nv4 - imageSz: %u", (v4Ptr)->imageSize); \
  wprintf(L"\nv4 - ppmX: %d", (v4Ptr)->ppmX); \
  wprintf(L"\nv4 - ppmY: %d", (v4Ptr)->ppmY); \
  wprintf(L"\nv4 - ciN: %u", (v4Ptr)->ciN); \
  wprintf(L"\nv4 - ciImportant: %u", (v4Ptr)->ciImportant); \
  wprintf(L"\nv4 - maskR: 0x%08x", (v4Ptr)->maskR); \
  wprintf(L"\nv4 - maskG: 0x%08x", (v4Ptr)->maskG); \
  wprintf(L"\nv4 - maskB: 0x%08x", (v4Ptr)->maskB); \
  wprintf(L"\nv4 - maskA: 0x%08x", (v4Ptr)->maskA); \
  wprintf(L"\nv4 - csType: 0x%08x (%c%c%c%c)", \
    (v4Ptr)->csType, (v4Ptr)->csType, (v4Ptr)->csType >> 8, \
    (v4Ptr)->csType >> 16, (v4Ptr)->csType >> 24); \
  wprintf(L"\nv4h - endPoints: %u,%u,%u %u,%u,%u %u,%u,%u", \
    (v4Ptr)->endPoints[0], (v4Ptr)->endPoints[1], (v4Ptr)->endPoints[2], \
    (v4Ptr)->endPoints[3], (v4Ptr)->endPoints[4], (v4Ptr)->endPoints[5], \
    (v4Ptr)->endPoints[6], (v4Ptr)->endPoints[7], (v4Ptr)->endPoints[8]); \
  wprintf(L"\nv4 - gammaR: %u", (v4Ptr)->gammaR); \
  wprintf(L"\nv4 - gammaG: %u", (v4Ptr)->gammaG); \
  wprintf(L"\nv4 - gammaB: %u\n", (v4Ptr)->gammaB); \
} while (0)

#define BMPV5_PRINT(v5Ptr, path) do { \
  wprintf(L"\n#BMP (debug)#"); \
  wprintf(L"\npathname: %s", path); \
  wprintf(L"\nv5 - size: %u", (v5Ptr)->size); \
  wprintf(L"\nv5 - width: %d", (v5Ptr)->width); \
  wprintf(L"\nv5 - height: %d", (v5Ptr)->height); \
  wprintf(L"\nv5 - planes: %u", (v5Ptr)->planes); \
  wprintf(L"\nv5 - bpp: %u", (v5Ptr)->bpp); \
  wprintf(L"\nv5 - compression: %u", (v5Ptr)->compression); \
  wprintf(L"\nv5 - imageSize: %u", (v5Ptr)->imageSize); \
  wprintf(L"\nv5 - ppmX: %d", (v5Ptr)->ppmX); \
  wprintf(L"\nv5 - ppmY: %d", (v5Ptr)->ppmY); \
  wprintf(L"\nv5 - ciN: %u", (v5Ptr)->ciN); \
  wprintf(L"\nv5 - ciImportant: %u", (v5Ptr)->ciImportant); \
  wprintf(L"\nv5 - maskR: 0x%08x", (v5Ptr)->maskR); \
  wprintf(L"\nv5 - maskG: 0x%08x", (v5Ptr)->maskG); \
  wprintf(L"\nv5 - maskB: 0x%08x", (v5Ptr)->maskB); \
  wprintf(L"\nv5 - maskA: 0x%08x", (v5Ptr)->maskA); \
  wprintf(L"\nv5 - csType: 0x%08x (%c%c%c%c)", \
    (v5Ptr)->csType, (v5Ptr)->csType, (v5Ptr)->csType >> 8, \
    (v5Ptr)->csType >> 16, (v5Ptr)->csType >> 24); \
  wprintf(L"\nv5h - endPoints: %u,%u,%u %u,%u,%u %u,%u,%u", \
    (v5Ptr)->endPoints[0], (v5Ptr)->endPoints[1], (v5Ptr)->endPoints[2], \
    (v5Ptr)->endPoints[3], (v5Ptr)->endPoints[4], (v5Ptr)->endPoints[5], \
    (v5Ptr)->endPoints[6], (v5Ptr)->endPoints[7], (v5Ptr)->endPoints[8]); \
  wprintf(L"\nv5h - gammaR: %u", (v5Ptr)->gammaR); \
  wprintf(L"\nv5h - gammaG: %u", (v5Ptr)->gammaG); \
  wprintf(L"\nv5h - gammaB: %u", (v5Ptr)->gammaB); \
  wprintf(L"\nv5h - intent: %u", (v5Ptr)->intent); \
  wprintf(L"\nv5h - profileData: %u", (v5Ptr)->profileData); \
  wprintf(L"\nv5h - profileSize: %u", (v5Ptr)->profileSize); \
  wprintf(L"\nv5h - reserved: %u\n", (v5Ptr)->reserved); \
} while (0)
// ------------------------------------------------------------------------

/// BMP file header.
///
struct BMPfh {
  uint8_t _[2];
  uint16_t type;
  uint32_t size;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t dataOffset;
};
constexpr const uint32_t BMPfhSize = 14;
static_assert(offsetof(BMPfh, dataOffset) == BMPfhSize-4+2, "!offsetof");

/// BMP info header.
///
struct BMPih {
  uint32_t size;
  int32_t width;
  int32_t height;
  uint16_t planes;
  uint16_t bpp;
  uint32_t compression;
  uint32_t imageSize;
  int32_t ppmX;
  int32_t ppmY;
  uint32_t ciN;
  uint32_t ciImportant;
};
constexpr const uint32_t BMPihSize = 40;
static_assert(offsetof(BMPih, ciImportant) == BMPihSize-4, "!offsetof");

/// BMP version 4 header.
///
struct BMPv4 {
  uint32_t size;
  int32_t width;
  int32_t height;
  uint16_t planes;
  uint16_t bpp;
  uint32_t compression;
  uint32_t imageSize;
  int32_t ppmX;
  int32_t ppmY;
  uint32_t ciN;
  uint32_t ciImportant;
  uint32_t maskR;
  uint32_t maskG;
  uint32_t maskB;
  uint32_t maskA;
  uint32_t csType;
  uint32_t endPoints[9];
  uint32_t gammaR;
  uint32_t gammaG;
  uint32_t gammaB;
};
constexpr const uint32_t BMPv4Size = 108;
static_assert(offsetof(BMPv4, gammaB) == BMPv4Size-4, "!offsetof");

/// BMP version 5 header.
///
struct BMPv5 {
  uint32_t size;
  int32_t width;
  int32_t height;
  uint16_t planes;
  uint16_t bpp;
  uint32_t compression;
  uint32_t imageSize;
  int32_t ppmX;
  int32_t ppmY;
  uint32_t ciN;
  uint32_t ciImportant;
  uint32_t maskR;
  uint32_t maskG;
  uint32_t maskB;
  uint32_t maskA;
  uint32_t csType;
  uint32_t endPoints[9];
  uint32_t gammaR;
  uint32_t gammaG;
  uint32_t gammaB;
  uint32_t intent;
  uint32_t profileData;
  uint32_t profileSize;
  uint32_t reserved;
};
constexpr const uint32_t BMPv5Size = 124;
static_assert(offsetof(BMPv5, reserved) == BMPv5Size-4, "!offsetof");

/// BMP format definitions.
///
constexpr const uint32_t BMPType = 0x4d42;
constexpr const uint32_t BMPComprRgb = 0;
constexpr const uint32_t BMPComprBitFld = 3;

/// Counts the number of unset low bits in a mask.
///
inline uint32_t lshfBMP(uint32_t mask, uint32_t bpp) {
  uint32_t res = 0;
  while (res != bpp && (mask & (1 << res)) == 0)
    ++res;
  return res;
}

/// Counts the number of bits set in a mask.
///
inline uint32_t bitsBMP(uint32_t mask, uint32_t bpp, uint32_t lshf) {
  uint32_t res = bpp;
  while (res > lshf && (mask & (1 << (res-1))) == 0)
    --res;
  return res - lshf;
}

INTERNAL_NS_END

void SG_NS::loadBMP(Texture::Data& dst, const wstring& pathname) {
  // Convert pathname string
  char mpath[256];
  const wchar_t* wsrc = pathname.data();
  mbstate_t state;
  memset(&state, 0, sizeof state);
  wcsrtombs(mpath, &wsrc, sizeof mpath, &state);
  if (wsrc)
    throw LimitExcept("Could not convert BMP file path");

  ifstream ifs(mpath);
  if (!ifs)
    throw FileExcept("Could not open BMP file");

  // Get file header
  BMPfh fh;
  if (!ifs.read(reinterpret_cast<char*>(&fh.type), BMPfhSize))
    throw FileExcept("Could not read from BMP file");

  // XXX
  BMPFH_PRINT(&fh, mpath);

  // The header size tells which structure to use
  uint32_t hdrSize;
  if (!ifs.read(reinterpret_cast<char*>(&hdrSize), sizeof hdrSize))
    throw FileExcept("Could not read from BMP file");
  hdrSize = letoh(hdrSize);

  uint32_t readN = BMPfhSize + sizeof hdrSize;

  int32_t width;
  int32_t height;
  uint16_t bpp;
  uint32_t compression;
  uint32_t maskRgba[4];
  uint32_t lshfRgba[4];
  uint32_t bitsRgba[4];

  // Read next bytes as BMPih
  auto readIh = [&] {
    BMPih ih;
    ih.size = htole(hdrSize);
    const auto n = BMPihSize - sizeof hdrSize;
    if (!ifs.read(reinterpret_cast<char*>(&ih.width), n))
      throw FileExcept("Could not read from BMP file");

    readN += n;
    width = letoh(ih.width);
    height = letoh(ih.height);
    bpp = letoh(ih.bpp);
    compression = letoh(ih.compression);
    // info header only supports non-alpha colors
    maskRgba[3] = 0;
    lshfRgba[3] = bpp;
    bitsRgba[3] = 0;

    switch (compression) {
    case BMPComprRgb:
      break;
    case BMPComprBitFld:
      if (readN == letoh(fh.dataOffset) ||
          !ifs.read(reinterpret_cast<char*>(maskRgba), 3*sizeof *maskRgba))
        throw FileExcept("Invalid BMP file");
      readN += 3*sizeof *maskRgba;
      maskRgba[0] = letoh(maskRgba[0]);
      maskRgba[1] = letoh(maskRgba[1]);
      maskRgba[2] = letoh(maskRgba[2]);
      break;
    default:
      throw FileExcept("Invalid BMP file");
    }

    // XXX
    BMPIH_PRINT(&ih, mpath);
  };

  // Read next bytes as BMPv4
  auto readV4 = [&] {
    BMPv4 v4;
    v4.size = htole(hdrSize);
    const auto n = BMPv4Size - sizeof hdrSize;
    if (!ifs.read(reinterpret_cast<char*>(&v4.width), n))
      throw FileExcept("Could not read from BMP file");

    readN += n;
    width = letoh(v4.width);
    height = letoh(v4.height);
    bpp = letoh(v4.bpp);
    compression = letoh(v4.compression);
    // 16/32 bpp formats have alpha channel
    maskRgba[3] = (bpp == 16 || bpp == 32) ? letoh(v4.maskA) : 0;
    lshfRgba[3] = lshfBMP(maskRgba[3], bpp);
    bitsRgba[3] = bitsBMP(maskRgba[3], bpp, lshfRgba[3]);

    switch (compression) {
    case BMPComprRgb:
      break;
    case BMPComprBitFld:
      maskRgba[0] = letoh(v4.maskR);
      maskRgba[1] = letoh(v4.maskG);
      maskRgba[2] = letoh(v4.maskB);
      break;
    default:
      throw FileExcept("Invalid BMP file");
    }

    // XXX
    BMPV4_PRINT(&v4, mpath);
  };

  // Read next bytes as BMPv5
  auto readV5 = [&] {
    BMPv5 v5;
    v5.size = htole(hdrSize);
    const auto n = BMPv5Size - sizeof hdrSize;
    if (!ifs.read(reinterpret_cast<char*>(&v5.width), n))
      throw FileExcept("Could not read from BMP file");

    readN += n;
    width = letoh(v5.width);
    height = letoh(v5.height);
    bpp = letoh(v5.bpp);
    compression = letoh(v5.compression);
    // 16/32 bpp formats have alpha channel
    maskRgba[3] = (bpp == 16 || bpp == 32) ? letoh(v5.maskA) : 0;
    lshfRgba[3] = lshfBMP(maskRgba[3], bpp);
    bitsRgba[3] = bitsBMP(maskRgba[3], bpp, lshfRgba[3]);

    switch (compression) {
    case BMPComprRgb:
      break;
    case BMPComprBitFld:
      maskRgba[0] = letoh(v5.maskR);
      maskRgba[1] = letoh(v5.maskG);
      maskRgba[2] = letoh(v5.maskB);
      break;
    default:
      throw FileExcept("Invalid BMP file");
    }

    // XXX
    BMPV5_PRINT(&v5, mpath);
  };

  switch (hdrSize) {
  case BMPihSize:
    readIh();
    break;
  case BMPv4Size:
    readV4();
    break;
  case BMPv5Size:
    readV5();
    break;
  default:
    throw FileExcept("Could not identify BMP file header");
  }

  if (width <= 0 || height == 0 || bpp == 0 || bpp > 32)
    throw FileExcept("Invalid BMP file");

  // TODO...
}
