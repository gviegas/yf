//
// SG
// Mesh.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/Except.h"

#include "yf/cg/Encoder.h"

#include "MeshImpl.h"

using namespace SG_NS;
using namespace std;

// TODO
Mesh::Mesh(FileType fileType, const wstring& meshFile) { }
Mesh::~Mesh() { }

Mesh::Mesh(const Data& data) : impl_(make_unique<Impl>(data)) { }

Mesh::Impl& Mesh::impl() {
  return *impl_;
}

// TODO
Mesh::Impl::Impl(const Data& data) { }
Mesh::Impl::~Impl() { }

void Mesh::Impl::updateVertices(void* data, uint64_t offset, uint64_t size) {
  // TODO
  throw runtime_error("Unimplemented");
}

void Mesh::Impl::updateIndices(void* data, uint32_t indexStart,
                               uint32_t indexCount) {
  // TODO
  throw runtime_error("Unimplemented");
}

void Mesh::Impl::encodeBindings(GrEncoder& encoder, uint32_t inputIndex) {
  // TODO
  throw runtime_error("Unimplemented");
}

void Mesh::Impl::encodeDraw(GrEncoder& encoder, uint32_t baseInstance,
                            uint32_t instanceCount) {
  // TODO
  throw runtime_error("Unimplemented");
}
