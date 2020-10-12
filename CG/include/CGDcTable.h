//
// yf
// CGDcTable.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_DCTABLE_H
#define YF_CG_DCTABLE_H

#include <cstdint>
#include <unordered_map>

#include "YFDefs.h"
#include "CGResult.h"

YF_NS_BEGIN

enum CGDcType {
  CGDcTypeUniform,
  CGDcTypeStorage,
  CGDcTypeImage,
  CGDcTypeSampler,
  CGDcTypeSampledImg,
  CGDcTypeImgSampler
};

struct CGDcEntry {
  CGDcType type;
  uint32_t elements;
};

using CGDcId = uint32_t;
using CGDcEntries = std::unordered_map<CGDcId, CGDcEntry>;

class CGBuffer;
class CGImage;

class CGDcTable {
 public:
  explicit CGDcTable(const CGDcEntries& entries);
  explicit CGDcTable(CGDcEntries&& entries);
  virtual ~CGDcTable();

  /// Allocates a given number of resources.
  ///
  virtual CGResult allocate(uint32_t n) = 0;

  /// Retrives the number of allocations.
  ///
  virtual uint32_t allocations() = 0;

  /// Writes to a table resource using a buffer object.
  ///
  virtual CGResult write(uint32_t allocation,
                         CGDcId id,
                         uint32_t element,
                         CGBuffer& buffer,
                         uint64_t offset,
                         uint64_t size) = 0;

  /// Writes to a table resource using an image object.
  ///
  virtual CGResult write(uint32_t allocation,
                         CGDcId id,
                         uint32_t element,
                         CGImage& image,
                         uint32_t layer) = 0;

  /// The table entries.
  ///
  const CGDcEntries entries;
};

YF_NS_END

#endif // YF_CG_DCTABLE_H
