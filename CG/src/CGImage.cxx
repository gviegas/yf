//
// yf
// CGImage.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "CGImage.h"

using namespace YF_NS;

CGImage::CGImage(CGPxFormat format,
                 CGSize2 size,
                 uint32_t layers,
                 uint32_t levels,
                 CGSamples samples)
                 : format(format),
                   size(size),
                   layers(layers),
                   levels(levels),
                   samples(samples) {}

CGImage::~CGImage() {}
