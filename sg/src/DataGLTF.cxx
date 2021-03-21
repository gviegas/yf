//
// SG
// DataGLTF.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cwchar>
#include <cstring>
#include <cctype>
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
    tokens.clear();
    char c;

    while (ifs.get(c) && isspace(c)) { }

    if (!ifs) {
      type = End;
      return type;
    }

    switch (c) {
    case '"':
      while (true) {
        ifs.get(c);
        if (!ifs) {
          type = Err;
          break;
        }
        if (c == '"') {
          type = Str;
          break;
        }
        if (c == '\\') {
          ifs.get(c);
          if (c != '"' && c != '\\') {
            // TODO: other escape sequences
            type = Err;
            break;
          }
        }
        tokens.push_back(c);
      }
      break;

    case '-':
    case '+':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      while (true) {
        tokens.push_back(c);
        ifs.get(c);
        if (isxdigit(c) || c == '.' || c == '-' || c == '+')
          continue;
        if (ifs)
          ifs.unget();
        break;
      }
      type = Num;
      break;

    case 't':
      while (true) {
        tokens.push_back(c);
        ifs.get(c);
        if (!islower(c))
          break;
      }
      if (ifs)
        ifs.unget();
      if (tokens == "true")
        type = Bool;
      else
        type = Err;
      break;

    case 'f':
      while (true) {
        tokens.push_back(c);
        ifs.get(c);
        if (!islower(c))
          break;
      }
      if (ifs)
        ifs.unget();
      if (tokens == "false")
        type = Bool;
      else
        type = Err;
      break;

    case 'n':
      while (true) {
        tokens.push_back(c);
        ifs.get(c);
        if (!islower(c))
          break;
      }
      if (ifs)
        ifs.unget();
      if (tokens == "null")
        type = Null;
      else
        type = Err;
      break;

    case '{':
    case '}':
    case '[':
    case ']':
    case ':':
    case ',':
      tokens.push_back(c);
      type = Op;
      break;

    default:
      type = Err;
    }

    return type;
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

    //////////
    do {
      switch (symbol.next()) {
      case Symbol::Str:
        wprintf(L"(Str) `%s`\n", symbol.tokens.data());
        break;
      case Symbol::Num:
        wprintf(L"(Num) `%s`\n", symbol.tokens.data());
        break;
      case Symbol::Bool:
        wprintf(L"(Bool) `%s`\n", symbol.tokens.data());
        break;
      case Symbol::Null:
        wprintf(L"(Null) `%s`\n", symbol.tokens.data());
        break;
      case Symbol::Op:
        wprintf(L"(Op) `%s`\n", symbol.tokens.data());
        break;
      case Symbol::End:
        wprintf(L"(End) `%s`\n", symbol.tokens.data());
        break;
      case Symbol::Err:
        wprintf(L"(ERR)\n `%s`", symbol.tokens.data());
        break;
      }
    } while (symbol.type != Symbol::End && symbol.type != Symbol::Err);
    //////////
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
