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

  vector<Vec3f> vs;
  vector<Vec2f> vts;
  vector<Vec3f> vns;

  string str;
  string sub[4];
  Vec3f v;
  Vec2f vt;
  Vec3f vn;
  uint32_t f[12];

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
    // TODO
  };

  istringstream iss;
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
}
