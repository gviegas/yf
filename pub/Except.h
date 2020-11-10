//
// YF
// Except.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_EXCEPT_H
#define YF_EXCEPT_H

#include <stdexcept>

#include "yf/Defs.h"

YF_NS_BEGIN

struct NoMemoryExcept : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct LimitExcept : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct NotReadyExcept : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct UnsupportedExcept : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct DeviceExcept : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct FileExcept : std::runtime_error {
  using std::runtime_error::runtime_error;
};

YF_NS_END

#endif // YF_EXCEPT_H
