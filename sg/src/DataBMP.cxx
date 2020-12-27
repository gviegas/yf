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

// XXX
#define BMPFH_PRINT(fhPtr, path) do { \
  wprintf(L"\n#Filetype-BMP (debug)#"); \
  wprintf(L"\npathname: %s", path); \
  wprintf(L"\nfh - type: 0x%02x (%c%c)", \
    (fhPtr)->type, (fhPtr)->type, (fhPtr)->type >> 8); \
  wprintf(L"\nfh - size: %u", (fhPtr)->size); \
  wprintf(L"\nfh - reserved1: %u", (fhPtr)->reserved1); \
  wprintf(L"\nfh - reserved2: %u", (fhPtr)->reserved2); \
  wprintf(L"\nfh - dataOffset: %u\n", (fhPtr)->dataOffset); \
} while (0)

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

  // TODO...
}
