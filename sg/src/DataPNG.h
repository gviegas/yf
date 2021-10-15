//
// SG
// DataPNG.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_DATAPNG_H
#define YF_SG_DATAPNG_H

#include <string>
#include <fstream>

#include "Texture.h"

SG_NS_BEGIN

/// Loads texture data from a PNG file.
///
void loadPNG(Texture::Data& dst, const std::string& pathname);
void loadPNG(Texture::Data& dst, std::ifstream& stream);

SG_NS_END

#endif // YF_SG_DATAPNG_H
