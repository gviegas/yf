//
// SG
// DataGLTF.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cwchar>
#include <cstring>
#include <cctype>
#include <cassert>
#include <fstream>

#include "DataGLTF.h"
#include "yf/Except.h"

using namespace YF_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

/// Symbol.
///
class Symbol {
 public:
  /// Type of symbol.
  ///
  enum Type {
    Str,
    Num,
    Bool,
    Null,
    Op,
    End,
    Err
  };

  Symbol(ifstream& ifs) : ifs_(ifs), type_(End), tokens_() { }
  Symbol(const Symbol&) = delete;
  Symbol& operator=(const Symbol&) = delete;
  ~Symbol() = default;

  explicit operator bool() const {
    return !operator!();
  }

  bool operator!() const {
    return type_ == End || type_ == Err;
  }

  /// Process the next symbol from stream.
  ///
  Type next() {
    tokens_.clear();
    char c;

    while (ifs_.get(c) && isspace(c)) { }

    if (!ifs_) {
      type_ = End;
      return type_;
    }

    switch (c) {
    case '"':
      while (true) {
        ifs_.get(c);
        if (!ifs_) {
          type_ = Err;
          break;
        }
        if (c == '"') {
          type_ = Str;
          break;
        }
        if (c == '\\') {
          ifs_.get(c);
          if (c != '"' && c != '\\') {
            // TODO: other escape sequences
            type_ = Err;
            break;
          }
        }
        tokens_.push_back(c);
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
        tokens_.push_back(c);
        ifs_.get(c);
        if (isxdigit(c) || c == '.' || c == '-' || c == '+')
          continue;
        if (ifs_)
          ifs_.unget();
        break;
      }
      type_ = Num;
      break;

    case 't':
      while (true) {
        tokens_.push_back(c);
        ifs_.get(c);
        if (!islower(c))
          break;
      }
      if (ifs_)
        ifs_.unget();
      if (tokens_ == "true")
        type_ = Bool;
      else
        type_ = Err;
      break;

    case 'f':
      while (true) {
        tokens_.push_back(c);
        ifs_.get(c);
        if (!islower(c))
          break;
      }
      if (ifs_)
        ifs_.unget();
      if (tokens_ == "false")
        type_ = Bool;
      else
        type_ = Err;
      break;

    case 'n':
      while (true) {
        tokens_.push_back(c);
        ifs_.get(c);
        if (!islower(c))
          break;
      }
      if (ifs_)
        ifs_.unget();
      if (tokens_ == "null")
        type_ = Null;
      else
        type_ = Err;
      break;

    case '{':
    case '}':
    case '[':
    case ']':
    case ':':
    case ',':
      tokens_.push_back(c);
      type_ = Op;
      break;

    default:
      type_ = Err;
    }

    return type_;
  }

  /// Consumes the current property.
  ///
  void consumeProperty() {
    assert(ifs_);
    assert(type_ == Str);

    if (next() != Op || tokens_[0] != ':')
      throw FileExcept("Invalid glTF file");

    switch (next()) {
    case Str:
    case Num:
    case Bool:
    case Null:
      break;

    case Op: {
      char op = tokens_[0];
      char cl;
      if (op == '{')
        cl = '}';
      else if (op == '[')
        cl = ']';
      else
        throw FileExcept("Invalid glTF file");

      size_t n = 1;
      do {
        switch (next()) {
          case Op:
            if (tokens_[0] == op)
              ++n;
            else if (tokens_[0] == cl)
              --n;
            break;

          case End:
          case Err:
            throw FileExcept("Invalid glTf file");

          default:
            break;
        }
      } while (n > 0);
    } break;

    default:
      throw FileExcept("Invalid glTF file");
    }
  }

  /// The type of the current symbol.
  ///
  Type type() const {
    return type_;
  }

  /// The tokens of the current symbol.
  ///
  const std::string& tokens() const {
    return tokens_;
  }

  /// The first token of the current symbol.
  ///
  char token() const {
    return tokens_.front();
  }

 private:
  ifstream& ifs_;
  Type type_{End};
  string tokens_{};
};

/// GLTF.
/// TODO
class GLTF {
 public:
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
