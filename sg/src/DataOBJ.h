//
// SG
// DataOBJ.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_DATAOBJ_H
#define YF_SG_DATAOBJ_H

#include "MeshImpl.h"

SG_NS_BEGIN

/// Loads mesh data from an OBJ file.
///
void loadOBJ(Mesh::Data& dst, const std::wstring& pathname);

SG_NS_END

#endif // YF_SG_DATAOBJ_H
