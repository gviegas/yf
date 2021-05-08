//
// SG
// DataGLTF.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_DATAGLTF_H
#define YF_SG_DATAGLTF_H

#include <cstddef>
#include <string>

#include "Collection.h"
#include "MeshImpl.h"

SG_NS_BEGIN

/// Loads contents from a glTF file.
///
void loadGLTF(Collection& collection, const std::wstring& pathname);

/// Loads mesh data from a glTF file.
///
void loadGLTF(Mesh::Data& dst, const std::wstring& pathname, size_t index);

SG_NS_END

#endif // YF_SG_DATAGLTF_H
