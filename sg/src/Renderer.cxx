//
// SG
// Renderer.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include <cassert>
#include <algorithm>
#include <array>
#include <list>
#include <typeinfo>
#include <stdexcept>

#include "yf/cg/Device.h"

#include "Renderer.h"
#include "MeshImpl.h"
#include "TextureImpl.h"
#include "Skin.h"
#include "Camera.h"

using namespace SG_NS;
using namespace std;

/// Resource identifiers that shaders must abide by.
///
constexpr uint32_t GlobalTable = 0;
constexpr uint32_t ModelTable = 1;

constexpr CG_NS::DcId MainUniform = 0;
constexpr CG_NS::DcId CheckUniform = 1;
constexpr CG_NS::DcId MaterialUniform = 2;
constexpr CG_NS::DcId ColorImgSampler = 3;
constexpr CG_NS::DcId MetalRoughImgSampler = 4;
constexpr CG_NS::DcId NormalImgSampler = 5;
constexpr CG_NS::DcId OcclusionImgSampler = 6;
constexpr CG_NS::DcId EmissiveImgSampler = 7;

/// Shader pathnames.
///
using Shader = pair<CG_NS::Stage, const wchar_t*>;

constexpr wchar_t ShaderDir[] = L"bin/";

constexpr Shader MdlShaders[]{
  {CG_NS::StageVertex, L"Model.vert"}, {CG_NS::StageFragment, L"Model.frag"}};

constexpr Shader Mdl2Shaders[]{
  {CG_NS::StageVertex, L"Model2.vert"}, MdlShaders[1]};

constexpr Shader Mdl4Shaders[]{
  {CG_NS::StageVertex, L"Model4.vert"}, MdlShaders[1]};

constexpr Shader Mdl8Shaders[]{
  {CG_NS::StageVertex, L"Model8.vert"}, MdlShaders[1]};

constexpr Shader Mdl16Shaders[]{
  {CG_NS::StageVertex, L"Model16.vert"}, MdlShaders[1]};

constexpr Shader Mdl32Shaders[]{
  {CG_NS::StageVertex, L"Model32.vert"}, MdlShaders[1]};

// FIXME: Writes to descriptor table may have strict alignment requirements.
// These must be provided by CG and accounted for when defining the lengths.

/// Global uniform.
///
/// (1) view : Mat4f
/// (2) proj : Mat4f
/// (3) view-proj: Mat4f
///
constexpr uint64_t GlobalLength = Mat4f::dataSize() * 3;

/// Model (per-instance) uniform.
///
/// (1) model : Mat4f
/// (2) model-view : Mat4f
/// (3) model-view-proj : Mat4f
/// (4) normal matrix : Mat4f
/// (5) joint matrices : Mat4f[JointN]
/// (6) normal joint matrices : Mat4f[JointN]
///
constexpr uint64_t JointN = 64;
constexpr uint64_t SkinningLength = Mat4f::dataSize() * (JointN << 1);
constexpr uint64_t InstanceLength = (Mat4f::dataSize() << 2) + SkinningLength;

/// Check list uniform.
///
/// (1) mask : uint32
/// (*) _alignment_
///
constexpr uint64_t CheckAlign = 60;
constexpr uint64_t CheckLength = 4 + CheckAlign;

/// Material uniform.
///
/// (1) color fac : Vec4f
/// (2) metallic fac : float
/// (3) roughness fac : float
/// (4) normal fac : float
/// (5) occlusion fac : float
/// (6) emissive fac : Vec3f
/// (*) _alignment_
///
constexpr uint64_t MaterialAlign = 20;
constexpr uint64_t MaterialLength = Vec4f::dataSize() + 16 +
                                    Vec3f::dataSize() + MaterialAlign;

// TODO: consider allowing custom length values
constexpr uint64_t UniformLength = 1ULL << 20;

/// Check uniform flags.
///
enum CheckBits : uint32_t {
  TangentBit       = 0x0001,
  NormalBit        = 0x0002,
  TexCoord0Bit     = 0x0004,
  TexCoord1Bit     = 0x0008,
  Color0Bit        = 0x0010,
  SkinBit          = 0x0020,
  ColorTexBit      = 0x0100,
  MetalRoughTexBit = 0x0200,
  NormalTexBit     = 0x0400,
  OcclusionTexBit  = 0x0800,
  EmissiveTexBit   = 0x1000
};

Renderer::Renderer() {
  auto& dev = CG_NS::device();

  // One global table instance for shared uniforms
  const CG_NS::DcEntries glb{{MainUniform, {CG_NS::DcTypeUniform, 1}}};
  glbTable_ = dev.dcTable(glb);
  glbTable_->allocate(1);

  cmdBuffer_ = dev.defaultQueue().cmdBuffer();

  unifBuffer_ = dev.buffer(UniformLength);
}

void Renderer::render(Scene& scene, CG_NS::Target& target) {
  auto pass = &target.pass();

  if (pass != prevPass_) {
    resource_.reset();
    resource2_.reset();
    resource4_.reset();
    resource8_.reset();
    resource16_.reset();
    resource32_.reset();
  } else if (&scene == prevScene_) {
    // TODO
  }

  prevScene_ = &scene;
  prevPass_ = pass;

  processGraph(scene);
  prepare();

  // Encode common commands
  CG_NS::GrEncoder enc;
  enc.setTarget(&target);
  enc.setViewport({0.0f, 0.0f, static_cast<float>(target.size_.width),
                   static_cast<float>(target.size_.height), 0.0f, 1.0f});
  enc.setScissor({{0}, target.size_});
  enc.setDcTable(GlobalTable, 0);
  enc.clearColor(scene.color());
  enc.clearDepth(1.0f);

  // Update global uniform buffer
  uint64_t off = 0;
  uint64_t len;
  uint64_t beg;

  len = Mat4f::dataSize();
  unifBuffer_->write(off, len, scene.camera().view().data());
  off += len;
  unifBuffer_->write(off, len, scene.camera().projection().data());
  off += len;
  unifBuffer_->write(off, len, scene.camera().transform().data());
  off += len;
  // TODO: other global data

  glbTable_->write(0, MainUniform, 0, *unifBuffer_, 0, GlobalLength);

  // Render models
  auto renderMdl = [&] {
    // Resource info
    struct ResInfo {
      const Resource* const resource{};
      const uint32_t instN{};
      uint32_t allocN{};
    };

    list<ResInfo> resources;
    if (resource_.table)
      resources.push_back({&resource_, 1, resource_.table->allocations()});
    if (resource2_.table)
      resources.push_back({&resource2_, 2, resource2_.table->allocations()});
    if (resource4_.table)
      resources.push_back({&resource4_, 4, resource4_.table->allocations()});
    if (resource8_.table)
      resources.push_back({&resource8_, 8, resource8_.table->allocations()});
    if (resource16_.table)
      resources.push_back({&resource16_, 16, resource16_.table->allocations()});
    if (resource32_.table)
      resources.push_back({&resource32_, 32, resource32_.table->allocations()});

    vector<MdlKey> completed{};

    // Render until done or resources depleted
    while (!resources.empty() && !models_.empty()) {
      for (auto& kv : models_) {
        const auto size = kv.second.size();

        const Resource* resource{};
        uint32_t n{};
        uint32_t alloc{};

        // Find a resource that can render this many instances
        for (auto it = resources.begin(); it != resources.end(); it++) {
          if (it->instN < size)
            continue;
          resource = it->resource;
          n = size;
          alloc = --it->allocN;
          if (it->allocN == 0)
            resources.erase(it);
          break;
        }

        // If no suitable resource is found, get one to render a subset
        if (!resource) {
          if (resources.empty())
            break;
          auto& r = resources.back();
          resource = r.resource;
          n = r.instN;
          alloc = --r.allocN;
          if (r.allocN == 0)
            resources.pop_back();
        }

        enc.setState(resource->state.get());
        enc.setDcTable(ModelTable, alloc);

        auto mesh = kv.second[0]->mesh();
        if (!mesh)
          throw runtime_error("Cannot render models with no mesh set");

        auto matl = kv.second[0]->material();
        if (!matl)
          // TODO: consider using default material instead
          throw runtime_error("Cannot render models with no material set");

        // Update instance-specific uniform buffer
        for (uint32_t i = 0; i < n; i++) {
          auto mdl = kv.second.back();
          kv.second.pop_back();

          beg = off;
          len = Mat4f::dataSize();

          const auto& m = mdl->worldTransform();
          unifBuffer_->write(off, len, m.data());
          off += len;

          const auto mv = scene.camera().view() * m;
          unifBuffer_->write(off, len, mv.data());
          off += len;

          const auto mvp = scene.camera().projection() * mv;
          unifBuffer_->write(off, len, mvp.data());
          off += len;

          const auto& nm = mdl->worldNormal();
          unifBuffer_->write(off, len, nm.data());
          off += len;

          auto skin = mdl->skin();
          array<Mat4f, (JointN << 1)> skinning;
          skinning.fill(Mat4f::identity());
          if (skin) {
            assert(skin->joints().size() <= JointN);
            size_t i = 0;
            for (const auto& jt : skin->joints()) {
              assert(jt); // XXX
              skinning[i] = jt->worldTransform() * skin->inverseBind()[i];
              skinning[i+JointN] = transpose(invert(skinning[i]));
              i++;
            }
          }
          len = SkinningLength;
          unifBuffer_->write(off, len, skinning.data());
          off += len;

          resource->table->write(alloc, MainUniform, i, *unifBuffer_, beg,
                                 InstanceLength);
        }

        // Update material
        pair<Texture*, CG_NS::DcId> texs[]{
          {matl->pbrmr().colorTex, ColorImgSampler},
          {matl->pbrmr().metalRoughTex, MetalRoughImgSampler},
          {matl->normal().texture, NormalImgSampler},
          {matl->occlusion().texture, OcclusionImgSampler},
          {matl->emissive().texture, EmissiveImgSampler}};

        for (auto& tp : texs) {
          if (tp.first)
            tp.first->impl().copy(*resource->table, alloc, tp.second,
                                  0, 0, nullptr);
        }

        beg = off;
        len = Vec4f::dataSize();
        unifBuffer_->write(off, len, matl->pbrmr().colorFac.data());
        off += len;
        len = 4;
        unifBuffer_->write(off, len, &matl->pbrmr().metallic);
        off += len;
        unifBuffer_->write(off, len, &matl->pbrmr().roughness);
        off += len;
        unifBuffer_->write(off, len, &matl->normal().scale);
        off += len;
        unifBuffer_->write(off, len, &matl->occlusion().strength);
        off += len;
        len = Vec3f::dataSize();
        unifBuffer_->write(off, len, matl->emissive().factor.data());
        off += len;
        off += MaterialAlign;

        resource->table->write(alloc, MaterialUniform, 0, *unifBuffer_, beg,
                               MaterialLength);

        // Update check list
        uint32_t chkMask = 0;

        if (mesh->impl().canBind(VxTypeTangent))
          chkMask |= TangentBit;
        if (mesh->impl().canBind(VxTypeNormal))
          chkMask |= NormalBit;
        if (mesh->impl().canBind(VxTypeTexCoord0))
          chkMask |= TexCoord0Bit;
        if (mesh->impl().canBind(VxTypeTexCoord1))
          chkMask |= TexCoord1Bit;
        if (mesh->impl().canBind(VxTypeColor0))
          chkMask |= Color0Bit;
        if (mesh->impl().canBind(VxTypeJoints0) &&
            mesh->impl().canBind(VxTypeWeights0))
          chkMask |= SkinBit;
        if (matl->pbrmr().colorTex)
          chkMask |= ColorTexBit;
        if (matl->pbrmr().metalRoughTex)
          chkMask |= MetalRoughTexBit;
        if (matl->normal().texture)
          chkMask |= NormalTexBit;
        if (matl->occlusion().texture)
          chkMask |= OcclusionTexBit;
        if (matl->emissive().texture)
          chkMask |= EmissiveTexBit;

        beg = off;
        len = 4;
        unifBuffer_->write(off, len, &chkMask);
        off += len;
        off += CheckAlign;

        resource->table->write(alloc, CheckUniform, 0, *unifBuffer_, beg,
                               CheckLength);

        // Encode commands for this mesh
        mesh->impl().encode(enc, 0, n);

        if (kv.second.empty())
          completed.push_back(kv.first);
      }

      for (const auto& k : completed)
        models_.erase(k);
      completed.clear();
    }
  };

  // Render & submit
  do {
    off = GlobalLength;
    renderMdl();

    cmdBuffer_->encode(enc);
    cmdBuffer_->enqueue();
    cmdBuffer_->queue().submit();
  } while (!models_.empty());

#if defined(YF_DEVEL) && defined(YF_DEVEL_PRINT)
  print();
#endif
}

void Renderer::processGraph(Scene& scene) {
  models_.clear();

  if (scene.isLeaf())
    return;

  scene.worldTransform() = scene.transform();
  scene.worldInverse() = invert(scene.worldTransform());

  scene.traverse([&](Node& node) {
    // Transforms
    node.worldTransform() = node.parent()->worldTransform() * node.transform();
    node.worldInverse() = invert(node.worldTransform());
    node.worldNormal() = transpose(node.worldInverse());

    // Model
    if (typeid(node) == typeid(Model)) {
      auto& mdl = static_cast<Model&>(node);
      const MdlKey key{mdl.mesh(), mdl.material()};

      auto it = models_.find(key);
      if (it == models_.end())
        models_.emplace(key, MdlValue{&mdl});
      else
        it->second.push_back(&mdl);
    }
  }, true);
}

void Renderer::prepare() {
  auto& dev = CG_NS::device();

  // Set model resources and returns required uniform space
  auto setMdl = [&](Resource& resource, uint32_t instN, uint32_t allocN) {
    assert(instN > 0);
    assert(allocN > 0);

    // Shaders
    if (resource.shaders.empty()) {
      switch (instN) {
      case 1:
        for (const auto& tp : MdlShaders)
          resource.shaders.push_back(dev.shader(tp.first,
                                                wstring(ShaderDir)+tp.second));
        break;
      case 2:
        for (const auto& tp : Mdl2Shaders)
          resource.shaders.push_back(dev.shader(tp.first,
                                                wstring(ShaderDir)+tp.second));
        break;
      case 4:
        for (const auto& tp : Mdl4Shaders)
          resource.shaders.push_back(dev.shader(tp.first,
                                                wstring(ShaderDir)+tp.second));
        break;
      case 8:
        for (const auto& tp : Mdl8Shaders)
          resource.shaders.push_back(dev.shader(tp.first,
                                                wstring(ShaderDir)+tp.second));
        break;
      case 16:
        for (const auto& tp : Mdl16Shaders)
          resource.shaders.push_back(dev.shader(tp.first,
                                                wstring(ShaderDir)+tp.second));
        break;
      case 32:
        for (const auto& tp : Mdl32Shaders)
          resource.shaders.push_back(dev.shader(tp.first,
                                                wstring(ShaderDir)+tp.second));
        break;
      default:
        assert(false);
        abort();
      }
    }

    // Descriptors
    if (!resource.table) {
      const CG_NS::DcEntries inst{
        {MainUniform,          {CG_NS::DcTypeUniform,    instN}},
        {CheckUniform,         {CG_NS::DcTypeUniform,    1}},
        {MaterialUniform,      {CG_NS::DcTypeUniform,    1}},
        {ColorImgSampler,      {CG_NS::DcTypeImgSampler, 1}},
        {MetalRoughImgSampler, {CG_NS::DcTypeImgSampler, 1}},
        {NormalImgSampler,     {CG_NS::DcTypeImgSampler, 1}},
        {OcclusionImgSampler,  {CG_NS::DcTypeImgSampler, 1}},
        {EmissiveImgSampler,   {CG_NS::DcTypeImgSampler, 1}}};
      resource.table = dev.dcTable(inst);
    }

    if (resource.table->allocations() != allocN)
      resource.table->allocate(allocN);

    // State
    if (!resource.state) {
      vector<CG_NS::Shader*> shd;
      for (const auto& s : resource.shaders)
        shd.push_back(s.get());

      const vector<CG_NS::DcTable*> tab{glbTable_.get(), resource.table.get()};

      const vector<CG_NS::VxInput> inp{vxInputFor(VxTypePosition),
                                       vxInputFor(VxTypeTangent),
                                       vxInputFor(VxTypeNormal),
                                       vxInputFor(VxTypeTexCoord0),
                                       vxInputFor(VxTypeTexCoord1),
                                       vxInputFor(VxTypeColor0),
                                       vxInputFor(VxTypeJoints0),
                                       vxInputFor(VxTypeWeights0)};

      resource.state = dev.state({prevPass_, shd, tab, inp,
                                  CG_NS::PrimitiveTriangle,
                                  CG_NS::PolyModeFill, CG_NS::CullModeBack,
                                  CG_NS::WindingCounterCw});
    }

    const auto instLen = InstanceLength * instN;
    const auto sharLen = CheckLength + MaterialLength;
    return (instLen + sharLen) * allocN;
  };

  uint64_t unifLen = GlobalLength;

  // Set models
  // TODO: check limits and catch errors
  uint32_t mdlN = 0;
  uint32_t mdl2N = 0;
  uint32_t mdl4N = 0;
  uint32_t mdl8N = 0;
  uint32_t mdl16N = 0;
  uint32_t mdl32N = 0;

  for (const auto& kv : models_) {
    uint32_t size = kv.second.size();
    while (size >= 32) {
      mdl32N++;
      size -= 32;
    }
    if (size >= 16) {
      mdl16N++;
      size -= 16;
    }
    if (size >= 8) {
      mdl8N++;
      size -= 8;
    }
    if (size >= 4) {
      mdl4N++;
      size -= 4;
    }
    if (size >= 2) {
      mdl2N++;
      size -= 2;
    }
    if (size == 1)
      mdlN++;
  }

  if (mdlN > 0)
    unifLen += setMdl(resource_, 1, mdlN);
  else
    resource_.reset();
  if (mdl2N > 0)
    unifLen += setMdl(resource2_, 2, mdl2N);
  else
    resource2_.reset();
  if (mdl4N > 0)
    unifLen += setMdl(resource4_, 4, mdl4N);
  else
    resource4_.reset();
  if (mdl8N > 0)
    unifLen += setMdl(resource8_, 8, mdl8N);
  else
    resource8_.reset();
  if (mdl16N > 0)
    unifLen += setMdl(resource16_, 16, mdl16N);
  else
    resource16_.reset();
  if (mdl32N > 0)
    unifLen += setMdl(resource32_, 32, mdl32N);
  else
    resource32_.reset();

  unifLen = (unifLen & ~255) + 256;

  // TODO: improve resizing
  // TODO: also consider shrinking if buffer grows too much
  if (unifLen > unifBuffer_->size_)
    unifBuffer_ = dev.buffer(unifLen);
}

//
// DEVEL
//

#ifdef YF_DEVEL

void Renderer::print() const {
  wprintf(L"\nRenderer");
  const vector<pair<string, const Resource*>> resources{{"1", &resource_},
                                                        {"2", &resource2_},
                                                        {"4", &resource4_},
                                                        {"8", &resource8_},
                                                        {"16", &resource16_},
                                                        {"32", &resource32_}};
  for (const auto& r : resources) {
    wprintf(L"\n resource <%s>:\t", r.first.data());
    if (r.second->table)
      wprintf(L" %u allocation(s)", r.second->table->allocations());
    else
      wprintf(L" unused");
  }
  wprintf(L"\n");
}

#endif
