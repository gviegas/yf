//
// SG
// NewRenderer.cxx
//
// Copyright © 2021 Gustavo C. Viegas.
//

#include "yf/cg/Device.h"

#include "NewRenderer.h"
#include "Node.h"
#include "Scene.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"

using namespace SG_NS;
using namespace std;

constexpr uint64_t UnifBufferSize = 1 << 21;

constexpr CG_NS::DcEntry GlobalUnif{0, CG_NS::DcTypeUniform, 1};

NewRenderer::NewRenderer() {
  auto& dev = CG_NS::device();

  cmdBuffer_ = dev.defaultQueue().cmdBuffer();
  unifBuffer_ = dev.buffer(UnifBufferSize);

  // This table will contain data common to all drawables
  mainTable_ = dev.dcTable({GlobalUnif});
  mainTable_->allocate(1);
}

void NewRenderer::processGraph(Node& rootNode) { /* TODO */ }
