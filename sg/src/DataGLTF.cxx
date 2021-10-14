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
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <stdexcept>

#include "DataGLTF.h"
#include "Model.h"
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
            // TODO: Other escape sequences
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
            n++;
          else if (tokens_[0] == cl)
            n--;
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
  const string& tokens() const {
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
  GLTF(const string& pathname) {
    const auto pos = pathname.find_last_of('/');
    if (pos != 0 && pos != pathname.npos)
      directory_ = {pathname.begin(), pathname.begin() + pos};

    try {
      ifs_ = new ifstream(pathname);
      if (!(*ifs_))
        throw FileExcept("Could not open glTF file");
      init(*ifs_);
      ownsStream_= true;
    } catch (...) {
      delete ifs_;
      throw;
    }
  }

  GLTF(ifstream& ifs, const string& directory)
    : directory_(directory), ownsStream_(false), ifs_(&ifs) {

    init(ifs);
  }

  GLTF(const GLTF&) = delete;
  GLTF& operator=(const GLTF&) = delete;

  ~GLTF() {
    if (ownsStream_)
      delete ifs_;
  }

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

      vector<pair<string, int32_t>> attributes{};
      int32_t indices = -1;
      int32_t mode = Triangles;
      int32_t material = -1;
      vector<vector<pair<string, int32_t>>> targets{};
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

  /// Seeks to the beginning of the binary buffer.
  ///
  /// XXX: If the `GLTF` object does not own the file stream, one must ensure
  /// that it still exists before calling this method.
  ///
  ifstream& bin() const {
    assert(buffers_.size() != 0 && buffers_[0].uri.empty());

    if (!ifs_->seekg(binOffset_))
      throw FileExcept("Could not seek glTF .glb file");

    return *ifs_;
  }

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
  bool ownsStream_ = false;
  ifstream* ifs_ = nullptr;
  ifstream::pos_type binOffset_ = -1;
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

  /// Initializes GLTF data from a file stream.
  ///
  void init(ifstream& ifs) {
    // Check whether this is a .glb or a .gltf file
    const auto beg = ifs.tellg();
    if (beg == ifstream::pos_type(-1))
      throw FileExcept("Could not tell position of glTF file");

    // TODO: Endian
    uint32_t magic;
    if (!ifs.read(reinterpret_cast<char*>(&magic), sizeof magic))
      throw FileExcept("Could not read from glTF file");

    if (magic == 0x46546C67) {
      // .glb
      uint32_t version;
      if (!ifs.read(reinterpret_cast<char*>(&version), sizeof version))
        throw FileExcept("Could not read from glTF .glb file");
      if (version != 2)
        throw UnsupportedExcept("Unsupported glTF .glb version");
      if (!ifs.seekg(4, ios_base::cur))
        throw FileExcept("Could not seek glTF .glb file");

      uint32_t jLen;
      if (!ifs.read(reinterpret_cast<char*>(&jLen), sizeof jLen))
        throw FileExcept("Could not read from glTF .glb file");
      if (!ifs.seekg(4, ios_base::cur))
        throw FileExcept("Could not seek glTF .glb file");

      binOffset_ = beg + ifstream::pos_type(28 + jLen);

    } else {
      // .gltf
      ifs.seekg(beg);
    }

    // Parse file contents
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

  /// Parses an array of objects.
  ///
  void parseObjectArray(Symbol& symbol, const function<void ()>& callback) {
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

    auto parseDictionary = [&](vector<pair<string, int32_t>>& dict) {
      string key{};
      int32_t value{};

      symbol.consumeUntil(Symbol::Str);

      while (true) {
        key = symbol.tokens();
        if (symbol.next() != Symbol::Op || symbol.token() != ':')
          throw FileExcept("Invalid glTF file");
        parseNum(symbol, value);
        dict.push_back({key, value});

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

/// GLTF data load.
///
class DataLoad {
 public:
  DataLoad(const GLTF& gltf)
    : gltf_(gltf), collection_(), buffers_(gltf.buffers().size()),
      images_(gltf.images().size()), joints_(gltf.nodes().size()) {

    collection_.scenes().resize(gltf.scenes().size());
    collection_.nodes().resize(gltf.nodes().size());
    collection_.meshes().resize(gltf.meshes().size());
    collection_.skins().resize(gltf.skins().size());
    collection_.textures().resize(gltf.textures().size());
    collection_.materials().resize(gltf.materials().size());
    collection_.animations().resize(gltf.animations().size());

    for (const auto& sk : gltf_.skins()) {
      for (const auto& jt : sk.joints)
        joints_[jt] = true;
    }
  }

  DataLoad(const DataLoad&) = delete;
  DataLoad& operator=(const DataLoad&) = delete;
  ~DataLoad() = default;

  /// Gets the collection.
  ///
  Collection& collection() {
    return collection_;
  }

  /// Loads everything.
  ///
  Collection& loadContents() {
    for (size_t i = 0; i < gltf_.scenes().size(); i++)
      loadScene(i);

    for (size_t i = 0; i < gltf_.nodes().size(); i++)
      if (!collection_.nodes()[i])
        loadGraph(i);

    for (size_t i = 0; i < gltf_.textures().size(); i++)
      loadTexture(i);
    for (size_t i = 0; i < gltf_.materials().size(); i++)
      loadMaterial(i);
    for (size_t i = 0; i < gltf_.meshes().size(); i++)
      loadMesh(i);
    for (size_t i = 0; i < gltf_.skins().size(); i++)
      loadSkin(i);
    for (size_t i = 0; i < gltf_.animations().size(); i++)
      loadAnimation(i);

    return collection_;
  }

  /// Loads a scene.
  ///
  Scene& loadScene(int32_t scene) {
    assert(scene >= 0 && static_cast<size_t>(scene) < gltf_.scenes().size());

    if (collection_.scenes()[scene])
      return *collection_.scenes()[scene];

    collection_.scenes()[scene] = make_unique<Scene>();
    auto& scn = *collection_.scenes()[scene];

    for (const auto& nd : gltf_.scenes()[scene].nodes)
      scn.insert(loadGraph(nd));

    // XXX
    auto& name = scn.name();
    for (const auto& c : gltf_.scenes()[scene].name)
      name.push_back(c);

    return scn;
  }

  /// Loads a node and its descendants.
  ///
  Node& loadGraph(int32_t rootNode) {
    auto& node = loadNode(rootNode);
    for (const auto& nd : gltf_.nodes()[rootNode].children)
      node.insert(loadGraph(nd));
    return node;
  }

  /// Loads a node.
  ///
  Node& loadNode(int32_t node) {
    assert(node >= 0 && static_cast<size_t>(node) < gltf_.nodes().size());

    if (collection_.nodes()[node])
      return *collection_.nodes()[node];

    const auto& nd = gltf_.nodes()[node];

    if (nd.mesh >= 0) {
      // Model
      if (joints_[node])
        throw UnsupportedExcept("Unsupported glTF node");

      collection_.nodes()[node] = make_unique<Model>();
      auto& model = static_cast<Model&>(*collection_.nodes()[node]);
      model.setMesh(&loadMesh(nd.mesh));

      if (nd.skin >= 0)
        model.setSkin(&loadSkin(nd.skin));

      // FIXME: Deprecated
      if (gltf_.meshes()[nd.mesh].primitives[0].material >= 0)
        model.setMaterial(&loadMaterial(gltf_.meshes()[nd.mesh]
                                        .primitives[0].material));

    } else if (joints_[node]) {
      // Joint
      collection_.nodes()[node] = make_unique<Joint>();

    } else {
      // Node
      collection_.nodes()[node] = make_unique<Node>();
    }

    auto& xform = collection_.nodes()[node]->transform();

    if (nd.transform.size() == 16) {
      xform[0] = {nd.transform[0], nd.transform[1],
                  nd.transform[2], nd.transform[3]};
      xform[1] = {nd.transform[4], nd.transform[5],
                  nd.transform[6], nd.transform[7]};
      xform[2] = {nd.transform[8], nd.transform[9],
                  nd.transform[10], nd.transform[11]};
      xform[3] = {nd.transform[12], nd.transform[13],
                  nd.transform[14], nd.transform[15]};

    } else {
      auto t = translate(nd.transform[0], nd.transform[1], nd.transform[2]);
      auto r = rotate(Qnionf({nd.transform[3], nd.transform[4],
                              nd.transform[5], nd.transform[6]}));
      auto s = scale(nd.transform[7], nd.transform[8], nd.transform[9]);

      xform = t * r * s;
    }

    // XXX
    auto& name = collection_.nodes()[node]->name();
    for (const auto& c : nd.name)
      name.push_back(c);

    return *collection_.nodes()[node];
  }

  /// Loads a texture.
  ///
  Texture& loadTexture(int32_t texture) {
    assert(texture >= 0 &&
           static_cast<size_t>(texture) < gltf_.textures().size());

    if (collection_.textures()[texture])
      return *collection_.textures()[texture];

    const auto& tex = gltf_.textures()[texture];
    CG_NS::Sampler splr{};

    if (tex.sampler > -1) {
      const auto& sampler = gltf_.samplers()[tex.sampler];

      // Convert from sampler's wrap mode to CG `WrapMode` value
      auto toWrapMode = [](int32_t wrap) -> CG_NS::WrapMode {
        switch (wrap) {
        case GLTF::Sampler::ClampToEdge:    return CG_NS::WrapModeClamp;
        case GLTF::Sampler::MirroredRepeat: return CG_NS::WrapModeMirror;
        case GLTF::Sampler::Repeat:         return CG_NS::WrapModeRepeat;
        default: throw UnsupportedExcept("Unsupported sampler wrap mode");
        }
      };

      splr.wrapU = toWrapMode(sampler.wrapS);
      splr.wrapV = toWrapMode(sampler.wrapT);

      switch (sampler.magFilter) {
      case GLTF::Sampler::Undefined:
      case GLTF::Sampler::Nearest:
        splr.magFilter = CG_NS::FilterNearest;
        break;
      case GLTF::Sampler::Linear:
        splr.magFilter = CG_NS::FilterLinear;
        break;
      default:
        throw UnsupportedExcept("Unsupported sampler mag. filter");
      }

      switch (sampler.minFilter) {
      case GLTF::Sampler::Undefined:
      case GLTF::Sampler::Nearest:
        splr.minFilter = CG_NS::FilterNearest;
        break;
      case GLTF::Sampler::Linear:
        splr.minFilter = CG_NS::FilterLinear;
        break;
      case GLTF::Sampler::NearestMipmapNearest:
        splr.minFilter = CG_NS::FilterNearestNearest;
        break;
      case GLTF::Sampler::LinearMipmapNearest:
        splr.minFilter = CG_NS::FilterLinearNearest;
        break;
      case GLTF::Sampler::NearestMipmapLinear:
        splr.minFilter = CG_NS::FilterNearestLinear;
        break;
      case GLTF::Sampler::LinearMipmapLinear:
        splr.minFilter = CG_NS::FilterLinearLinear;
        break;
      default:
        throw UnsupportedExcept("Unsupported sampler min. filter");
      }
    }

    const auto& image = loadImage(tex.source);
    collection_.textures()[texture] = make_unique<Texture>(image, splr,
                                                           TexCoordSet0);
    return *collection_.textures()[texture];
  }

  /// Loads a material.
  ///
  Material& loadMaterial(int32_t material) {
    assert(material >= 0 &&
           static_cast<size_t>(material) < gltf_.materials().size());

    if (collection_.materials()[material])
      return *collection_.materials()[material];

    // Get texture from info
    auto getTexture = [&](const GLTF::Material::TextureInfo& info) {
      if (info.index < 0)
        return Texture::Ptr{};

      TexCoordSet coordSet;
      switch (info.texCoord) {
      case 0:
        coordSet = TexCoordSet0;
        break;
      case 1:
        coordSet = TexCoordSet1;
        break;
      default:
        throw UnsupportedExcept("Unsupported texture coord. set");
      }

      const auto& texture = loadTexture(info.index);
      return make_unique<Texture>(texture, texture.sampler(), coordSet);
    };

    const auto& matl = gltf_.materials()[material];
    auto& dst = *(collection_.materials()[material] = make_unique<Material>());

    // PBRMR
    const auto& pbrmr = matl.pbrMetallicRoughness;
    dst.pbrmr().colorTex = getTexture(pbrmr.baseColorTexture);
    dst.pbrmr().colorFac = {pbrmr.baseColorFactor[0],
                            pbrmr.baseColorFactor[1],
                            pbrmr.baseColorFactor[2],
                            pbrmr.baseColorFactor[3]};
    dst.pbrmr().metalRoughTex = getTexture(pbrmr.metallicRoughnessTexture);
    dst.pbrmr().metallic = pbrmr.metallicFactor;
    dst.pbrmr().roughness = pbrmr.roughnessFactor;

    // Normal
    const auto& normal = matl.normalTexture;
    dst.normal().texture = getTexture(normal);
    dst.normal().scale = normal.scale;

    // Occlusion
    const auto& occlusion = matl.occlusionTexture;
    dst.occlusion().texture = getTexture(occlusion);
    dst.occlusion().strength = occlusion.strength;

    // Emissive
    const auto& emissive = matl.emissiveTexture;
    dst.emissive().texture = getTexture(emissive);
    dst.emissive().factor = {matl.emissiveFactor[0],
                             matl.emissiveFactor[1],
                             matl.emissiveFactor[2]};

    return dst;
  }

  /// Loads a mesh.
  ///
  Mesh& loadMesh(int32_t mesh) {
    assert(mesh >= 0 && static_cast<size_t>(mesh) < gltf_.meshes().size());

    if (collection_.meshes()[mesh])
      return *collection_.meshes()[mesh];

    // Convert from primitive's attribute string to `VxData` value
    auto toVxData = [](const string& att) -> VxData {
      if (att == "POSITION")   return VxDataPosition;
      if (att == "NORMAL")     return VxDataNormal;
      if (att == "TANGENT")    return VxDataTangent;
      if (att == "TEXCOORD_0") return VxDataTexCoord0;
      if (att == "TEXCOORD_1") return VxDataTexCoord1;
      if (att == "COLOR_0")    return VxDataColor0;
      if (att == "JOINTS_0")   return VxDataJoints0;
      if (att == "WEIGHTS_0")  return VxDataWeights0;
      throw UnsupportedExcept("Unsupported glTF primitive");
    };

    // Convert from primitives's mode to CG `Topology` value
    auto toTopology = [](int32_t mode) -> CG_NS::Topology {
      switch (mode) {
      case GLTF::Mesh::Primitive::Points:        return CG_NS::TopologyPoint;
      case GLTF::Mesh::Primitive::Lines:         return CG_NS::TopologyLine;
      case GLTF::Mesh::Primitive::Triangles:     return CG_NS::TopologyTriangle;
      case GLTF::Mesh::Primitive::LineStrip:     return CG_NS::TopologyLnStrip;
      case GLTF::Mesh::Primitive::TriangleStrip: return CG_NS::TopologyTriStrip;
      case GLTF::Mesh::Primitive::TriangleFan:   return CG_NS::TopologyTriFan;
      default: throw UnsupportedExcept("Unsupported glTF primitive");
      }
    };

    Mesh::Data data{};

    // Get vertex data from buffer and update data accessor
    auto getData = [&](const GLTF::Accessor& acc,
                       Mesh::Data::Accessor& accData) {

      const auto& view = gltf_.bufferViews()[acc.bufferView];
      auto& ifs = seekBufferView(acc.bufferView, acc.byteOffset);
      auto size = acc.sizeOfComponentType() * acc.sizeOfType();

      accData.dataIndex = data.data.size();
      accData.dataOffset = 0;
      accData.elementN = acc.count;
      accData.elementSize = size;

      size *= acc.count;
      data.data.push_back(make_unique<char[]>(size));
      auto dst = data.data.back().get();

      if (view.byteStride <= 0) {
        // Non-interleaved
        if (!ifs.read(dst, size))
          throw FileExcept("Could not read from glTF .glb/.bin file");
      } else {
        // Interleaved
        for (uint32_t i = 0; i < accData.elementN; i++) {
          if (!ifs.seekg(view.byteStride * i, ios_base::cur))
            throw FileExcept("Could not seek glTF .glb/.bin file");
          if (!ifs.read(dst, accData.elementSize))
            throw FileExcept("Could not read from glTF .glb/.bin file");
        }
      }
    };

    for (const auto& prim : gltf_.meshes()[mesh].primitives) {
      data.primitives.push_back({toTopology(prim.mode)});
      auto& primData = data.primitives.back();

      for (const auto& att : prim.attributes) {
        primData.accessors.push_back({toVxData(att.first)});
        getData(gltf_.accessors()[att.second], primData.accessors.back());
      }

      if (prim.indices >= 0) {
        primData.accessors.push_back({VxDataIndices});
        getData(gltf_.accessors()[prim.indices], primData.accessors.back());
      }

      if (prim.material >= 0) {
        const auto& material = loadMaterial(prim.material);
        primData.material = make_unique<Material>(material);
      }
    }

    collection_.meshes()[mesh] = make_unique<Mesh>(data);
    return *collection_.meshes()[mesh];
  }

  /// Loads a skin.
  ///
  Skin& loadSkin(int32_t skin) {
    assert(skin >= 0 && static_cast<size_t>(skin) < gltf_.skins().size());

    if (collection_.skins()[skin])
      return *collection_.skins()[skin];

    const auto& sk = gltf_.skins()[skin];
    vector<Mat4f> inverseBind{};

    if (sk.inverseBindMatrices >= 0) {
      const auto& acc = gltf_.accessors()[sk.inverseBindMatrices];
      const auto& view = gltf_.bufferViews()[acc.bufferView];
      if (acc.count == 0 || acc.componentType != GLTF::Accessor::Float ||
          acc.type != "MAT4" || view.byteStride > 0)
        throw UnsupportedExcept("Unsupported glTF skin");

      inverseBind.resize(acc.count);
      auto& ifs = seekAccessor(sk.inverseBindMatrices);

#if 0
      for (auto& m : inverseBind) {
        auto data = reinterpret_cast<char*>(m.data());
        if (!ifs.read(data, Mat4f::dataSize()))
          throw FileExcept("Could not read from glTF .glb/.bin file");
      }
#else
      static_assert(is_trivially_copyable<Mat4f>());
      static_assert(sizeof(Mat4f) == Mat4f::dataSize());

      auto data = reinterpret_cast<char*>(inverseBind.data());
      if (!ifs.read(data, Mat4f::dataSize() * acc.count))
        throw FileExcept("Could not read from glTF .glb/.bin file");
#endif
    }

    collection_.skins()[skin] = make_unique<Skin>(sk.joints.size(),
                                                  inverseBind);

    // XXX: Joint hierarchy NOT set
    uint32_t joint = 0;
    for (const auto& jt : sk.joints) {
      auto& node = static_cast<Joint&>(loadNode(jt));
      collection_.skins()[skin]->setJoint(node, joint++);
    }

    return *collection_.skins()[skin];
  }

  /// Loads an animation.
  ///
  Animation& loadAnimation(int32_t animation) {
    assert(animation >= 0 &&
           static_cast<size_t>(animation) < gltf_.animations().size());

    if (collection_.animations()[animation])
      return *collection_.animations()[animation];

    const auto& anim = gltf_.animations()[animation];

    // `GLTF::Accessor` to `Animation` input/output
    vector<pair<int32_t, size_t>> accMap;

    vector<Animation::Action> actions{};
    vector<Animation::Timeline> inputs{};
    vector<Animation::Translation> outT{};
    vector<Animation::Rotation> outR{};
    vector<Animation::Scale> outS{};

    for (const auto& ch : anim.channels) {
      actions.push_back({});
      auto& action = actions.back();

      // XXX: `loadNode()` calls this function
      action.target = &loadNode(ch.target.node);

      if (ch.target.path == "translation")
        action.type = Animation::T;
      else if (ch.target.path == "rotation")
        action.type = Animation::R;
      else if (ch.target.path == "scale")
        action.type = Animation::S;
      else
        throw UnsupportedExcept("Unsupported glTF animation");

      assert(ch.sampler >= 0 &&
             static_cast<size_t>(ch.sampler) < anim.samplers.size());

      const auto& sampler = anim.samplers[ch.sampler];

      if (sampler.interpolation == "STEP")
        action.method = Animation::Step;
      else if (sampler.interpolation == "LINEAR")
        action.method = Animation::Linear;
      else if (sampler.interpolation == "CUBICSPLINE")
        action.method = Animation::Cubic;
      else
        throw UnsupportedExcept("Unsupported glTF animation");

      // Input
      auto it = find_if(accMap.begin(), accMap.end(),
                        [&](auto& p) { return p.first == sampler.input; });

      if (it == accMap.end()) {
        const auto& acc = gltf_.accessors()[sampler.input];
        const auto& view = gltf_.bufferViews()[acc.bufferView];

        if (acc.count == 0 || acc.componentType != GLTF::Accessor::Float ||
            acc.type != "SCALAR" || view.byteStride > 0)
          throw UnsupportedExcept("Unsupported glTF animation");

        action.input = inputs.size();
        accMap.push_back(make_pair(sampler.input, action.input));
        inputs.push_back(Animation::Timeline(acc.count));
        auto data = reinterpret_cast<char*>(inputs.back().data());
        auto& ifs = seekAccessor(sampler.input);

        if (!ifs.read(data, sizeof(float) * acc.count))
          throw FileExcept("Could not read from .glb/.bin file");

      } else {
        action.input = it->second;
      }

      // Output
      it = find_if(accMap.begin(), accMap.end(),
                   [&](auto& p) { return p.first == sampler.output; });

      if (it == accMap.end()) {
        const auto& acc = gltf_.accessors()[sampler.output];
        const auto& view = gltf_.bufferViews()[acc.bufferView];

        if (acc.count == 0 || view.byteStride > 0)
          throw UnsupportedExcept("Unsupported glTF animation");

        char* data = nullptr;
        size_t size = 0;
        auto& ifs = seekAccessor(sampler.output);

        switch (action.type) {
        case Animation::T:
          if (acc.componentType != GLTF::Accessor::Float || acc.type != "VEC3")
            throw UnsupportedExcept("Unsupported glTF animation");
          action.output = outT.size();
          outT.push_back(Animation::Translation(acc.count));
          data = reinterpret_cast<char*>(outT.back().data());
          size = Vec3f::dataSize() * acc.count;
          if (!ifs.read(data, size))
            throw FileExcept("Could not read from .glb/.bin file");
          break;

        case Animation::R:
          if (acc.componentType != GLTF::Accessor::Float || acc.type != "VEC4")
            throw UnsupportedExcept("Unsupported glTF animation");
          action.output = outR.size();
          outR.push_back(Animation::Rotation{});
          {
            vector<Vec4f> tmp(acc.count);
            data = reinterpret_cast<char*>(tmp.data());
            size = Vec4f::dataSize() * acc.count;
            if (!ifs.read(data, size))
              throw FileExcept("Could not read from .glb/.bin file");
            auto& r = outR.back();
            for (const auto& v : tmp)
              r.push_back(Qnionf(v));
          }
          break;

        case Animation::S:
          if (acc.componentType != GLTF::Accessor::Float || acc.type != "VEC3")
            throw UnsupportedExcept("Unsupported glTF animation");
          action.output = outS.size();
          outS.push_back(Animation::Scale(acc.count));
          data = reinterpret_cast<char*>(outS.back().data());
          size = Vec3f::dataSize() * acc.count;
          if (!ifs.read(data, size))
            throw FileExcept("Could not read from .glb/.bin file");
          break;
        }

        accMap.push_back(make_pair(sampler.output, action.output));

      } else {
        action.output = it->second;
      }
    }

    auto& dst = collection_.animations()[animation];
    dst = make_unique<Animation>(inputs, outT, outR, outS);
    dst->actions() = actions;

    // XXX
    auto& name = dst->name();
    for (const auto& c : anim.name)
      name.push_back(c);

    return *dst;
  }

 private:
  const GLTF& gltf_;
  Collection collection_{};
  vector<ifstream> buffers_{};
  vector<Texture::Ptr> images_{};
  vector<bool> joints_{};

  /// Seeks into buffer as specified by a `GLTF::BufferView`.
  ///
  ifstream& seekBufferView(int32_t bufferView, uint64_t offset = 0) {
    assert(bufferView >= 0 &&
           static_cast<size_t>(bufferView) < gltf_.bufferViews().size());

    const auto& view = gltf_.bufferViews()[bufferView];
    const auto& buffer = gltf_.buffers()[view.buffer];
    ifstream* ifs;

    if (buffer.uri.empty()) {
      // Embedded (.glb)
      if (view.buffer != 0)
        throw UnsupportedExcept("Unsupported glTF buffer");

      ifs = &gltf_.bin();

    } else {
      // External (.bin)
      ifs = &buffers_[view.buffer];

      if (ifs->is_open()) {
        if (!ifs->seekg(0))
          throw FileExcept("Could not seek glTF .bin file");
      } else {
        const auto pathname = gltf_.directory() + '/' + buffer.uri;
        ifs->open(pathname);
        if (!ifs->is_open())
          throw FileExcept("Could not open glTF .bin file");
      }
    }

    offset += view.byteOffset;
    if (offset > 0 && !ifs->seekg(offset, ios_base::cur))
      throw FileExcept("Could not seek glTF .glb/.bin file");

    return *ifs;
  }

  /// Seeks into buffer as specified by a `GLTF::Accessor`.
  ///
  ifstream& seekAccessor(int32_t accessor) {
    assert(accessor >= 0 &&
           static_cast<size_t>(accessor) < gltf_.accessors().size());

    const auto& acc = gltf_.accessors()[accessor];
    return seekBufferView(acc.bufferView, acc.byteOffset);
  }

  /// Loads an image.
  ///
  Texture& loadImage(int32_t image) {
    assert(image >= 0 && static_cast<size_t>(image) < gltf_.images().size());

    if (images_[image])
      return *images_[image];

    const auto& img = gltf_.images()[image];
    if (img.uri.empty()) {
      // Image provided through a buffer view
      auto& ifs = seekBufferView(img.bufferView);
      images_[image] = make_unique<Texture>(ifs);
    } else {
      // Image provided through an URI
      // TODO: Base64
      const auto pathname = gltf_.directory() + '/' + img.uri;
      images_[image] = make_unique<Texture>(pathname);
    }

    return *images_[image];
  }
};

INTERNAL_NS_END

void SG_NS::loadGLTF(Collection& collection, const string& pathname) {
  GLTF gltf(pathname);

#ifdef YF_DEVEL
  printGLTF(gltf);
#endif

  // TODO
}

void SG_NS::loadGLTF(Collection& collection, ifstream& stream) {
  GLTF gltf(stream, "");

#ifdef YF_DEVEL
  printGLTF(gltf);
#endif

  // TODO
}

void SG_NS::loadGLTF(Mesh::Data& dst, const string& pathname, size_t index) {
  GLTF gltf(pathname);

#ifdef YF_DEVEL
  printGLTF(gltf);
#endif

  if (index >= gltf.meshes().size())
    throw invalid_argument("loadGLTF() index out of bounds");

  // TODO
}

void SG_NS::loadGLTF(Mesh::Data& dst, ifstream& stream, size_t index) {
  GLTF gltf(stream, "");

#ifdef YF_DEVEL
  printGLTF(gltf);
#endif

  if (index >= gltf.meshes().size())
    throw invalid_argument("loadGLTF() index out of bounds");

  // TODO
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
      for (size_t i = 0; i < 3; i++)
        wprintf(L"%.6f ", nd.transform[i]);
      wprintf(L"]");
      wprintf(L"\n    R: [ ");
      for (size_t i = 3; i < 7; i++)
        wprintf(L"%.6f ", nd.transform[i]);
      wprintf(L"]");
      wprintf(L"\n    S: [ ");
      for (size_t i = 7; i < 10; i++)
        wprintf(L"%.6f ", nd.transform[i]);
      wprintf(L"]");
    } else {
      for (size_t i = 0; i < nd.transform.size(); i++) {
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
