//
// yf
// CGImage.cxx
//
// Copyright (C) 2020 Gustavo C. Viegas.
//

#include "CGImage.h"

using namespace yf;

CGImage::CGImage(CGPxFormat format,
                 CGSize2 size,
                 uint32_t layers,
                 uint32_t levels,
                 CGSamples samples)
  : _format(format), _size(size), _layers(layers), _levels(levels),
    _samples(samples) {}

CGImage::~CGImage() {}
