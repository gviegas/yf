//
// SG
// DataOBJ.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <cwchar>
#include <cstring>
#include <memory>
#include <sstream>
#include <fstream>

#include "DataOBJ.h"
#include "Vector.h"
#include "yf/Except.h"

using namespace SG_NS;
using namespace std;

void SG_NS::loadOBJ(Mesh::Data& dst, const wstring& pathname) {
  // Convert pathname string
  string path{};
  size_t len = (pathname.size() + 1) * sizeof(wchar_t);
  path.resize(len);
  const wchar_t* wsrc = pathname.data();
  mbstate_t state;
  memset(&state, 0, sizeof state);
  len = wcsrtombs(path.data(), &wsrc, path.size(), &state);
  if (wsrc || static_cast<size_t>(-1) == len)
    throw ConversionExcept("Could not convert OBJ file path");
  path.resize(len);

  ifstream ifs(path);
  if (!ifs)
    throw FileExcept("Could not open OBJ file");

  vector<Vec3f> positions;
  vector<Vec2f> texCoords;
  vector<Vec3f> normals;
  vector<uint32_t> indices;
  unordered_map<string, uint32_t> map;

  string str;
  string sub[4];
  istringstream iss;

  Vec3f v;
  vector<Vec3f> vs;
  Vec2f vt;
  vector<Vec2f> vts;
  Vec3f vn;
  vector<Vec3f> vns;

  // Face format flags
  enum : uint32_t {
    FNone = 0,
    FPos  = 0x01,
    FTc   = 0x02,
    FNorm = 0x04,
    FQuad = 0x10
  };
  uint32_t format = FNone;

  // Process a face from `sub` strings as indicated by `format`
  auto processFace = [&] {
    const uint32_t n = format & FQuad ? 4 : 3;
    uint32_t face[4]{0};
    uint32_t i1, i2, i3;
    char sep;
    string key;

    // Fill attribute and index vectors with mesh data & default-initialize
    // any missing vertex attribute
    switch (format) {
    case FPos|FTc|FNorm:
    case FPos|FTc|FNorm|FQuad:
      for (uint32_t i = 0; i < n; ++i) {
        iss.clear();
        iss.str(sub[i]);
        iss >> i1 >> sep >> i2 >> sep >> i3;

        key = to_string(i1) + to_string(i2) + to_string(i3);
        auto it = map.find(key);

        if (it == map.end()) {
          face[i] = positions.size();
          map.emplace(key, face[i]);
          positions.push_back(vs[i1-1]);
          texCoords.push_back(vts[i2-1]);
          normals.push_back(vns[i3-1]);
        } else {
          face[i] = it->second;
        }
      }

      indices.push_back(face[0]);
      indices.push_back(face[1]);
      indices.push_back(face[2]);
      if (format & FQuad) {
        indices.push_back(face[0]);
        indices.push_back(face[2]);
        indices.push_back(face[3]);
      }
      break;

    case FPos|FTc:
    case FPos|FTc|FQuad:
      for (uint32_t i = 0; i < n; ++i) {
        iss.clear();
        iss.str(sub[i]);
        iss >> i1 >> sep >> i2;

        key = to_string(i1) + to_string(i2);
        auto it = map.find(key);

        if (it == map.end()) {
          face[i] = positions.size();
          map.emplace(key, face[i]);
          positions.push_back(vs[i1-1]);
          texCoords.push_back(vts[i2-1]);
          normals.push_back({});
        } else {
          face[i] = it->second;
        }
      }

      indices.push_back(face[0]);
      indices.push_back(face[1]);
      indices.push_back(face[2]);
      if (format & FQuad) {
        indices.push_back(face[0]);
        indices.push_back(face[2]);
        indices.push_back(face[3]);
      }
      break;

    case FPos|FNorm:
    case FPos|FNorm|FQuad:
      for (uint32_t i = 0; i < n; ++i) {
        iss.clear();
        iss.str(sub[i]);
        iss >> i1 >> sep >> sep >> i2;

        key = to_string(i1) + to_string(i2);
        auto it = map.find(key);

        if (it == map.end()) {
          face[i] = positions.size();
          map.emplace(key, face[i]);
          positions.push_back(vs[i1-1]);
          texCoords.push_back({});
          normals.push_back(vns[i2-1]);
        } else {
          face[i] = it->second;
        }
      }

      indices.push_back(face[0]);
      indices.push_back(face[1]);
      indices.push_back(face[2]);
      if (format & FQuad) {
        indices.push_back(face[0]);
        indices.push_back(face[2]);
        indices.push_back(face[3]);
      }
      break;

    case FPos:
    case FPos|FQuad:
      for (uint32_t i = 0; i < n; ++i) {
        iss.clear();
        iss.str(sub[i]);
        iss >> i1;

        key = to_string(i1);
        auto it = map.find(key);

        if (it == map.end()) {
          face[i] = positions.size();
          map.emplace(key, face[i]);
          positions.push_back(vs[i1-1]);
          texCoords.push_back({});
          normals.push_back({});
        } else {
          face[i] = it->second;
        }
      }

      indices.push_back(face[0]);
      indices.push_back(face[1]);
      indices.push_back(face[2]);
      if (format & FQuad) {
        indices.push_back(face[0]);
        indices.push_back(face[2]);
        indices.push_back(face[3]);
      }
      break;

    default:
      throw runtime_error("Invalid OBJ file");
    }
  };

  string line;
  line.resize(256);

  while (ifs.getline(line.data(), line.size())) {
    iss.str(line.substr(0, ifs.gcount()));
    iss >> str;

    if (str == "v") {
      // position
      iss >> v[0] >> v[1] >> v[2];
      vs.push_back({v[0], v[1], v[2]});

    } else if (str == "vt") {
      // texture coordinate
      iss >> vt[0] >> vt[1];
      vts.push_back({vt[0], vt[1]});

    } else if (str == "vn") {
      // normal
      iss >> vn[0] >> vn[1] >> vn[2];
      vns.push_back({vn[0], vn[1], vn[2]});

    } else if (str == "f") {
      // face indices
      iss >> sub[0] >> sub[1] >> sub[2];

      // Scan once to find out the face format
      if (format == FNone) {
        uint32_t i = 0;
        format |= FPos;
        while (++i < sub[0].size()) {
          if (sub[0][i] == '/') {
            ++i;
            if (i == sub[0].size())
              throw FileExcept("Invalid OBJ file");
            if (sub[0][i] == '/') {
              format |= FNorm;
            } else {
              format |= FTc;
              while (++i < sub[0].size()) {
                if (sub[0][i] == '/') {
                  format |= FNorm;
                  break;
                }
              }
            }
            break;
          }
        }
      }

      // Is this a quad?
      if (!iss.eof()) {
        //XXX: assuming no extraneous white space at eol
        iss >> sub[3];
        if (!iss.eof())
          throw FileExcept("Invalid OBJ file");
        format |= FQuad;
      }

      processFace();
      iss.clear();

    } else {
      // something else
      continue;
    }
  }

  if (positions.empty() || indices.size() < positions.size())
    throw FileExcept("Invalid OBJ file");

  // Copy vertex data
  const uint32_t posCount = positions.size();
  const uint64_t posSize = posCount * sizeof positions[0];
  const uint32_t tcCount = texCoords.size();
  const uint64_t tcSize =  tcCount * sizeof texCoords[0];
  const uint32_t normCount = normals.size();
  const uint64_t normSize = normCount * sizeof normals[0];
  const uint64_t vxSize = posSize + tcSize + normSize;

  dst.data.push_back(make_unique<uint8_t[]>(vxSize));
  memcpy(dst.data.back().get(), positions.data(), posSize);
  memcpy(dst.data.back().get()+posSize, texCoords.data(), tcSize);
  memcpy(dst.data.back().get()+posSize+tcSize, normals.data(), normSize);

  dst.vxAccessors.emplace(VxTypePosition,
                          Mesh::Data::Accessor{0, 0, posCount,
                                               sizeof positions[0]});
  dst.vxAccessors.emplace(VxTypeTexCoord0,
                          Mesh::Data::Accessor{0, posSize, tcCount,
                                               sizeof texCoords[0]});
  dst.vxAccessors.emplace(VxTypeNormal,
                          Mesh::Data::Accessor{0, posSize+tcSize, normCount,
                                               sizeof normals[0]});

  // Copy index data
  const uint32_t ixCount = indices.size();

  if (ixCount < UINT16_MAX) {
    // 16-bit indices
    const uint64_t ixSize = ixCount * sizeof(uint16_t);
    dst.data.push_back(make_unique<uint8_t[]>(ixSize));
    for (size_t i = 0; i < ixCount; ++i)
      reinterpret_cast<uint16_t*>(dst.data.back().get())[i] = indices[i];
    dst.ixAccessor.elementSize = sizeof(uint16_t);

  } else {
    // 32-bit indices
    const uint64_t ixSize = ixCount * sizeof(uint32_t);
    dst.data.push_back(make_unique<uint8_t[]>(ixSize));
    memcpy(dst.data.back().get(), indices.data(), ixSize);
    dst.ixAccessor.elementSize = sizeof(uint32_t);
  }

  dst.ixAccessor.dataIndex = 1;
  dst.ixAccessor.dataOffset = 0;
  dst.ixAccessor.elementN = ixCount;
}
