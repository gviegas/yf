//
// CG
// DcTable.h
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#ifndef YF_CG_DCTABLE_H
#define YF_CG_DCTABLE_H

#include <cstdint>
#include <memory>
#include <vector>

#include "yf/cg/Defs.h"
#include "yf/cg/Buffer.h"
#include "yf/cg/Image.h"

CG_NS_BEGIN

using DcId = uint32_t;

/// Descriptor types.
///
enum DcType {
  DcTypeUniform,
  DcTypeStorage,
  DcTypeImage,
  DcTypeImgSampler
};

/// Descriptor table entry.
///
struct DcEntry {
  DcId id;
  DcType type;
  uint32_t elements;
};

/// Descriptor table.
///
class DcTable {
 public:
  using Ptr = std::unique_ptr<DcTable>;

  explicit DcTable(const std::vector<DcEntry>& entries);
  virtual ~DcTable();

  /// Allocates a given number of resources.
  ///
  virtual void allocate(uint32_t n) = 0;

  /// Retrives the number of allocations.
  ///
  virtual uint32_t allocations() const = 0;

  /// Writes to a table resource using a buffer object.
  ///
  virtual void write(uint32_t allocation, DcId id, uint32_t element,
                     Buffer& buffer, uint64_t offset, uint64_t size) = 0;

  /// Writes to a table resource using an image object.
  ///
  virtual void write(uint32_t allocation, DcId id, uint32_t element,
                     Image& image, uint32_t layer, uint32_t level) = 0;

  /// Writes to a table resource using an image object and sampler parameters.
  ///
  virtual void write(uint32_t allocation, DcId id, uint32_t element,
                     Image& image, uint32_t layer, uint32_t level,
                     Sampler& sampler) = 0;

  /// The table entries.
  ///
  const std::vector<DcEntry> entries_;
};

CG_NS_END

#endif // YF_CG_DCTABLE_H
