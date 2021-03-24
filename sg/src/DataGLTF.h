//
// SG
// DataGLTF.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_DATAGLTF_H
#define YF_SG_DATAGLTF_H

#include <string>
#include <cstdint>

#include "Node.h"
#include "Model.h"
#include "MeshImpl.h"

SG_NS_BEGIN

/// Loads node graph from a glTF file.
///
void loadGLTF(Node& dst, const std::wstring& pathname, uint32_t index);

/// Loads model object from a glTF file.
///
void loadGLTF(Model& dst, const std::wstring& pathname, uint32_t index);

/// Loads mesh data from a glTF file.
///
void loadGLTF(Mesh::Data& dst, const std::wstring& pathname, uint32_t index);

SG_NS_END

#endif // YF_SG_DATAGLTF_H
