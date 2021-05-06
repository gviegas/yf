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
#include <vector>
#include <unordered_map>
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
///
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

    // Set directory
    const auto pos = path.find_last_of('/');
    if (pos != 0 && pos != path.npos)
      directory_ = {path.begin(), path.begin() + pos};

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
        else if (symbol.tokens() == "skins")
          parseSkins(symbol);
        else if (symbol.tokens() == "materials")
          parseMaterials(symbol);
        else if (symbol.tokens() == "textures")
          parseTextures(symbol);
        else if (symbol.tokens() == "samplers")
          parseSamplers(symbol);
        else if (symbol.tokens() == "images")
          parseImages(symbol);
        else if (symbol.tokens() == "cameras")
          parseCameras(symbol);
        else if (symbol.tokens() == "animations")
          parseAnimations(symbol);
        else if (symbol.tokens() == "accessors")
          parseAccessors(symbol);
        else if (symbol.tokens() == "bufferViews")
          parseBufferViews(symbol);
        else if (symbol.tokens() == "buffers")
          parseBuffers(symbol);
        else if (symbol.tokens() == "asset")
          parseAsset(symbol);
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

  /// Element of `glTF.scenes` property.
  ///
  struct Scene {
    vector<int32_t> nodes{};
    string name{};
  };

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

  /// Element of `glTF.meshes` property.
  ///
  struct Mesh {
    struct Primitive {
      enum Mode : int32_t {
        Points = 0,
        Lines = 1,
        LineLoop = 2,
        LineStrip = 3,
        Triangles = 4,
        TriangleStrip = 5,
        TriangleFan = 6
      };

      unordered_map<string, int32_t> attributes{};
      int32_t indices = -1;
      int32_t mode = Triangles;
      int32_t material = -1;
      vector<unordered_map<string, int32_t>> targets{};
    };

    vector<Primitive> primitives{};
    vector<float> weights{};
    string name{};
  };

  /// Element of `glTF.skins` property.
  ///
  struct Skin {
    int32_t inverseBindMatrices = -1;
    int32_t skeleton = -1;
    vector<int32_t> joints{};
    string name{};
  };

  /// Element of `glTF.materials` property.
  ///
  struct Material {
    struct TextureInfo {
      int32_t index = -1;
      int32_t texCoord = 0;
    };

    struct PbrMetallicRoughness {
      TextureInfo baseColorTexture{};
      vector<float> baseColorFactor{1.0f, 1.0f, 1.0f, 1.0f};
      TextureInfo metallicRoughnessTexture{};
      float metallicFactor = 1.0f;
      float roughnessFactor = 1.0f;
    };

    struct NormalTextureInfo : TextureInfo {
      float scale = 1.0f;
    };

    struct OcclusionTextureInfo : TextureInfo {
      float strength = 1.0f;
    };

    PbrMetallicRoughness pbrMetallicRoughness{};
    NormalTextureInfo normalTexture{};
    OcclusionTextureInfo occlusionTexture{};
    TextureInfo emissiveTexture{};
    vector<float> emissiveFactor{0.0f, 0.0f, 0.0f};
    string alphaMode{"OPAQUE"};
    float alphaCutoff = 0.5f;
    bool doubleSided = false;
    string name{};
  };

  /// Element of `glTF.textures` property.
  ///
  struct Texture {
    int32_t sampler = -1;
    int32_t source = -1;
    string name{};
  };

  /// Element of `gltf.samplers` property.
  ///
  struct Sampler {
    enum WrapMode : int32_t {
      ClampToEdge = 33071,
      MirroredRepeat = 33648,
      Repeat = 10497
    };

    enum Filter : int32_t {
      Nearest = 9728,
      Linear = 9729,
      NearestMipmapNearest = 9984,
      LinearMipmapNearest = 9985,
      NearestMipmapLinear = 9986,
      LinearMipmapLinear = 9987,

      Undefined = -1
    };

    int32_t wrapS = Repeat;
    int32_t wrapT = Repeat;
    int32_t magFilter = Undefined;
    int32_t minFilter = Undefined;
    string name{};
  };

  /// Element of `gltf.images` property.
  ///
  struct Image {
    string uri{};
    string mimeType{};
    int32_t bufferView = -1;
    string name{};
  };

  /// Element of `gltF.cameras` property.
  ///
  struct Camera {
    union {
      struct {
        float aspectRatio = -1.0f;
        float yfov = -1.0f;
        float zfar = -1.0f;
        float znear = -1.0f;
      } perspective{};

      struct {
        float xmag = -1.0f;
        float ymag = -1.0f;
        float zfar = -1.0f;
        float znear = -1.0f;
      } orthographic;
    };

    string type{};
    string name{};
  };

  /// Element of `glTF.animations` property.
  ///
  struct Animation {
    struct Channel {
      struct Target {
        int32_t node = -1;
        string path{};
      };

      int32_t sampler = -1;
      Target target{};
    };

    struct Sampler {
      int32_t input = -1;
      string interpolation{};
      int32_t output = -1;
    };

    vector<Channel> channels{};
    vector<Sampler> samplers{};
    string name{};
  };

  /// Element of `glTF.accessors` property.
  ///
  struct Accessor {
    enum ComponentType : int32_t {
      Byte = 5120,
      UnsignedByte = 5121,
      Short = 5122,
      UnsignedShort = 5123,
      UnsignedInt = 5125,
      Float = 5126,

      Undefined = -1
    };

    struct Sparse {
      struct Indices {
        int32_t bufferView = -1;
        int64_t byteOffset = 0LL;
        int32_t componentType = Undefined;
      };

      struct Values {
        int32_t bufferView = -1;
        int64_t byteOffset = 0LL;
      };

      int32_t count = -1;
      Indices indices{};
      Values values{};
    };

    int32_t bufferView = -1;
    int64_t byteOffset = 0LL;
    int32_t componentType = Undefined;
    bool normalized = false;
    int32_t count = -1;
    string type{};
    vector<double> min{};
    vector<double> max{};
    Sparse sparse{};
    string name{};

    /// Size of `componentType`.
    ///
    size_t sizeOfComponentType() const {
      switch (componentType) {
      case Byte:
      case UnsignedByte:
        return 1;
      case Short:
      case UnsignedShort:
        return 2;
      case UnsignedInt:
      case Float:
        return 4;
      default:
        break;
      }
      return 0;
    }

    /// Size of `type`.
    ///
    size_t sizeOfType() const {
      if (type == "SCALAR")
        return 1;
      if (type == "VEC2")
        return 2;
      if (type == "VEC3")
        return 3;
      if (type == "VEC4")
        return 4;
      if (type == "MAT2")
        return 4;
      if (type == "MAT3")
        return 9;
      if (type == "MAT4")
        return 16;
      return 0;
    }
  };

  /// Element of `glTF.bufferViews` property.
  ///
  struct BufferView {
    enum Target : int32_t {
      ArrayBuffer = 34962,
      ElementArrayBuffer = 34963,

      Undefined = -1
    };

    int32_t buffer = -1;
    int64_t byteOffset = 0LL;
    int64_t byteLength = -1LL;
    int32_t byteStride = -1;
    int32_t target = Undefined;
    string name{};
  };

  /// Element of `glTF.buffers` property.
  ///
  struct Buffer {
    string uri{};
    int64_t byteLength = -1LL;
    string name{};
  };

  /// `glTF.asset` property.
  ///
  struct Asset {
    string copyright{};
    string generator{};
    string version{};
    string minVersion{};
  };

  /// Getters.
  ///
  const string& directory() const {
    return directory_;
  }

  int32_t scene() const {
    return scene_;
  }

  const vector<Scene>& scenes() const {
    return scenes_;
  }

  const vector<Node>& nodes() const {
    return nodes_;
  }

  const vector<Mesh>& meshes() const {
    return meshes_;
  }

  const vector<Skin>& skins() const {
    return skins_;
  }

  const vector<Material>& materials() const {
    return materials_;
  }

  const vector<Texture>& textures() const {
    return textures_;
  }

  const vector<Sampler>& samplers() const {
    return samplers_;
  }

  const vector<Image>& images() const {
    return images_;
  }

  const vector<Camera>& cameras() const {
    return cameras_;
  }

  const vector<Animation>& animations() const {
    return animations_;
  }

  const vector<Accessor>& accessors() const {
    return accessors_;
  }

  const vector<BufferView>& bufferViews() const {
    return bufferViews_;
  }

  const vector<Buffer>& buffers() const {
    return buffers_;
  }

  const Asset& asset() const {
    return asset_;
  }

 private:
  string directory_{};
  int32_t scene_ = -1;
  vector<Scene> scenes_{};
  vector<Node> nodes_{};
  vector<Mesh> meshes_{};
  vector<Skin> skins_{};
  vector<Material> materials_{};
  vector<Texture> textures_{};
  vector<Sampler> samplers_{};
  vector<Image> images_{};
  vector<Camera> cameras_{};
  vector<Animation> animations_{};
  vector<Accessor> accessors_{};
  vector<BufferView> bufferViews_{};
  vector<Buffer> buffers_{};
  Asset asset_{};

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
  void parseStrArray(Symbol& symbol, vector<string>& dst, bool clear = false) {
    if (clear)
      dst.clear();

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

  /// Parses a wide integer number.
  ///
  void parseNum(Symbol& symbol, int64_t& dst, bool next = true) {
    if (next || symbol.type() != Symbol::Num)
      symbol.consumeUntil(Symbol::Num);

    dst = stoll(symbol.tokens());
  }

  /// Parses a floating-point number.
  ///
  void parseNum(Symbol& symbol, float& dst, bool next = true) {
    if (next || symbol.type() != Symbol::Num)
      symbol.consumeUntil(Symbol::Num);

    dst = stof(symbol.tokens());
  }

  /// Parses a wide floating-point number.
  ///
  void parseNum(Symbol& symbol, double& dst, bool next = true) {
    if (next || symbol.type() != Symbol::Num)
      symbol.consumeUntil(Symbol::Num);

    dst = stod(symbol.tokens());
  }

  /// Parses an array of numbers.
  ///
  template<class T>
  void parseNumArray(Symbol& symbol, vector<T>& dst, bool clear = false) {
    static_assert(is_arithmetic<T>(),
                  "parseNumArray() requires a numeric type");
    if (clear)
      dst.clear();

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

  /// Parses a boolean value.
  ///
  void parseBool(Symbol& symbol, bool& dst, bool next = true) {
    if (next || symbol.type() != Symbol::Bool)
      symbol.consumeUntil(Symbol::Bool);

    dst = symbol.tokens() == "true";
  }

  /// Parses `gltf.scene`.
  ///
  void parseScene(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "scene");

    parseNum(symbol, scene_);
  }

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

  /// Parses `glTF.meshes`.
  ///
  void parseMeshes(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "meshes");

    auto parseDictionary = [&](unordered_map<string, int32_t>& dict) {
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
            parseDictionary(prim.attributes);
          else if (symbol.tokens() == "indices")
            parseNum(symbol, prim.indices);
          else if (symbol.tokens() == "mode")
            parseNum(symbol, prim.mode);
          else if (symbol.tokens() == "material")
            parseNum(symbol, prim.material);
          else if (symbol.tokens() == "targets")
            parseObjectArray(symbol, [&] {
              prim.targets.push_back({});
              parseDictionary(prim.targets.back());
            });
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

  /// Parses `glTF.skins`.
  ///
  void parseSkins(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "skins");

    parseObjectArray(symbol, [&] {
      skins_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "inverseBindMatrices")
            parseNum(symbol, skins_.back().inverseBindMatrices);
          else if (symbol.tokens() == "skeleton")
            parseNum(symbol, skins_.back().skeleton);
          else if (symbol.tokens() == "joints")
            parseNumArray(symbol, skins_.back().joints);
          else if (symbol.tokens() == "name")
            parseStr(symbol, skins_.back().name);
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

  /// Parses `glTF.materials`.
  ///
  void parseMaterials(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "materials");

    auto parseTextureInfo = [&](Material::TextureInfo& dst) {
      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "index")
            parseNum(symbol, dst.index);
          else if (symbol.tokens() == "texCoord")
            parseNum(symbol, dst.texCoord);
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

    auto parsePbrMetallicRoughness = [&] {
      auto& pbrmr = materials_.back().pbrMetallicRoughness;

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "baseColorTexture")
            parseTextureInfo(pbrmr.baseColorTexture);
          else if (symbol.tokens() == "baseColorFactor")
            parseNumArray(symbol, pbrmr.baseColorFactor, true);
          else if (symbol.tokens() == "metallicRoughnessTexture")
            parseTextureInfo(pbrmr.metallicRoughnessTexture);
          else if (symbol.tokens() == "metallicFactor")
            parseNum(symbol, pbrmr.metallicFactor);
          else if (symbol.tokens() == "roughnessFactor")
            parseNum(symbol, pbrmr.roughnessFactor);
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

    auto parseNormalTexture = [&] {
      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "index")
            parseNum(symbol, materials_.back().normalTexture.index);
          else if (symbol.tokens() == "texCoord")
            parseNum(symbol, materials_.back().normalTexture.texCoord);
          else if (symbol.tokens() == "scale")
            parseNum(symbol, materials_.back().normalTexture.scale);
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

    auto parseOcclusionTexture = [&] {
      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "index")
            parseNum(symbol, materials_.back().occlusionTexture.index);
          else if (symbol.tokens() == "texCoord")
            parseNum(symbol, materials_.back().occlusionTexture.texCoord);
          else if (symbol.tokens() == "strength")
            parseNum(symbol, materials_.back().occlusionTexture.strength);
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
      materials_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "pbrMetallicRoughness")
            parsePbrMetallicRoughness();
          else if (symbol.tokens() == "normalTexture")
            parseNormalTexture();
          else if (symbol.tokens() == "occlusionTexture")
            parseOcclusionTexture();
          else if (symbol.tokens() == "emissiveTexture")
            parseTextureInfo(materials_.back().emissiveTexture);
          else if (symbol.tokens() == "emissiveFactor")
            parseNumArray(symbol, materials_.back().emissiveFactor, true);
          else if (symbol.tokens() == "alphaMode")
            parseStr(symbol, materials_.back().alphaMode);
          else if (symbol.tokens() == "alphaCutoff")
            parseNum(symbol, materials_.back().alphaCutoff);
          else if (symbol.tokens() == "doubleSided")
            parseBool(symbol, materials_.back().doubleSided);
          else if (symbol.tokens() == "name")
            parseStr(symbol, materials_.back().name);
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

  /// Parses `glTF.textures`.
  ///
  void parseTextures(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "textures");

    parseObjectArray(symbol, [&] {
      textures_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "sampler")
            parseNum(symbol, textures_.back().sampler);
          else if (symbol.tokens() == "source")
            parseNum(symbol, textures_.back().source);
          else if (symbol.tokens() == "name")
            parseStr(symbol, textures_.back().name);
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

  /// Parses `glTF.samplers`.
  ///
  void parseSamplers(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "samplers");

    parseObjectArray(symbol, [&] {
      samplers_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "wrapS")
            parseNum(symbol, samplers_.back().wrapS);
          else if (symbol.tokens() == "wrapT")
            parseNum(symbol, samplers_.back().wrapT);
          else if (symbol.tokens() == "magFilter")
            parseNum(symbol, samplers_.back().magFilter);
          else if (symbol.tokens() == "minFilter")
            parseNum(symbol, samplers_.back().minFilter);
          else if (symbol.tokens() == "name")
            parseStr(symbol, samplers_.back().name);
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

  /// Parses `gltf.images`.
  ///
  void parseImages(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "images");

    parseObjectArray(symbol, [&] {
      images_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "uri")
            parseStr(symbol, images_.back().uri);
          else if (symbol.tokens() == "mimeType")
            parseStr(symbol, images_.back().mimeType);
          else if (symbol.tokens() == "bufferView")
            parseNum(symbol, images_.back().bufferView);
          else if (symbol.tokens() == "name")
            parseStr(symbol, images_.back().name);
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

  /// Parses `glTF.cameras`.
  ///
  void parseCameras(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "cameras");

    auto parsePerspective = [&] {
      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "aspectRatio")
            parseNum(symbol, cameras_.back().perspective.aspectRatio);
          else if (symbol.tokens() == "yfov")
            parseNum(symbol, cameras_.back().perspective.yfov);
          else if (symbol.tokens() == "zfar")
            parseNum(symbol, cameras_.back().perspective.zfar);
          else if (symbol.tokens() == "znear")
            parseNum(symbol, cameras_.back().perspective.znear);
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

    auto parseOrthographic = [&] {
      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "xmag")
            parseNum(symbol, cameras_.back().orthographic.xmag);
          else if (symbol.tokens() == "ymag")
            parseNum(symbol, cameras_.back().orthographic.ymag);
          else if (symbol.tokens() == "zfar")
            parseNum(symbol, cameras_.back().orthographic.zfar);
          else if (symbol.tokens() == "znear")
            parseNum(symbol, cameras_.back().orthographic.znear);
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
      cameras_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "type")
            parseStr(symbol, cameras_.back().type);
          else if (symbol.tokens() == "perspective")
            parsePerspective();
          else if (symbol.tokens() == "orthographic")
            parseOrthographic();
          else if (symbol.tokens() == "name")
            parseStr(symbol, cameras_.back().name);
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

  /// Parses `glTF.animations`.
  ///
  void parseAnimations(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "animations");

    auto parseChannel = [&] {
      animations_.back().channels.push_back({});
      auto& ch = animations_.back().channels.back();

      auto parseTarget = [&] {
        while (true) {
          switch (symbol.next()) {
          case Symbol::Str:
            if (symbol.tokens() == "node")
              parseNum(symbol, ch.target.node);
            else if (symbol.tokens() == "path")
              parseStr(symbol, ch.target.path);
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

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "sampler")
            parseNum(symbol, ch.sampler);
          else if (symbol.tokens() == "target")
            parseTarget();
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

    auto parseSampler = [&] {
      animations_.back().samplers.push_back({});
      auto& sp = animations_.back().samplers.back();

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "input")
            parseNum(symbol, sp.input);
          else if (symbol.tokens() == "interpolation")
            parseStr(symbol, sp.interpolation);
          else if (symbol.tokens() == "output")
            parseNum(symbol, sp.output);
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
      animations_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "channels")
            parseObjectArray(symbol, parseChannel);
          else if (symbol.tokens() == "samplers")
            parseObjectArray(symbol, parseSampler);
          else if (symbol.tokens() == "name")
            parseStr(symbol, animations_.back().name);
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

  /// Parses `glTF.accessors`.
  ///
  void parseAccessors(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "accessors");

    auto parseSparse = [&] {
      auto& sparse = accessors_.back().sparse;

      auto parseIndices = [&] {
        while (true) {
          switch (symbol.next()) {
          case Symbol::Str:
            if (symbol.tokens() == "bufferView")
              parseNum(symbol, sparse.indices.bufferView);
            else if (symbol.tokens() == "byteOffset")
              parseNum(symbol, sparse.indices.byteOffset);
            else if (symbol.tokens() == "componentType")
              parseNum(symbol, sparse.indices.componentType);
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

      auto parseValues = [&] {
        while (true) {
          switch (symbol.next()) {
          case Symbol::Str:
            if (symbol.tokens() == "bufferView")
              parseNum(symbol, sparse.values.bufferView);
            else if (symbol.tokens() == "byteOffset")
              parseNum(symbol, sparse.values.byteOffset);
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

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "count")
            parseNum(symbol, sparse.count);
          else if (symbol.tokens() == "indices")
            parseIndices();
          else if (symbol.tokens() == "values")
            parseValues();
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
      accessors_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "bufferView")
            parseNum(symbol, accessors_.back().bufferView);
          else if (symbol.tokens() == "byteOffset")
            parseNum(symbol, accessors_.back().byteOffset);
          else if (symbol.tokens() == "componentType")
            parseNum(symbol, accessors_.back().componentType);
          else if (symbol.tokens() == "normalized")
            parseBool(symbol, accessors_.back().normalized);
          else if (symbol.tokens() == "count")
            parseNum(symbol, accessors_.back().count);
          else if (symbol.tokens() == "type")
            parseStr(symbol, accessors_.back().type);
          else if (symbol.tokens() == "min")
            parseNumArray(symbol, accessors_.back().min);
          else if (symbol.tokens() == "max")
            parseNumArray(symbol, accessors_.back().max);
          else if (symbol.tokens() == "sparse")
            parseSparse();
          else if (symbol.tokens() == "name")
            parseStr(symbol, accessors_.back().name);
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

  /// Parses `glTF.bufferViews`.
  ///
  void parseBufferViews(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "bufferViews");

    parseObjectArray(symbol, [&] {
      bufferViews_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "buffer")
            parseNum(symbol, bufferViews_.back().buffer);
          else if (symbol.tokens() == "byteOffset")
            parseNum(symbol, bufferViews_.back().byteOffset);
          else if (symbol.tokens() == "byteLength")
            parseNum(symbol, bufferViews_.back().byteLength);
          else if (symbol.tokens() == "byteStride")
            parseNum(symbol, bufferViews_.back().byteStride);
          else if (symbol.tokens() == "target")
            parseNum(symbol, bufferViews_.back().target);
          else if (symbol.tokens() == "name")
            parseStr(symbol, bufferViews_.back().name);
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

  /// Parses `glTF.buffers`.
  ///
  void parseBuffers(Symbol& symbol) {
    assert(symbol.type() == Symbol::Str);
    assert(symbol.tokens() == "buffers");

    parseObjectArray(symbol, [&] {
      buffers_.push_back({});

      while (true) {
        switch (symbol.next()) {
        case Symbol::Str:
          if (symbol.tokens() == "uri")
            parseStr(symbol, buffers_.back().uri);
          else if (symbol.tokens() == "byteLength")
            parseNum(symbol, buffers_.back().byteLength);
          else if (symbol.tokens() == "name")
            parseStr(symbol, buffers_.back().name);
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

#ifdef YF_DEVEL
  friend void printGLTF(const GLTF&);
#endif
};

/// Loads a single mesh from a GLTF object.
///
void loadMesh(Mesh::Data& dst, unordered_map<int32_t, ifstream>& bufferMap,
              const GLTF& gltf, size_t index) {

  assert(index < gltf.meshes().size());

  const auto& mesh = gltf.meshes()[index];

  if (mesh.primitives.empty())
    throw runtime_error("Invalid glTF primitives");

  // TODO: multiple primitives & other primitive topologies
  if (mesh.primitives.size() > 1 ||
      mesh.primitives.front().mode != GLTF::Mesh::Primitive::Triangles)
    throw UnsupportedExcept("Unsupported glTF mesh");

  // Convert from primitive's attribute string to `VxType` value
  auto typeOfAttribute = [](const string& attr) -> VxType {
    if (attr == "POSITION")
      return VxTypePosition;
    if (attr == "TANGENT")
      return VxTypeTangent;
    if (attr == "NORMAL")
      return VxTypeNormal;
    if (attr == "TEXCOORD_0")
      return VxTypeTexCoord0;
    if (attr == "TEXCOORD_1")
      return VxTypeTexCoord1;
    if (attr == "COLOR_0")
      return VxTypeColor0;
    if (attr == "JOINTS_0")
      return VxTypeJoints0;
    if (attr == "WEIGHTS_0")
      return VxTypeWeights0;
    throw UnsupportedExcept("Unsupported glTF primitive");
  };

  // Description of data in a given buffer
  struct Desc {
    int32_t type;
    const GLTF::Accessor& accessor;
    const GLTF::BufferView& bufferView;
  };

  unordered_map<int32_t, vector<Desc>> descMap{};

  // TODO: validate glTF data
  for (const auto& prim : mesh.primitives) {
    for (const auto& att : prim.attributes) {

      const auto type = typeOfAttribute(att.first);
      const auto& acc = gltf.accessors()[att.second];
      const auto& view = gltf.bufferViews()[acc.bufferView];

      auto it = descMap.find(view.buffer);
      if (it == descMap.end())
        descMap.emplace(view.buffer, vector<Desc>{{type, acc, view}});
      else
        it->second.push_back({type, acc, view});
    }

    if (prim.indices >= 0) {
      const auto& acc = gltf.accessors()[prim.indices];
      const auto& view = gltf.bufferViews()[acc.bufferView];

      auto it = descMap.find(view.buffer);
      assert(it != descMap.end());
      it->second.push_back({-1, acc, view});
    }
  }

  for (const auto& dm : descMap) {
    const auto& buffer = gltf.buffers()[dm.first];

    // TODO: .glb
    if (buffer.uri.empty())
      throw UnsupportedExcept("Unsupported glTF buffer");

    auto it = bufferMap.find(dm.first);

    if (it == bufferMap.end()) {
      const auto pathname = gltf.directory() + '/' + buffer.uri;
      it = bufferMap.emplace(dm.first, ifstream(pathname)).first;
      if (!it->second)
        throw FileExcept("Could not open glTF .bin file");
    }

    auto& ifs = it->second;

    for (const auto& dc : dm.second) {
      size_t size = dc.accessor.sizeOfComponentType() *
                    dc.accessor.sizeOfType();

      const Mesh::Data::Accessor da{static_cast<uint32_t>(dst.data.size()),
                                    0,
                                    static_cast<uint32_t>(dc.accessor.count),
                                    static_cast<uint32_t>(size)};

      size *= da.elementN;
      dst.data.push_back(make_unique<uint8_t[]>(size));
      auto dt = reinterpret_cast<char*>(dst.data.back().get());

      if (!ifs.seekg(dc.accessor.byteOffset + dc.bufferView.byteOffset))
        throw FileExcept("Could not seek glTF .bin file");

      if (dc.bufferView.byteStride > 0) {
        // interleaved
        for (size_t i = 0; i < da.elementN; ++i) {
          if (!ifs.seekg(dc.bufferView.byteStride * i, ios_base::cur))
            throw FileExcept("Could not seek glTF .bin file");
          if (!ifs.read(dt, da.elementSize))
            throw FileExcept("Could not read from glTF .bin file");
        }
      } else {
        // packed
        if (!ifs.read(dt, size))
          throw FileExcept("Could not read from glTF .bin file");
      }

      if (dc.type >= 0)
        dst.vxAccessors.emplace(static_cast<VxType>(dc.type), da);
      else
        dst.ixAccessor = da;
    }
  }
}

/// Loads a single material from a GLTF object.
///
void loadMaterial(Material& dst, unordered_map<int32_t, Texture>& textureMap,
                  const GLTF& gltf, size_t index) {

  assert(index < gltf.materials().size());

  const auto& material = gltf.materials()[index];

  // Gets texture
  auto getTexture = [&](const GLTF::Material::TextureInfo& info) -> Texture {
    if (info.index < 0)
      return {};

    auto it = textureMap.find(info.index);
    if (it != textureMap.end())
      return it->second;

    const auto& texture = gltf.textures()[info.index];
    const auto& image = gltf.images()[texture.source];

    // TODO: image type check & support for buffer view
    if (image.uri == "")
      throw runtime_error("glTF image load from buffer view unimplemented");

    wstring pathname;
    for (const auto& c : gltf.directory())
      pathname.push_back(c);
    pathname.push_back('/');
    for (const auto& c : image.uri)
      pathname.push_back(c);

    Texture tex(Texture::Png, pathname);
    return textureMap.emplace(info.index, tex).first->second;
  };

  // PBRMR
  const auto& pbrmr = material.pbrMetallicRoughness;
  dst.pbrmr().colorTex = getTexture(pbrmr.baseColorTexture);
  dst.pbrmr().colorFac = {pbrmr.baseColorFactor[0], pbrmr.baseColorFactor[1],
                          pbrmr.baseColorFactor[2], pbrmr.baseColorFactor[3]};
  dst.pbrmr().metalRoughTex = getTexture(pbrmr.metallicRoughnessTexture);
  dst.pbrmr().metallic = pbrmr.metallicFactor;
  dst.pbrmr().roughness = pbrmr.roughnessFactor;

  // Normal
  const auto& normal = material.normalTexture;
  dst.normal().texture = getTexture(normal);
  dst.normal().scale = normal.scale;

  // Occlusion
  const auto& occlusion = material.occlusionTexture;
  dst.occlusion().texture = getTexture(occlusion);
  dst.occlusion().strength = occlusion.strength;

  // Emissive
  const auto& emissive = material.emissiveTexture;
  dst.emissive().texture = getTexture(emissive);
  dst.emissive().factor = {material.emissiveFactor[0],
                           material.emissiveFactor[1],
                           material.emissiveFactor[2]};
}

/// Loads a single skin from a GLTF object.
///
void loadSkin(Skin& dst, unordered_map<int32_t, ifstream>& bufferMap,
              const GLTF& gltf, size_t index) {

  assert(index < gltf.skins().size());

  const auto& skin = gltf.skins()[index];

  // Bind-pose
  vector<Mat4f> bindPose;

  for (const auto& jt : skin.joints) {
    const auto& xform = gltf.nodes()[jt].transform;
    if (xform.size() == 16) {
      bindPose.push_back({{xform[0], xform[1], xform[2], xform[3]},
                          {xform[4], xform[5], xform[6], xform[7]},
                          {xform[8], xform[9], xform[10], xform[11]},
                          {xform[12], xform[13], xform[14], xform[15]}});
    } else {
      auto t = translate(xform[0], xform[1], xform[2]);
      auto r = rotate(Qnionf({xform[3], xform[4], xform[5], xform[6]}));
      auto s = scale(xform[7], xform[8], xform[9]);
      bindPose.push_back(t * r * s);
    }
  }

  // Inverse-bind
  vector<Mat4f> inverseBind;

  if (skin.inverseBindMatrices > 0) {
    const auto& acc = gltf.accessors()[skin.inverseBindMatrices];
    const auto& view = gltf.bufferViews()[acc.bufferView];
    const auto& buffer = gltf.buffers()[view.buffer];

    auto it = bufferMap.find(view.buffer);

    if (it == bufferMap.end()) {
      const auto pathname = gltf.directory() + '/' + buffer.uri;
      it = bufferMap.emplace(view.buffer, ifstream(pathname)).first;
      if (!it->second)
        throw FileExcept("Could not open glTF .bin file");
    }

    auto& ifs = it->second;

    if (!ifs.seekg(acc.byteOffset + view.byteOffset))
      throw FileExcept("Could not seek glTF .bin file");

    inverseBind.resize(acc.count);
    for (auto& m : inverseBind) {
      auto dt = reinterpret_cast<char*>(m.data());
      if (!ifs.read(dt, Mat4f::dataSize()))
        throw FileExcept("Could not read from glTF .bin file");
    }
  }

  // Create skin
  dst = {bindPose, inverseBind};
}

/// Loads contents from a GLTF object.
///
void loadContents(Collection& collection, const GLTF& gltf,
                  int32_t sceneIndex, int32_t nodeIndex) {

  // Check which nodes are joints
  vector<bool> isJoint(gltf.nodes().size(), false);
  for (const auto& sk : gltf.skins()) {
    for (const auto& jt : sk.joints)
      isJoint[jt] = true;
  }

  // Select nodes
  vector<int32_t> nodes;

  auto setDescendants = [&] {
    for (size_t i = 0; i < nodes.size(); ++i) {
      for (const auto& nd : gltf.nodes()[nodes[i]].children) {
        if (!isJoint[nd])
          nodes.push_back(nd);
      }
    }
  };

  if (sceneIndex > -1) {
    // Nodes from a specific scene
    for (const auto& nd : gltf.scenes()[sceneIndex].nodes)
      nodes.push_back(nd);
    setDescendants();
  } else if (nodeIndex > -1) {
    // Nodes from a specific subgraph
    nodes.push_back(nodeIndex);
    setDescendants();
  } else {
    // Everything
    for (const auto& scn : gltf.scenes()) {
      for (const auto& nd : scn.nodes) {
        if (!isJoint[nd])
          nodes.push_back(nd);
      }
    }
    if (nodes.empty()) {
      for (size_t nd = 0; nd < gltf.nodes().size(); ++nd) {
        if (!isJoint[nd])
          nodes.push_back(nd);
      }
    } else {
      setDescendants();
    }
  }

  // Select resources
  vector<int32_t> meshes;
  vector<int32_t> materials;
  vector<int32_t> skins;

  unordered_map<int32_t, ifstream> bufferMap;
  unordered_map<int32_t, Texture> textureMap;

  if (sceneIndex < 0 && nodeIndex < 0) {
    // Everything
    for (size_t i = 0; i < gltf.meshes().size(); ++i)
      meshes.push_back(i);
    for (size_t i = 0; i < gltf.materials().size(); ++i)
      materials.push_back(i);
    for (size_t i = 0; i < gltf.skins().size(); ++i)
      skins.push_back(i);
    // TODO...
  } else {
    // Only referenced resources
    // TODO...
  }

  // Create meshes
  for (const auto& mesh : meshes) {
    Mesh::Data data;
    loadMesh(data, bufferMap, gltf, mesh);
    collection.meshes().push_back({data});
  }

  // Create materials
  for (const auto& matl : materials) {
    collection.materials().push_back({});
    loadMaterial(collection.materials().back(), textureMap, gltf, matl);
  }

  // Create skins
  for (const auto& sk : skins) {
    collection.skins().push_back({});
    loadSkin(collection.skins().back(), bufferMap, gltf, sk);
  }

  // Store textures
  // XXX: redundant for textures used by materials
  for (const auto& kv : textureMap)
    collection.textures().push_back(kv.second);

  // Create nodes
  // TODO: joint nodes
  for (const auto& nd : nodes) {
    const auto& node = gltf.nodes()[nd];

    // Transform
    Mat4f xform;

    if (node.transform.size() == 16) {
      xform[0] = {node.transform[0], node.transform[1],
                  node.transform[2], node.transform[3]};
      xform[1] = {node.transform[4], node.transform[5],
                  node.transform[6], node.transform[7]};
      xform[2] = {node.transform[8], node.transform[9],
                  node.transform[10], node.transform[11]};
      xform[3] = {node.transform[12], node.transform[13],
                  node.transform[14], node.transform[15]};
    } else {
      auto t = translate(node.transform[0], node.transform[1],
                         node.transform[2]);
      auto r = rotate(Qnionf({node.transform[3], node.transform[4],
                              node.transform[5], node.transform[6]}));
      auto s = scale(node.transform[7], node.transform[8], node.transform[9]);
      xform = t * r * s;
    }

    // Node
    if (node.mesh > -1) {
      collection.models().push_back({});
      auto& mdl = collection.models().back();

      mdl.setMesh(collection.meshes()[meshes[node.mesh]]);
      const auto matl = gltf.meshes()[node.mesh].primitives[0].material;
      if (matl > -1)
        mdl.setMaterial(collection.materials()[materials[matl]]);
      if (node.skin > -1)
        mdl.setSkin(collection.skins()[skins[node.skin]]);
      mdl.transform() = xform;

    } else {
      collection.nodes().push_back({});
      collection.nodes().back().transform() = xform;
    }
  }

  // TODO...
}

INTERNAL_NS_END

void SG_NS::loadGLTF(Collection& collection, const wstring& pathname) {
  GLTF gltf(pathname);

#ifdef YF_DEVEL
  printGLTF(gltf);
#endif

  loadContents(collection, gltf, -1, -1);
}

void SG_NS::loadGLTF(Scene& dst, Collection& collection,
                     const wstring& pathname, size_t index) {
  GLTF gltf(pathname);

#ifdef YF_DEVEL
  printGLTF(gltf);
#endif

  if (index >= gltf.scenes().size())
    throw invalid_argument("loadGLTF() index out of bounds");

  // TODO
  throw runtime_error("loadGLTF(Scene) unimplemented");
}

void SG_NS::loadGLTF(Node& dst, Collection& collection,
                     const wstring& pathname, size_t index) {
  GLTF gltf(pathname);

#ifdef YF_DEVEL
  printGLTF(gltf);
#endif

  if (index >= gltf.nodes().size())
    throw invalid_argument("loadGLTF() index out of bounds");

  // TODO
  throw runtime_error("loadGLTF(Node) unimplemented");
}

void SG_NS::loadGLTF(Mesh::Data& dst, const wstring& pathname, size_t index) {
  GLTF gltf(pathname);

#ifdef YF_DEVEL
  printGLTF(gltf);
#endif

  if (index >= gltf.meshes().size())
    throw invalid_argument("loadGLTF() index out of bounds");

  unordered_map<int32_t, ifstream> bufferMap;
  loadMesh(dst, bufferMap, gltf, index);
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
    wprintf(L"\n   nodes: [ ");
    for (auto nd : scn.nodes)
      wprintf(L"%d ", nd);
    wprintf(L"]");
  }

  wprintf(L"\n nodes:");
  for (const auto& nd : gltf.nodes_) {
    wprintf(L"\n  node `%s`:", nd.name.data());
    wprintf(L"\n   children: [ ");
    for (auto ch : nd.children)
      wprintf(L"%d ", ch);
    wprintf(L"]");
    wprintf(L"\n   camera: %d", nd.camera);
    wprintf(L"\n   mesh: %d", nd.mesh);
    wprintf(L"\n   skin: %d", nd.skin);
    wprintf(L"\n   weights: [ ");
    for (auto wt : nd.weights)
      wprintf(L"%.6f ", wt);
    wprintf(L"]");
    wprintf(L"\n   transform:");
    if (nd.transform.size() == 10) {
      wprintf(L"\n    T: [ ");
      for (size_t i = 0; i < 3; ++i)
        wprintf(L"%.6f ", nd.transform[i]);
      wprintf(L"]");
      wprintf(L"\n    R: [ ");
      for (size_t i = 3; i < 7; ++i)
        wprintf(L"%.6f ", nd.transform[i]);
      wprintf(L"]");
      wprintf(L"\n    S: [ ");
      for (size_t i = 7; i < 10; ++i)
        wprintf(L"%.6f ", nd.transform[i]);
      wprintf(L"]");
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
      wprintf(L"\n    mode: %d", prm.mode);
      wprintf(L"\n    material: %d", prm.material);
      for (const auto& tgt : prm.targets) {
        wprintf(L"\n     target:");
        for (const auto& att : tgt)
          wprintf(L"\n      %s : %d", att.first.data(), att.second);
      }
    }
    wprintf(L"\n   weights: [ ");
    for (auto wt : msh.weights)
      wprintf(L"%.6f ", wt);
    wprintf(L"]");
  }

  wprintf(L"\n skins:");
  for (const auto& sk : gltf.skins_) {
    wprintf(L"\n  skin `%s`:", sk.name.data());
    wprintf(L"\n   inversebindMatrices: %d", sk.inverseBindMatrices);
    wprintf(L"\n   skeleton: %d", sk.skeleton);
    wprintf(L"\n   joints: [ ");
    for (auto jt : sk.joints)
      wprintf(L"%d ", jt);
    wprintf(L"]");
  }

  wprintf(L"\n materials:");
  for (const auto& ml : gltf.materials_) {
    wprintf(L"\n  material `%s`:", ml.name.data());
    wprintf(L"\n   pbrMetallicRoughness:");
    wprintf(L"\n    baseColorTexture:");
    wprintf(L"\n     index: %d",
            ml.pbrMetallicRoughness.baseColorTexture.index);
    wprintf(L"\n     texCoord: %d",
            ml.pbrMetallicRoughness.baseColorTexture.texCoord);
    wprintf(L"\n    baseColorFactor: [ ");
    for (auto v : ml.pbrMetallicRoughness.baseColorFactor)
      wprintf(L"%.6f ", v);
    wprintf(L"]");
    wprintf(L"\n    metallicRoughnessTexture:");
    wprintf(L"\n     index: %d",
            ml.pbrMetallicRoughness.metallicRoughnessTexture.index);
    wprintf(L"\n     texCoord: %d",
            ml.pbrMetallicRoughness.metallicRoughnessTexture.texCoord);
    wprintf(L"\n    metallicFactor: %.6f",
            ml.pbrMetallicRoughness.metallicFactor);
    wprintf(L"\n    roughnessFactor: %.6f",
            ml.pbrMetallicRoughness.roughnessFactor);
    wprintf(L"\n   normalTexture:");
    wprintf(L"\n    index: %d", ml.normalTexture.index);
    wprintf(L"\n    texCoord: %d", ml.normalTexture.texCoord);
    wprintf(L"\n    scale: %.6f", ml.normalTexture.scale);
    wprintf(L"\n   occlusionTexture:");
    wprintf(L"\n    index: %d", ml.occlusionTexture.index);
    wprintf(L"\n    texCoord: %d", ml.occlusionTexture.texCoord);
    wprintf(L"\n    strength: %.6f", ml.occlusionTexture.strength);
    wprintf(L"\n   emissiveTexture:");
    wprintf(L"\n    index: %d", ml.emissiveTexture.index);
    wprintf(L"\n    texCoord: %d", ml.emissiveTexture.texCoord);
    wprintf(L"\n   emissiveFactor: [ ");
    for (auto v : ml.emissiveFactor)
      wprintf(L"%.6f ", v);
    wprintf(L"]");
    wprintf(L"\n   alphaMode: `%s`", ml.alphaMode.data());
    wprintf(L"\n   alphaCutoff: %.6f", ml.alphaCutoff);
    wprintf(L"\n   doubleSided: %s", ml.doubleSided ? "true" : "false");
  }

  wprintf(L"\n textures:");
  for (const auto& tex : gltf.textures_) {
    wprintf(L"\n  texture `%s`:", tex.name.data());
    wprintf(L"\n   sampler: %d", tex.sampler);
    wprintf(L"\n   source: %d", tex.source);
  }

  wprintf(L"\n samplers:");
  for (const auto& spl : gltf.samplers_) {
    wprintf(L"\n  sampler `%s`:", spl.name.data());
    wprintf(L"\n   wrapS: %d", spl.wrapS);
    wprintf(L"\n   wrapT: %d", spl.wrapT);
    wprintf(L"\n   magFilter: %d", spl.magFilter);
    wprintf(L"\n   minFilter: %d", spl.minFilter);
  }

  wprintf(L"\n images:");
  for (const auto& img : gltf.images_) {
    wprintf(L"\n  image `%s`:", img.name.data());
    wprintf(L"\n   uri: %s", img.uri.data());
    wprintf(L"\n   mimeType: %s", img.mimeType.data());
    wprintf(L"\n   bufferView: %d", img.bufferView);
  }

  wprintf(L"\n cameras:");
  for (const auto& cam : gltf.cameras_) {
    wprintf(L"\n  camera `%s`:", cam.name.data());
    wprintf(L"\n   type: %s", cam.type.data());
    if (cam.type == "perspective") {
      wprintf(L"\n   perspective:");
      wprintf(L"\n    aspectRatio: %.6f", cam.perspective.aspectRatio);
      wprintf(L"\n    yfov: %.6f", cam.perspective.yfov);
      wprintf(L"\n    zfar: %.6f", cam.perspective.zfar);
      wprintf(L"\n    znear: %.6f", cam.perspective.znear);
    } else if (cam.type == "orthographic") {
      wprintf(L"\n   orthographic:");
      wprintf(L"\n    xmag: %.6f", cam.orthographic.xmag);
      wprintf(L"\n    ymag: %.6f", cam.orthographic.ymag);
      wprintf(L"\n    zfar: %.6f", cam.orthographic.zfar);
      wprintf(L"\n    znear: %.6f", cam.orthographic.znear);
    } else {
      wprintf(L"\n   !unknown type!");
    }
  }

  wprintf(L"\n animations:");
  for (const auto& an : gltf.animations_) {
    wprintf(L"\n  animation `%s`:", an.name.data());
    wprintf(L"\n   channels:");
    for (const auto& ch : an.channels) {
      wprintf(L"\n    channel:");
      wprintf(L"\n     sampler: %d", ch.sampler);
      wprintf(L"\n     target:");
      wprintf(L"\n      node: %d", ch.target.node);
      wprintf(L"\n      path: %s", ch.target.path.data());
    }
    wprintf(L"\n   samplers:");
    for (const auto& sp : an.samplers) {
      wprintf(L"\n    sampler:");
      wprintf(L"\n     input: %d", sp.input);
      wprintf(L"\n     interpolation: %s", sp.interpolation.data());
      wprintf(L"\n     output: %d", sp.output);
    }
  }

  wprintf(L"\n accessors:");
  for (const auto& ac : gltf.accessors_) {
    wprintf(L"\n  accessor `%s`:", ac.name.data());
    wprintf(L"\n   bufferView: %d", ac.bufferView);
    wprintf(L"\n   byteOffset: %lld", ac.byteOffset);
    wprintf(L"\n   componentType: %d", ac.componentType);
    wprintf(L"\n   normalized: %s", ac.normalized ? "true" : "false");
    wprintf(L"\n   count: %d", ac.count);
    wprintf(L"\n   type: %s", ac.type.data());
    wprintf(L"\n   min: [ ");
    for (auto v : ac.min)
      wprintf(L"%.6f ", v);
    wprintf(L"]");
    wprintf(L"\n   max: [ ");
    for (auto v : ac.max)
      wprintf(L"%.6f ", v);
    wprintf(L"]");
    wprintf(L"\n   sparse:");
    wprintf(L"\n    count: %d", ac.sparse.count);
    wprintf(L"\n    indices:");
    wprintf(L"\n     bufferView: %d", ac.sparse.indices.bufferView);
    wprintf(L"\n     byteOffset: %lld", ac.sparse.indices.byteOffset);
    wprintf(L"\n     componentType: %d", ac.sparse.indices.componentType);
    wprintf(L"\n    values:");
    wprintf(L"\n     bufferView: %d", ac.sparse.values.bufferView);
    wprintf(L"\n     byteOffset: %lld", ac.sparse.values.byteOffset);
  }

  wprintf(L"\n bufferViews:");
  for (const auto& bv : gltf.bufferViews_) {
    wprintf(L"\n  bufferView `%s`:", bv.name.data());
    wprintf(L"\n   buffer: %d", bv.buffer);
    wprintf(L"\n   byteOffset: %lld", bv.byteOffset);
    wprintf(L"\n   byteLength: %lld", bv.byteLength);
    wprintf(L"\n   byteStride: %d", bv.byteStride);
    wprintf(L"\n   target: %d", bv.target);
  }

  wprintf(L"\n buffers:");
  for (const auto& b : gltf.buffers_) {
    wprintf(L"\n  buffer `%s`:", b.name.data());
    wprintf(L"\n   uri: %s", b.uri.data());
    wprintf(L"\n   byteLength: %lld", b.byteLength);
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
