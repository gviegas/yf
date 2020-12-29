//
// SG
// DataOBJ.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <cwchar>
#include <cstring>
#include <sstream>
#include <fstream>
#include <memory>

#include "yf/Except.h"

#include "DataOBJ.h"
#include "Vector.h"

using namespace SG_NS;
using namespace std;

void SG_NS::loadOBJ(Mesh::Data& dst, const wstring& pathname) {
  // Convert pathname string
  char mpath[256];
  const wchar_t* wsrc= pathname.data();
  mbstate_t state;
  memset(&state, 0, sizeof state);
  wcsrtombs(mpath, &wsrc, sizeof mpath, &state);
  if (wsrc)
    throw LimitExcept("Could not convert OBJ file path");

  ifstream ifs(mpath);
  if (!ifs)
    throw FileExcept("Could not open OBJ file");

  // TODO: define this type somewhere else
  struct Vertex {
    Vec3f pos;
    Vec2f tc;
    Vec3f norm;
  };
  static_assert(is_standard_layout<Vertex>(), "Bad vertex data layout");

  vector<Vertex> vertices;
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

    // Fill `vertices` and `indices` with mesh data & default-initialize
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
          face[i] = vertices.size();
          map.emplace(key, face[i]);
          vertices.push_back({vs[i1-1], vts[i2-1], vns[i3-1]});
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
          face[i] = vertices.size();
          map.emplace(key, face[i]);
          vertices.push_back({vs[i1-1], vts[i2-1], {}});
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
          face[i] = vertices.size();
          map.emplace(key, face[i]);
          vertices.push_back({vs[i1-1], {}, vns[i2-1]});
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
          face[i] = vertices.size();
          map.emplace(key, face[i]);
          vertices.push_back({vs[i1-1], {}, {}});
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

  // TODO...

  // ----------------------------------------------------------------------
  // XXX
  wprintf(L"#vs#\n");
  for (const auto& x : vs) {
    for (const auto& y : x)
      wprintf(L"%.3f ", y);
    wprintf(L"\n");
  }
  wprintf(L"#vts#\n");
  for (const auto& x : vts) {
    for (const auto& y : x)
      wprintf(L"%.3f ", y);
    wprintf(L"\n");
  }
  wprintf(L"#vns#\n");
  for (const auto& x : vns) {
    for (const auto& y : x)
      wprintf(L"%.3f ", y);
    wprintf(L"\n");
  }

  wprintf(L"#vertices#\n");
  for (const auto& x : vertices) {
    for (const auto& y : x.pos)
      wprintf(L"%.3f ", y);
    wprintf(L", ");
    for (const auto& y : x.tc)
      wprintf(L"%.3f ", y);
    wprintf(L", ");
    for (const auto& y : x.norm)
      wprintf(L"%.3f ", y);
    wprintf(L"\n");
  }

  wprintf(L"#indices#\n");
  for (const auto& i : indices)
    wprintf(L"%u ", i);
  wprintf(L"\n");

  wprintf(L"#map#\n");
  for (const auto& kv : map)
    wprintf(L"%s,%u ", kv.first.data(), kv.second);
  wprintf(L"\n");

  exit(1);
  // ----------------------------------------------------------------------
}
