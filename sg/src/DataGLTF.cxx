//
// SG
// DataGLTF.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cwchar>
#include <cstring>
#include <fstream>

#include "DataGLTF.h"
#include "yf/Except.h"

using namespace YF_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct Symbol {
  enum Type {
    Str,
    Num,
    Bool,
    Null,
    Op,
    End,
    Err
  };

  ifstream& ifs;
  Type type;
  string tokens;

  Symbol(ifstream& ifs) : ifs(ifs), type(End), tokens() { }
  Symbol(const Symbol&) = delete;
  Symbol& operator=(const Symbol&) = delete;
  ~Symbol() = default;

  Type next() {
    // TODO
    return Err;
  }
};

// TODO
struct GLTF {
  GLTF(const wstring& pathname) {
    // Convert pathname string
    string path{};
    size_t len = (pathname.size() + 1) * sizeof(wchar_t);
    path.resize(len);
    const wchar_t* wsrc = pathname.data();
    mbstate_t state;
    memset(&state, 0, sizeof state);
    len = wcsrtombs(path.data(), &wsrc, path.size(), &state);
    if (wsrc || static_cast<size_t>(-1) == len)
      throw ConversionExcept("Could not convert glTF file path");
    path.resize(len);

    ifstream ifs(path);
    if (!ifs)
      throw FileExcept("Could not open glTF file");

    Symbol symbol(ifs);

    // TODO
  }

  GLTF(const GLTF&) = delete;
  GLTF& operator=(const GLTF&) = delete;
  ~GLTF() = default;
};

INTERNAL_NS_END

void SG_NS::loadGLTF(Node& dst, const wstring& pathname) {
  GLTF gltf(pathname);

  // TODO
}

void SG_NS::loadGLTF(Model& dst, const wstring& pathname) {
  GLTF gltf(pathname);

  // TODO
}

void SG_NS::loadGLTF(Mesh::Data& dst, const wstring& pathname) {
  GLTF gltf(pathname);

  // TODO
}
