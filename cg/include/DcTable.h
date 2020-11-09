//
// CG
// DcTable.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_CG_DCTABLE_H
#define YF_CG_DCTABLE_H

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "yf/cg/Defs.h"

CG_NS_BEGIN

/// Descriptor types.
///
enum DcType {
  DcTypeUniform,
  DcTypeStorage,
  DcTypeImage,
  DcTypeSampler,
  DcTypeSampledImg,
  DcTypeImgSampler
};

/// Descriptor table entry.
///
struct DcEntry {
  DcType type;
  uint32_t elements;
};

using DcId = uint32_t;
using DcEntries = std::unordered_map<DcId, DcEntry>;

class Buffer;
class Image;

/// Descriptor table.
///
class DcTable {
 public:
  using Ptr = std::unique_ptr<DcTable>;
  explicit DcTable(const DcEntries& entries);
  explicit DcTable(DcEntries&& entries);
  virtual ~DcTable();

  /// Allocates a given number of resources.
  ///
  virtual void allocate(uint32_t n) = 0;

  /// Retrives the number of allocations.
  ///
  virtual uint32_t allocations() = 0;

  /// Writes to a table resource using a buffer object.
  ///
  virtual void write(uint32_t allocation,
                     DcId id,
                     uint32_t element,
                     Buffer& buffer,
                     uint64_t offset,
                     uint64_t size) = 0;

  /// Writes to a table resource using an image object.
  ///
  virtual void write(uint32_t allocation,
                     DcId id,
                     uint32_t element,
                     Image& image,
                     uint32_t layer) = 0;

  /// The table entries.
  ///
  const DcEntries entries;
};

CG_NS_END

#endif // YF_CG_DCTABLE_H
