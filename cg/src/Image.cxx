//
// CG
// Image.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "yf/cg/Image.h"

using namespace CG_NS;

Image::Image(PxFormat format,
             Size2 size,
             uint32_t layers,
             uint32_t levels,
             Samples samples)
             :
             format_(format), size_(size), layers_(layers), levels_(levels),
             samples_(samples) { }

Image::~Image() { }
