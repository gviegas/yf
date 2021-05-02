//
// SG
// DataGLTF.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_DATAGLTF_H
#define YF_SG_DATAGLTF_H

#include <string>
#include <cstddef>

#include "Scene.h"
#include "MeshImpl.h"

SG_NS_BEGIN

/// Loads scene graph from a glTF file.
///
void loadGLTF(Scene& dst, const std::wstring& pathname, size_t index);

/// Loads node subgraph from a glTF file.
///
void loadGLTF(Node& dst, const std::wstring& pathname, size_t index);

/// Loads mesh data from a glTF file.
///
void loadGLTF(Mesh::Data& dst, const std::wstring& pathname, size_t index);

SG_NS_END

#endif // YF_SG_DATAGLTF_H
