//
// yf
// CGDcTable.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "CGDcTable.h"

using namespace YF_NS;

CGDcTable::CGDcTable(const CGDcEntries& entries) : entries(entries) {}

CGDcTable::CGDcTable(CGDcEntries&& entries) : entries(entries) {}

CGDcTable::~CGDcTable() {}
