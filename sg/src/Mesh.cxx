//
// SG
// Mesh.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Mesh.h"

using namespace SG_NS;
using namespace std;

class Mesh::Impl {
 public:
  Impl(FileType fileType, const wstring& meshFile) {
    // TODO
  }

  ~Impl() {
    // TODO
  }

 private:
  // TODO
};

Mesh::Mesh(FileType fileType, const wstring& meshFile)
  : impl_(make_unique<Impl>(fileType, meshFile)) { }

Mesh::~Mesh() { }
