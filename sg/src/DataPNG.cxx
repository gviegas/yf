//
// SG
// DataPNG.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cstdint>
#include <cwchar>
#include <cstring>
#include <fstream>

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
  // TODO
};

INTERNAL_NS_END

void SG_NS::loadPNG(Texture::Data& dst, const std::wstring& pathname) {
  // TODO
  throw runtime_error("Unimplemented");
}
