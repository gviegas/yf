//
// SG
// DataPNG.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <cstring>
#include <cassert>
#include <fstream>
#include <atomic>

#include "DataPNG.h"
#include "yf/Except.h"

#if defined(_DEFAULT_SOURCE)
# include <endian.h>
inline uint16_t letoh(uint16_t v) { return le16toh(v); }
inline uint32_t letoh(uint32_t v) { return le32toh(v); }
inline uint16_t htole(uint16_t v) { return htole16(v); }
inline uint32_t htole(uint32_t v) { return htole16(v); }
inline uint16_t betoh(uint16_t v) { return be16toh(v); }
inline uint32_t betoh(uint32_t v) { return be32toh(v); }
inline uint16_t htobe(uint16_t v) { return htobe16(v); }
inline uint32_t htobe(uint32_t v) { return htobe16(v); }
#else
// TODO
# error "Invalid platform"
#endif

using namespace YF_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

class PNG {
 public:
  /// File signature.
  ///
  static constexpr uint8_t Signature[]{137, 80, 78, 71, 13, 10, 26, 10};

  /// Chunk names.
  ///
  static constexpr uint8_t IHDRType[]{'I', 'H', 'D', 'R'};

  /// IHDR.
  ///
  struct IHDR {
    uint32_t width;
    uint32_t height;
    uint8_t bitDepth;
    uint8_t colorType;
    uint8_t compressionMethod;
    uint8_t filterMethod;
    uint8_t interlaceMethod;
  };
  static constexpr uint32_t IHDRSize = 13;
  static_assert(offsetof(IHDR, interlaceMethod) == IHDRSize-1, "!offsetof");

  /// Computes CRC.
  ///
  uint32_t computeCRC(const uint8_t* data, uint32_t n) const {
    assert(data);
    assert(n > 0);

    static uint32_t table[256]{};
    static atomic pending{true};
    static bool wait = true;

    if (pending.exchange(false)) {
      for (uint32_t i = 0; i < 256; ++i) {
        auto x = i;
        for (uint32_t j = 0; j < 8; ++j)
          x = (x & 1) ? (0xEDB88320 ^ (x >> 1)) : (x >> 1);
        table[i] = x;
      }
      wait = false;
    } else {
      while (wait) { }
    }

    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < n; ++i)
      crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFF;
  }

  PNG(const std::wstring& pathname) {
    // Convert pathname string
    string path{};
    size_t len = (pathname.size() + 1) * sizeof(wchar_t);
    path.resize(len);
    const wchar_t* wsrc = pathname.data();
    mbstate_t state;
    memset(&state, 0, sizeof state);
    len = wcsrtombs(path.data(), &wsrc, path.size(), &state);
    if (wsrc || static_cast<size_t>(-1) == len)
      throw ConversionExcept("Could not convert PNG file path");
    path.resize(len);

    ifstream ifs(path);
    if (!ifs)
      throw FileExcept("Could not open PNG file");

    // Check signature
    uint8_t sign[sizeof Signature];

    if (!ifs.read(reinterpret_cast<char*>(sign), sizeof sign))
      throw FileExcept("Could not read from PNG file");

    if (memcmp(sign, Signature, sizeof sign) != 0)
      throw FileExcept("Invalid PNG file");

    // Process chunks
    vector<uint8_t> buffer{};
    buffer.resize(4096);
    const uint32_t dataOff = 8;

    if (!ifs.read(reinterpret_cast<char*>(buffer.data()), dataOff))
      throw FileExcept("Could not read from PNG file");

    if (memcmp(buffer.data()+4, IHDRType, 4) != 0)
      throw FileExcept("Invalid PNG file");

    // TODO...
  }

  PNG(const PNG&) = delete;
  PNG& operator=(const PNG&) = delete;
  ~PNG() = default;

 private:
};

INTERNAL_NS_END

void SG_NS::loadPNG(Texture::Data& dst, const std::wstring& pathname) {
  PNG png(pathname);

  // TODO
  exit(0);
}
