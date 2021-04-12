//
// SG
// DataBMP.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cwchar>
#include <cstring>
#include <fstream>
#include <memory>
#include <algorithm>

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

#ifdef YF_DEVEL
void printBMP(const BMPfh& fh, const wstring& pathname);
void printBMP(const BMPih& ih, const wstring& pathname);
void printBMP(const BMPv4& v4, const wstring& pathname);
void printBMP(const BMPv5& v5, const wstring& pathname);
#endif

INTERNAL_NS_END

void SG_NS::loadBMP(Texture::Data& dst, const wstring& pathname) {
  // Convert pathname string
  string path{};
  size_t len = (pathname.size() + 1) * sizeof(wchar_t);
  path.resize(len);
  const wchar_t* wsrc = pathname.data();
  mbstate_t state;
  memset(&state, 0, sizeof state);
  len = wcsrtombs(path.data(), &wsrc, path.size(), &state);
  if (wsrc || static_cast<size_t>(-1) == len)
    throw ConversionExcept("Could not convert BMP file path");
  path.resize(len);

  ifstream ifs(path);
  if (!ifs)
    throw FileExcept("Could not open BMP file");

  // Get file header
  BMPfh fh;
  if (!ifs.read(reinterpret_cast<char*>(&fh.type), BMPfhSize))
    throw FileExcept("Could not read from BMP file");

  if (letoh(fh.type) != BMPType)
    throw FileExcept("Invalid BMP file");

#ifdef YF_DEVEL
  printBMP(fh, pathname);
#endif

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
  uint32_t ciN;
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
    ciN = letoh(ih.ciN);
    // info header only supports non-alpha colors
    maskRgba[3] = 0;
    lshfRgba[3] = bpp;
    bitsRgba[3] = 0;

    switch (compression) {
    case BMPComprRgb:
      break;
    case BMPComprBitFld:
      if (readN == letoh(fh.dataOffset) ||
          !ifs.read(reinterpret_cast<char*>(maskRgba), 3 * sizeof *maskRgba))
        throw FileExcept("Invalid BMP file");
      readN += 3 * sizeof *maskRgba;
      maskRgba[0] = letoh(maskRgba[0]);
      maskRgba[1] = letoh(maskRgba[1]);
      maskRgba[2] = letoh(maskRgba[2]);
      break;
    default:
      throw FileExcept("Invalid BMP file");
    }

#ifdef YF_DEVEL
    printBMP(ih, pathname);
#endif
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
    ciN = letoh(v4.ciN);
    // 16/32 bpp formats have alpha channel
    maskRgba[3] = (bpp == 16 || bpp == 32) ? letoh(v4.maskA) : 0;

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

#ifdef YF_DEVEL
    printBMP(v4, pathname);
#endif
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
    ciN = letoh(v5.compression);
    // 16/32 bpp formats have alpha channel
    maskRgba[3] = (bpp == 16 || bpp == 32) ? letoh(v5.maskA) : 0;

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

#ifdef YF_DEVEL
    printBMP(v5, pathname);
#endif
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

  if (width <= 0 || height == 0 || bpp == 0 || bpp > 64)
    throw FileExcept("Invalid BMP file");

  const uint32_t dataOffset = letoh(fh.dataOffset);
  if (bpp > 8 && readN != fh.dataOffset && !ifs.seekg(dataOffset))
    throw FileExcept("Invalid BMP file");

  const size_t channels = maskRgba[3] != 0 ? 4 : 3;
  const size_t dataLen = channels * width * (height < 0 ? -height : height);
  auto data = make_unique<uint8_t[]>(dataLen);

  int32_t to, from, increment;
  if (height > -1) {
    // pixel data is stored bottom-up
#ifdef YF_FLIP_BMP
    from = 0;
    to = height;
    increment = 1;
#else
    from = height-1;
    to = -1;
    increment = -1;
#endif
  } else {
    // pixel data is stored top-down
#ifdef YF_FLIP_BMP
    from = -height-1;
    to = -1;
    increment = -1;
#else
    from = 0;
    to = -height;
    increment = 1;
#endif
  }

  // Read next bytes as 8bpp format
  auto read8 = [&] {
    if (ciN == 0)
      ciN = 256;
    auto ci = make_unique<uint32_t[]>(ciN);
    if (!ifs.read(reinterpret_cast<char*>(ci.get()), ciN * sizeof(uint32_t)))
      throw FileExcept("Could not read from BMP file");

    const size_t padding = width & 3 ? 4 - (width & 3) : 0;
    const size_t lineSize = width + padding;
    auto scanline = make_unique<uint8_t[]>(lineSize);

    for (auto i = from; i != to; i += increment) {
      if (!ifs.read(reinterpret_cast<char*>(scanline.get()), lineSize))
        throw FileExcept("Could not read from BMP file");

      for (int32_t j = 0; j < width; ++j) {
        auto index = channels*width*i + channels*j;
        memcpy(&data[index], &scanline[j], channels);
      }
    }
  };

  // Read next bytes as 16bpp format
  auto read16 = [&] {
    if (compression == BMPComprRgb) {
      maskRgba[0] = 0x7C00;
      maskRgba[1] = 0x03e0;
      maskRgba[2] = 0x001f;
    }
    for (size_t i = 0; i < channels; ++i) {
      lshfRgba[i] = lshfBMP(maskRgba[i], bpp);
      bitsRgba[i] = bitsBMP(maskRgba[i], bpp, lshfRgba[i]);
    }

    const size_t padding = (width & 1) << 1;
    const size_t lineSize = (width << 1) + padding;
    auto scanline = make_unique<uint16_t[]>(lineSize >> 1);

    // each channel will be scaled to the 8-bit range
    const uint32_t diffRgba[4]{min(8U, 8U-bitsRgba[0]),
                               min(8U, 8U-bitsRgba[1]),
                               min(8U, 8U-bitsRgba[2]),
                               min(8U, 8U-bitsRgba[3])};
    uint16_t pixel;
    uint32_t scale;
    uint32_t component;
    size_t index;

    for (auto i = from; i != to; i += increment) {
      if (!ifs.read(reinterpret_cast<char*>(scanline.get()), lineSize))
        throw FileExcept("Could not read from BMP file");

      for (int32_t j = 0; j < width; ++j) {
        pixel = letoh(scanline[j]);
        for (size_t k = 0; k < channels; ++k) {
          index = channels*width*i + channels*j + k;
          component = (pixel & maskRgba[k]) >> lshfRgba[k];
          scale = 1 << diffRgba[k];
          data[index] = component*scale + component%scale;
        }
      }
    }
  };

  // Read next bytes as 24bpp format
  auto read24 = [&] {
    if (compression != BMPComprRgb)
      throw FileExcept("Invalid BMP file");

    const size_t padding = width % 4;
    const size_t lineSize = 3 * width + padding;
    auto scanline = make_unique<uint8_t[]>(lineSize);

    size_t k0, k2;
    if (letoh(0xFF) == 0xFF) {
      k0 = 2;
      k2 = 0;
    } else {
      k0 = 0;
      k2 = 2;
    }

    for (auto i = from; i != to; i += increment) {
      if (!ifs.read(reinterpret_cast<char*>(scanline.get()), lineSize))
        throw FileExcept("Could no read from BMP file");

      for (int32_t j = 0; j < width; ++j) {
        auto index = channels*width*i + channels*j;
        data[index++] = scanline[3*j+k0];
        data[index++] = scanline[3*j+1];
        data[index++] = scanline[3*j+k2];
      }
    }
  };

  // Read next bytes as 32bpp format
  auto read32 = [&] {
    if (compression == BMPComprRgb) {
      maskRgba[0] = 0x00ff0000;
      maskRgba[1] = 0x0000ff00;
      maskRgba[2] = 0x000000ff;
    }
    for (size_t i = 0; i < channels; ++i)
      lshfRgba[i] = lshfBMP(maskRgba[i], bpp);

    // no padding needed
    const size_t lineSize = width << 2;
    auto scanline = make_unique<uint32_t[]>(width);

    for (auto i = from; i != to; i += increment) {
      if (!ifs.read(reinterpret_cast<char*>(scanline.get()), lineSize))
        throw FileExcept("Could not read from BMP file");

      for (int32_t j = 0; j < width; ++j) {
        auto pixel = letoh(scanline[j]);
        for (size_t k = 0; k < channels; ++k) {
          auto index = channels*width*i + channels*j + k;
          data[index] = (pixel & maskRgba[k]) >> lshfRgba[k];
        }
      }
    }
  };

  switch (bpp) {
  case 8:
    read8();
    break;
  case 16:
    read16();
    break;
  case 24:
    read24();
    break;
  case 32:
    read32();
    break;
  default:
    throw FileExcept("Unsupported BMP bpp value");
  }

  // Set destination for the data
  dst.data.swap(data);
  if (channels == 4)
    dst.format = CG_NS::PxFormatRgba8Srgb;
  else
    dst.format = CG_NS::PxFormatRgb8Srgb;
  dst.size.width = width;
  dst.size.height = height < 0 ? -height : height;
  // TODO
  dst.levels = 1;
  dst.samples = CG_NS::Samples1;
}

//
// DEVEL
//

#ifdef YF_DEVEL

INTERNAL_NS_BEGIN

void printBMP(const BMPfh& fh, const wstring& pathname) {
  wprintf(L"\n#BMP#");
  wprintf(L"\npathname: %ls", pathname.data());
  wprintf(L"\nfh - type: 0x%02x (%c%c)", fh.type, fh.type, fh.type >> 8);
  wprintf(L"\nfh - size: %u", fh.size);
  wprintf(L"\nfh - reserved1: %u", fh.reserved1);
  wprintf(L"\nfh - reserved2: %u", fh.reserved2);
  wprintf(L"\nfh - dataOffset: %u\n", fh.dataOffset);
}

void printBMP(const BMPih& ih, const wstring& pathname) {
  wprintf(L"\n#BMP#");
  wprintf(L"\npathname: %ls", pathname.data());
  wprintf(L"\nih - size: %u", ih.size);
  wprintf(L"\nih - width: %d", ih.width);
  wprintf(L"\nih - height: %d", ih.height);
  wprintf(L"\nih - planes: %u", ih.planes);
  wprintf(L"\nih - bpp: %u", ih.bpp);
  wprintf(L"\nih - compression: %u", ih.compression);
  wprintf(L"\nih - imageSize: %u", ih.imageSize);
  wprintf(L"\nih - ppmX: %d", ih.ppmX);
  wprintf(L"\nih - ppmY: %d", ih.ppmY);
  wprintf(L"\nih - ciN: %u", ih.ciN);
  wprintf(L"\nih - ciImportant: %u\n", ih.ciImportant);
}

void printBMP(const BMPv4& v4, const wstring& pathname) {
  wprintf(L"\n#BMP#");
  wprintf(L"\npathname: %ls", pathname.data());
  wprintf(L"\nv4 - size: %u", v4.size);
  wprintf(L"\nv4 - width: %d", v4.width);
  wprintf(L"\nv4 - height: %d", v4.height);
  wprintf(L"\nv4 - planes: %u", v4.planes);
  wprintf(L"\nv4 - bpp: %u", v4.bpp);
  wprintf(L"\nv4- compression: %u", v4.compression);
  wprintf(L"\nv4 - imageSz: %u", v4.imageSize);
  wprintf(L"\nv4 - ppmX: %d", v4.ppmX);
  wprintf(L"\nv4 - ppmY: %d", v4.ppmY);
  wprintf(L"\nv4 - ciN: %u", v4.ciN);
  wprintf(L"\nv4 - ciImportant: %u", v4.ciImportant);
  wprintf(L"\nv4 - maskR: 0x%08x", v4.maskR);
  wprintf(L"\nv4 - maskG: 0x%08x", v4.maskG);
  wprintf(L"\nv4 - maskB: 0x%08x", v4.maskB);
  wprintf(L"\nv4 - maskA: 0x%08x", v4.maskA);
  wprintf(L"\nv4 - csType: 0x%08x (%c%c%c%c)", v4.csType,
          v4.csType, v4.csType >> 8, v4.csType >> 16, v4.csType >> 24);
  wprintf(L"\nv4 - endPoints: %u,%u,%u %u,%u,%u %u,%u,%u",
          v4.endPoints[0], v4.endPoints[1], v4.endPoints[2],
          v4.endPoints[3], v4.endPoints[4], v4.endPoints[5],
          v4.endPoints[6], v4.endPoints[7], v4.endPoints[8]);
  wprintf(L"\nv4 - gammaR: %u", v4.gammaR);
  wprintf(L"\nv4 - gammaG: %u", v4.gammaG);
  wprintf(L"\nv4 - gammaB: %u\n", v4.gammaB);
}

void printBMP(const BMPv5& v5, const wstring& pathname) {
  wprintf(L"\n#BMP#");
  wprintf(L"\npathname: %ls", pathname.data());
  wprintf(L"\nv5 - size: %u", v5.size);
  wprintf(L"\nv5 - width: %d", v5.width);
  wprintf(L"\nv5 - height: %d", v5.height);
  wprintf(L"\nv5 - planes: %u", v5.planes);
  wprintf(L"\nv5 - bpp: %u", v5.bpp);
  wprintf(L"\nv5 - compression: %u", v5.compression);
  wprintf(L"\nv5 - imageSize: %u", v5.imageSize);
  wprintf(L"\nv5 - ppmX: %d", v5.ppmX);
  wprintf(L"\nv5 - ppmY: %d", v5.ppmY);
  wprintf(L"\nv5 - ciN: %u", v5.ciN);
  wprintf(L"\nv5 - ciImportant: %u", v5.ciImportant);
  wprintf(L"\nv5 - maskR: 0x%08x", v5.maskR);
  wprintf(L"\nv5 - maskG: 0x%08x", v5.maskG);
  wprintf(L"\nv5 - maskB: 0x%08x", v5.maskB);
  wprintf(L"\nv5 - maskA: 0x%08x", v5.maskA);
  wprintf(L"\nv5 - csType: 0x%08x (%c%c%c%c)", v5.csType,
          v5.csType, v5.csType >> 8, v5.csType >> 16, v5.csType >> 24);
  wprintf(L"\nv5 - endPoints: %u,%u,%u %u,%u,%u %u,%u,%u",
          v5.endPoints[0], v5.endPoints[1], v5.endPoints[2],
          v5.endPoints[3], v5.endPoints[4], v5.endPoints[5],
          v5.endPoints[6], v5.endPoints[7], v5.endPoints[8]);
  wprintf(L"\nv5 - gammaR: %u", v5.gammaR);
  wprintf(L"\nv5 - gammaG: %u", v5.gammaG);
  wprintf(L"\nv5 - gammaB: %u", v5.gammaB);
  wprintf(L"\nv5 - intent: %u", v5.intent);
  wprintf(L"\nv5 - profileData: %u", v5.profileData);
  wprintf(L"\nv5 - profileSize: %u", v5.profileSize);
  wprintf(L"\nv5 - reserved: %u\n", v5.reserved);
}

INTERNAL_NS_END

#endif
