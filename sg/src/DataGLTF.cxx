//
// SG
// DataGLTF.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cstdint>
#include <cwchar>
#include <cstring>
#include <cctype>
#include <cassert>
#include <fstream>
#include <vector>
#include <type_traits>

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

  /// Gets the next symbol from file stream.
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

  /// Consumes symbols until one with the given type is parsed.
  ///
  void consumeUntil(Type type) {
    assert(ifs_);
    assert(type != Err);

    while (true) {
      next();
      if (type_ == type)
        break;
      if (type_ == End || type_ == Err)
        throw FileExcept("Invalid glTF file");
    }
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

    // Process symbols
    Symbol symbol(ifs);

    if (symbol.next() != Symbol::Op || symbol.token() != '{')
      throw FileExcept("Invalid glTF file");

    while (true) {
      switch (symbol.next()) {
      case Symbol::Str:
        if (symbol.tokens() == "scene")
          parseScene(symbol);
        else if (symbol.tokens() == "scenes")
          parseScenes(symbol);
        else if (symbol.tokens() == "nodes")
          parseNodes(symbol);
        else if (symbol.tokens() == "meshes")
          parseMeshes(symbol);
        else if (symbol.tokens() == "asset")
          parseAsset(symbol);
        // TODO...
        else
          symbol.consumeProperty();
        break;

      case Symbol::Op:
        if (symbol.token() == '}')
          return;
        break;

      default:
        throw FileExcept("Invalid glTF file");
      }
    }
  }

  GLTF(const GLTF&) = delete;
  GLTF& operator=(const GLTF&) = delete;
  ~GLTF() = default;

  /// Parses an array of objects.
  ///
  void parseObjectArray(Symbol& symbol, function<void ()> callback) {
    if (symbol.type() == Symbol::Op && symbol.token() == '{')
      callback();

    while (true) {
      switch (symbol.next()) {
      case Symbol::Op:
        if (symbol.token() == '{')
          callback();
        else if (symbol.token() == ']')
          return;
        break;

      default:
        throw FileExcept("Invalid glTF file");
      }
    }
  }

  /// Parses a string.
  ///
  void parseStr(Symbol& symbol, string& dst, bool next = true) {
    if (next || symbol.type() != Symbol::Str)
      symbol.consumeUntil(Symbol::Str);

    dst = symbol.tokens();
  }

  /// Parses an array of strings.
  ///
  void parseStrArray(Symbol& symbol, vector<string>& dst) {
    while (true) {
      switch (symbol.next()) {
      case Symbol::Str:
        dst.push_back(symbol.tokens());
        break;

      case Symbol::Op:
        if (symbol.token() == ']')
          return;
        break;

      default:
        throw FileExcept("Invalid glTF file");
      }
    }
  }

  /// Parses an integer number.
  ///
  void parseNum(Symbol& symbol, int32_t& dst, bool next = true) {
    if (next || symbol.type() != Symbol::Num)
      symbol.consumeUntil(Symbol::Num);

    dst = stol(symbol.tokens());
  }

  /// Parses a floating-point number.
  ///
  void parseNum(Symbol& symbol, float& dst, bool next = true) {
    if (next || symbol.type() != Symbol::Num)
      symbol.consumeUntil(Symbol::Num);

    dst = stof(symbol.tokens());
  }

  /// Parses an array of numbers.
  ///
  template<class T>
  void parseNumArray(Symbol& symbol, vector<T>& dst) {
    static_assert(is_arithmetic<T>(),
                  "parseNumArray() requires a numeric type");
    T num;

    while (true) {
      switch (symbol.next()) {
      case Symbol::Num:
        parseNum(symbol, num, false);
        dst.push_back(num);
        break;

      case Symbol::Op:
        if (symbol.token() == ']')
          return;
        break;

      default:
        throw FileExcept("Invalid glTF file");
      }
    }
  }

  /// Parses `gltf.scene`.
  ///
  void parseScene(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "scene");

    parseNum(symbol, scene_);
  }

  /// Element of `glTF.scenes` property.
  ///
  struct Scene {
    vector<int32_t> nodes{};
    string name{};
  };

  /// Parses `glTF.scenes`.
  ///
  void parseScenes(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "scenes");

    parseObjectArray(symbol, [&] {
      scenes_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "nodes")
            parseNumArray(symbol, scenes_.back().nodes);
          else if (symbol.tokens() == "name")
            parseStr(symbol, scenes_.back().name);
          else
            symbol.consumeProperty();
          break;

        case Symbol::Op:
          if (symbol.token() == '}')
            return;
          break;

        default:
          throw FileExcept("Invalid glTF file");
        }
      }
    });
  }

  /// Element of `glTF.nodes` property.
  ///
  struct Node {
    // `size()` tells whether this is a TRS or a matrix
    vector<float> transform{0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f,
                            1.0f, 1.0f, 1.0f};

    vector<int32_t> children{};
    int32_t camera = -1;
    int32_t mesh = -1;
    int32_t skin = -1;
    vector<float> weights{};
    string name{};
  };

  /// Parses `glTF.nodes`.
  ///
  void parseNodes(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "nodes");

    parseObjectArray(symbol, [&] {
      nodes_.push_back({});

      vector<float> t, r, s, m;

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "children")
            parseNumArray(symbol, nodes_.back().children);
          else if (symbol.tokens() == "camera")
            parseNum(symbol, nodes_.back().camera);
          else if (symbol.tokens() == "mesh")
            parseNum(symbol, nodes_.back().mesh);
          else if (symbol.tokens() == "skin")
            parseNum(symbol, nodes_.back().skin);
          else if (symbol.tokens() == "weights")
            parseNumArray(symbol, nodes_.back().weights);
          else if (symbol.tokens() == "translation")
            parseNumArray(symbol, t);
          else if (symbol.tokens() == "rotation")
            parseNumArray(symbol, r);
          else if (symbol.tokens() == "scale")
            parseNumArray(symbol, s);
          else if (symbol.tokens() == "matrix")
            parseNumArray(symbol, m);
          else if (symbol.tokens() == "name")
            parseStr(symbol, nodes_.back().name);
          else
            symbol.consumeProperty();
          break;

        case Symbol::Op:
          if (symbol.token() == '}') {
            if (m.empty()) {
              auto& xform = nodes_.back().transform;
              if (!t.empty()) {
                if (t.size() != 3)
                  throw FileExcept("Invalid glTF file");
                xform[0] = t[0];
                xform[1] = t[1];
                xform[2] = t[2];
              }
              if (!r.empty()) {
                if (r.size() != 4)
                  throw FileExcept("Invalid glTF file");
                xform[3] = r[0];
                xform[4] = r[1];
                xform[5] = r[2];
                xform[6] = r[3];
              }
              if (!s.empty()) {
                if (s.size() != 3)
                  throw FileExcept("Invalid glTF file");
                xform[7] = s[0];
                xform[8] = s[1];
                xform[9] = s[2];
              }
            } else {
              if (m.size() != 16)
                throw FileExcept("Invalid glTF file");
              nodes_.back().transform = m;
            }
            return;
          }
          break;

        default:
          throw FileExcept("Invalid glTF file");
        }
      }
    });
  }

  /// Element of `glTF.meshes` property.
  ///
  struct Mesh {
    struct Primitives {
      unordered_map<string, int32_t> attributes{};
      int32_t indices = -1;
      int32_t material = -1;
      vector<unordered_map<string, int32_t>> targets{};

      enum Mode {
        Points = 0,
        Lines = 1,
        LineLoop = 2,
        LineStrip = 3,
        Triangles = 4,
        TriangleStrip = 5,
        TriangleFan = 6
      };

      Mode mode = Triangles;
    };

    vector<Primitives> primitives{};
    vector<float> weights{};
    string name{};
  };

  /// Parses `glTF.meshes`.
  ///
  void parseMeshes(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "meshes");

    auto parseDictionary = [&] {
      auto& dict = symbol.type() == Symbol::Str ?
                   meshes_.back().primitives.back().attributes :
                   meshes_.back().primitives.back().targets.back();
      string key{};
      int32_t value{};

      symbol.consumeUntil(Symbol::Str);

      while (true) {
        key = symbol.tokens();
        if (symbol.next() != Symbol::Op || symbol.token() != ':')
          throw FileExcept("Invalid glTF file");
        parseNum(symbol, value);
        dict.emplace(key, value);

        switch (symbol.next()) {
        case Symbol::Op:
          if (symbol.token() == '}')
            return;
          if (symbol.next() != Symbol::Str)
            throw FileExcept("Invalid glTF file");
          break;

        default:
          throw FileExcept("Invalid glTF file");
        }
      }
    };

    auto parsePrimitive = [&] {
      meshes_.back().primitives.push_back({});
      auto& prim = meshes_.back().primitives.back();

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "attributes")
            parseDictionary();
          else if (symbol.tokens() == "indices")
            parseNum(symbol, prim.indices);
          else if (symbol.tokens() == "material")
            parseNum(symbol, prim.material);
          else if (symbol.tokens() == "targets")
            parseObjectArray(symbol, [&] {
              prim.targets.push_back({});
              parseDictionary();
            });
          else if (symbol.tokens() == "mode")
            parseNum(symbol, reinterpret_cast<int32_t&>(prim.mode));
          else
            symbol.consumeProperty();
          break;

        case Symbol::Op:
          if (symbol.token() == '}')
            return;
          break;

        default:
          throw FileExcept("Invalid glTF file");
        }
      }
    };

    parseObjectArray(symbol, [&] {
      meshes_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "primitives")
            parseObjectArray(symbol, parsePrimitive);
          else if (symbol.tokens() == "weights")
            parseNumArray(symbol, meshes_.back().weights);
          else if (symbol.tokens() == "name")
            parseStr(symbol, meshes_.back().name);
          else
            symbol.consumeProperty();
          break;

        case Symbol::Op:
          if (symbol.token() == '}')
            return;
          break;

        default:
          throw FileExcept("Invalid glTF file");
        }
      }
    });
  }

  /// `glTF.asset` property.
  ///
  struct Asset {
    string copyright{};
    string generator{};
    string version{};
    string minVersion{};
  };

  /// Parses `glTF.asset`.
  ///
  void parseAsset(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "asset");

    while (true) {
      switch (symbol.next()) {
      case Symbol::Str:
        if (symbol.tokens() == "copyright")
          parseStr(symbol, asset_.copyright);
        else if (symbol.tokens() == "generator")
          parseStr(symbol, asset_.generator);
        else if (symbol.tokens() == "version")
          parseStr(symbol, asset_.version);
        else if (symbol.tokens() == "minVersion")
          parseStr(symbol, asset_.minVersion);
        else
          symbol.consumeProperty();
        break;

      case Symbol::Op:
        if (symbol.token() == '}')
          return;
        break;

      default:
        throw FileExcept("Invalid glTF file");
      }
    }
  }

 private:
  int32_t scene_ = -1;
  vector<Scene> scenes_{};
  vector<Node> nodes_{};
  vector<Mesh> meshes_{};
  Asset asset_{};

#ifdef YF_DEVEL
  friend void printGLTF(const GLTF&);
#endif
};

INTERNAL_NS_END

void SG_NS::loadGLTF(Node& dst, const wstring& pathname) {
  GLTF gltf(pathname);

  // TODO
#ifdef YF_DEVEL
  printGLTF(gltf);
#endif
}

void SG_NS::loadGLTF(Model& dst, const wstring& pathname) {
  GLTF gltf(pathname);

  // TODO
#ifdef YF_DEVEL
  printGLTF(gltf);
#endif
}

void SG_NS::loadGLTF(Mesh::Data& dst, const wstring& pathname) {
  GLTF gltf(pathname);

  // TODO
#ifdef YF_DEVEL
  printGLTF(gltf);
#endif
}

//
// DEVEL
//

#ifdef YF_DEVEL

INTERNAL_NS_BEGIN

void printGLTF(const GLTF& gltf) {
  wprintf(L"\nGLTF");

  wprintf(L"\n scene: %d", gltf.scene_);

  wprintf(L"\n scenes:");
  for (const auto& scn : gltf.scenes_) {
    wprintf(L"\n  scene `%s`:", scn.name.data());
    wprintf(L"\n   nodes:");
    for (auto nd : scn.nodes)
      wprintf(L" %d", nd);
  }

  wprintf(L"\n nodes:");
  for (const auto& nd : gltf.nodes_) {
    wprintf(L"\n  node `%s`:", nd.name.data());
    wprintf(L"\n   children:");
    for (auto ch : nd.children)
      wprintf(L" %d", ch);
    wprintf(L"\n   camera: %d", nd.camera);
    wprintf(L"\n   mesh: %d", nd.mesh);
    wprintf(L"\n   skin: %d", nd.skin);
    wprintf(L"\n   weights:");
    for (auto wt : nd.weights)
      wprintf(L" %.6f", wt);
    wprintf(L"\n   transform:");
    if (nd.transform.size() == 10) {
      wprintf(L"\n    T:");
      for (size_t i = 0; i < 3; ++i)
        wprintf(L" %.6f", nd.transform[i]);
      wprintf(L"\n    R:");
      for (size_t i = 3; i < 7; ++i)
        wprintf(L" %.6f", nd.transform[i]);
      wprintf(L"\n    S:");
      for (size_t i = 7; i < 10; ++i)
        wprintf(L" %.6f", nd.transform[i]);
    } else {
      for (size_t i = 0; i < nd.transform.size(); ++i) {
        if (i%4 == 0)
          wprintf(L"\n   ");
        wprintf(L" %.6f", nd.transform[i]);
      }
    }
  }

  wprintf(L"\n meshes:");
  for (const auto& msh : gltf.meshes_) {
    wprintf(L"\n  mesh `%s`:", msh.name.data());
    wprintf(L"\n   primitives:");
    for (const auto& prm : msh.primitives) {
      wprintf(L"\n    attributes:");
      for (const auto& att : prm.attributes)
        wprintf(L"\n     %s : %d", att.first.data(), att.second);
      wprintf(L"\n    indices: %d", prm.indices);
      wprintf(L"\n    material: %d", prm.material);
      wprintf(L"\n    mode: %d", prm.mode);
      for (const auto& tgt : prm.targets) {
        wprintf(L"\n     target:");
        for (const auto& att : tgt)
          wprintf(L"\n      %s : %d", att.first.data(), att.second);
      }
    }
    wprintf(L"\n   weights:");
    for (auto wt : msh.weights)
      wprintf(L" %.6f", wt);
  }

  wprintf(L"\n asset:");
  wprintf(L"\n  copyright: %s", gltf.asset_.copyright.data());
  wprintf(L"\n  generator: %s", gltf.asset_.generator.data());
  wprintf(L"\n  version: %s", gltf.asset_.version.data());
  wprintf(L"\n  minVersion: %s", gltf.asset_.minVersion.data());

  wprintf(L"\n");
}

INTERNAL_NS_END

#endif
