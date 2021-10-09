//
// SG
// MeshTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"
#include "MeshImpl.h"

using namespace SG_NS;
using namespace std;

TEST_NS_BEGIN

struct MeshTest : Test {
  MeshTest() : Test(L"Mesh") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto print = [](Mesh* mesh = nullptr) {
      if (!mesh) {
        wcout << "\nSegments: " << Primitive::Impl::segments_.size() << "\n";
        for (const auto& s : Primitive::Impl::segments_)
          wcout << " [" << s.offset << ":" << s.size << "]\n";
      } else {
        wcout << "\nMesh\n"
              << " Primitives: " << mesh->primitiveCount() << "\n";
        for (uint32_t i = 0; i < mesh->primitiveCount(); i++) {
          const auto& prim = mesh->primitive(i);
          wcout << "  [" << i << "] ";
          switch (prim.topology()) {
          case CG_NS::TopologyPoint:
            wcout << "Point";
            break;
          case CG_NS::TopologyLine:
            wcout << "Line";
            break;
          case CG_NS::TopologyTriangle:
            wcout << "Triangle";
            break;
          case CG_NS::TopologyLnStrip:
            wcout << "Line Strip";
            break;
          case CG_NS::TopologyTriStrip:
            wcout << "Triangle Strip";
            break;
          case CG_NS::TopologyTriFan:
            wcout << "Triangle Fan";
            break;
          default:
            wcout << "???";
          }
          wcout << ", " << hex << prim.dataMask() << dec << "h\n";
        }
      }
    };

    bool ctorChk = true;
    bool dtorChk = true;
    bool countChk = true;
    bool topChk = true;
    bool bindChk = true;

    auto& buf = Primitive::Impl::buffer_;
    auto& segs = Primitive::Impl::segments_;

    a.push_back({L"Primitive::Impl::buffer_", buf != nullptr});
    a.push_back({L"Primitive::Impl::segments_",
                 segs.size() == 1 && segs.front().offset == 0 &&
                 segs.front().size == buf->size()});

    wcout << "\nBuffer: " << Primitive::Impl::buffer_->size() << endl;
    print();

    Mesh::Data data;
    data.data.push_back(make_unique<char[]>(2048));
    data.primitives.push_back({});
    auto& prim = data.primitives.back();
    prim.accessors.push_back({VxDataPosition, 0, 0, 24, 12});

    Mesh m1(data);
    print(&m1);
    print();
    if (buf->size() == (24*12)) {
      if (segs.size() != 0)
        ctorChk = false;
    } else {
      if (segs.size() != 1 || segs.front().offset != (24*12) ||
          segs.front().size != buf->size() - (24*12))
        ctorChk = false;
    }
    if (m1.primitiveCount() != 1)
      countChk = false;
    if (m1[0].topology() != CG_NS::TopologyTriangle)
      topChk = false;
    if (!(m1[0].dataMask() & VxDataPosition) ||
        m1[0].dataMask() & VxDataNormal)
      bindChk = false;

    Mesh* m2 = new Mesh(data);
    print(m2);
    print();
    if (buf->size() == (2*24*12)) {
      if (segs.size() != 0)
        ctorChk = false;
    } else {
      if (segs.size() != 1 || segs.front().offset != (2*24*12) ||
          segs.front().size != buf->size() - (2*24*12))
        ctorChk = false;
    }
    if (m2->primitiveCount() != 1)
      countChk = false;
    if (m2->primitive(0).topology() != CG_NS::TopologyTriangle)
      topChk = false;
    if (!(m2->primitive(0).dataMask() & VxDataPosition) ||
        m2->primitive(0).dataMask() & VxDataJoints0)
      bindChk = false;

    prim.accessors.push_back({VxDataIndices, 0, 1024, 36, 2});

    Mesh m3(data);
    print(&m3);
    print();
    if (segs.size() != 1 || segs.front().offset != (3*24*12+36*2) ||
        segs.front().size != buf->size() - (3*24*12+36*2))
      ctorChk = false;
    if (m3.primitiveCount() != 1)
      countChk = false;
    if (m3[0].topology() != CG_NS::TopologyTriangle)
      topChk = false;
    if (!(m3[0].dataMask() & VxDataPosition) ||
        m3[0].dataMask() & VxDataTexCoord0)
      bindChk = false;

    delete m2;
    print();
    if (segs.size() != 2 || segs.front().offset != (24*12) ||
        segs.front().size != (24*12) ||
        (++segs.begin())->offset != (3*24*12+36*2) ||
        (++segs.begin())->size != buf->size() - (3*24*12+36*2))
      dtorChk = false;

    prim.accessors.pop_back();

    Mesh m4(data);
    print(&m4);
    print();
    if (segs.size() != 1 || segs.front().offset != (3*24*12+36*2) ||
        segs.front().size != buf->size() - (3*24*12+36*2))
      ctorChk = false;
    if (m4.primitiveCount() != 1)
      countChk = false;
    if (m4[0].topology() != CG_NS::TopologyTriangle)
      topChk = false;
    if (!(m4[0].dataMask() & VxDataPosition) ||
        m4[0].dataMask() & VxDataTexCoord1)
      bindChk = false;

    prim.accessors.push_back({VxDataTexCoord0, 0, 24*12, 24, 8});
    prim.topology = CG_NS::TopologyPoint;

    Mesh m5(data);
    print(&m5);
    print();
    if (segs.size() != 1 || segs.front().offset != (4*24*12+36*2+24*8) ||
        segs.front().size != buf->size() - (4*24*12+36*2+24*8))
      ctorChk = false;
    if (m5.primitiveCount() != 1)
      countChk = false;
    if (m5[0].topology() != CG_NS::TopologyPoint)
      topChk = false;
    if (!(m5[0].dataMask() & VxDataPosition) ||
        !(m5[0].dataMask() & VxDataTexCoord0) ||
        m5[0].dataMask() & VxDataTexCoord1)
      bindChk = false;

    a.push_back({L"Mesh(Data)", ctorChk});
    a.push_back({L"~Mesh()", dtorChk});
    a.push_back({L"primitiveCount()", countChk});
    a.push_back({L"topology()", topChk});
    a.push_back({L"dataMask()", bindChk});

    return a;
  }
};

Test* meshTest() {
  static MeshTest test;
  return &test;
}

TEST_NS_END
