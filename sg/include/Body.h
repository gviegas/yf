//
// SG
// Body.h
//
// Copyright © 2021 Gustavo C. Viegas.
//

#ifndef YF_SG_BODY_H
#define YF_SG_BODY_H

#include <memory>

#include "yf/sg/Defs.h"
#include "yf/sg/Vector.h"

SG_NS_BEGIN

/// Physics body.
///
class Body {
 public:
  Body(const Vec3f& offset, float radius);
  Body(const Vec3f& offset, const Vec3f& dimensions);
  Body(const Body& other);
  Body& operator=(const Body& other);
  ~Body();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

SG_NS_END

#endif // YF_SG_BODY_H
