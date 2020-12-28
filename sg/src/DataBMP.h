//
// SG
// DataBMP.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_SG_DATABMP_H
#define YF_SG_DATABMP_H

#include "TextureImpl.h"

SG_NS_BEGIN

/// Loads texture data from a BMP file.
///
void loadBMP(Texture::Data& dst, const std::wstring& pathname);

SG_NS_END

#endif // YF_SG_DATABMP_H
