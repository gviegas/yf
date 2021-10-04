//
// SG
// MeshTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <iostream>

#include "Test.h"
#include "MeshImpl.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct MeshTest : Test {
  MeshTest() : Test(L"Mesh") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto print = [](Mesh* mesh = nullptr) {
      if (!mesh) {
        wcout << "\nSegments: " << Mesh::Impl::segments_.size() << "\n";
        for (const auto& s : Mesh::Impl::segments_)
          wcout << " [" << s.offset << ":" << s.size << "]\n";
      } else {
        auto& impl = mesh->impl();
        wcout << "\nMesh Impl\n"
              << " Primitives: " << impl.primitiveCount() << "\n";
        for (uint32_t i = 0; i < impl.primitiveCount(); i++) {
          wcout << "  [" << i << "] ";
          switch (impl.topology(i)) {
          case CG_NS::TopologyPoint:
            wcout << "Point\n";
            break;
          case CG_NS::TopologyLine:
            wcout << "Line\n";
            break;
          case CG_NS::TopologyTriangle:
            wcout << "Triangle\n";
            break;
          case CG_NS::TopologyLnStrip:
            wcout << "Line Strip\n";
            break;
          case CG_NS::TopologyTriStrip:
            wcout << "Triangle Strip\n";
            break;
          case CG_NS::TopologyTriFan:
            wcout << "Triangle Fan\n";
            break;
          default:
            wcout << "???\n";
          }
        }
      }
    };

    bool ctorChk = true;
    bool dtorChk = true;
    bool countChk = true;
    bool topChk = true;
    bool bindChk = true;
    bool ibufChk = true;

    auto& buf = Mesh::Impl::buffer_;
    auto& segs = Mesh::Impl::segments_;

    a.push_back({L"Mesh::Impl::buffer_", buf != nullptr});
    a.push_back({L"Mesh::Impl::segments_", segs.size() == 1 &&
                                           segs.front().offset == 0 &&
                                           segs.front().size == buf->size()});

    wcout << "\nBuffer: " << Mesh::Impl::buffer_->size() << endl;
    print();

    Mesh::Data data;
    data.data.push_back(make_unique<char[]>(2048));
    data.primitives.push_back({});
    auto& prim = data.primitives.back();
    prim.vxAccessors.emplace(VxTypePosition,
                             Mesh::Data::Accessor{0, 0, 24, 12});

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
    if (m1.impl().primitiveCount() != 1)
      countChk = false;
    if (m1.impl().topology(0) != CG_NS::TopologyTriangle)
      topChk = false;
    if (!m1.impl().canBind(VxTypePosition) || m1.impl().canBind(VxTypeNormal))
      bindChk = false;
    if (m1.impl().isIndexed())
      ibufChk = false;

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
    if (m2->impl().primitiveCount() != 1)
      countChk = false;
    if (m2->impl().topology(0) != CG_NS::TopologyTriangle)
      topChk = false;
    if (!m2->impl().canBind(VxTypePosition) ||
        m2->impl().canBind(VxTypeJoints0))
      bindChk = false;

    prim.ixAccessor.dataIndex = 0;
    prim.ixAccessor.dataOffset = 1024;
    prim.ixAccessor.elementN = 36;
    prim.ixAccessor.elementSize = 2;

    Mesh m3(data);
    print(&m3);
    print();
    if (segs.size() != 1 || segs.front().offset != (3*24*12+36*2) ||
        segs.front().size != buf->size() - (3*24*12+36*2))
      ctorChk = false;
    if (m3.impl().primitiveCount() != 1)
      countChk = false;
    if (m3.impl().topology(0) != CG_NS::TopologyTriangle)
      topChk = false;
    if (!m3.impl().canBind(VxTypePosition) ||
        m3.impl().canBind(VxTypeTexCoord0))
      bindChk = false;
    if (!m3.impl().isIndexed())
      ibufChk = false;

    delete m2;
    print();
    if (segs.size() != 2 || segs.front().offset != (24*12) ||
        segs.front().size != (24*12) ||
        (++segs.begin())->offset != (3*24*12+36*2) ||
        (++segs.begin())->size != buf->size() - (3*24*12+36*2))
      dtorChk = false;

    prim.ixAccessor = {};

    Mesh m4(data);
    print(&m4);
    print();
    if (segs.size() != 1 || segs.front().offset != (3*24*12+36*2) ||
        segs.front().size != buf->size() - (3*24*12+36*2))
      ctorChk = false;
    if (m4.impl().primitiveCount() != 1)
      countChk = false;
    if (m4.impl().topology(0) != CG_NS::TopologyTriangle)
      topChk = false;
    if (!m4.impl().canBind(VxTypePosition) ||
        m4.impl().canBind(VxTypeTexCoord1))
      bindChk = false;
    if (m4.impl().isIndexed())
      ibufChk = false;

    prim.vxAccessors.emplace(VxTypeTexCoord0,
                             Mesh::Data::Accessor{0, 24*12, 24, 8});
    prim.topology = CG_NS::TopologyPoint;

    Mesh m5(data);
    print(&m5);
    print();
    if (segs.size() != 1 || segs.front().offset != (4*24*12+36*2+24*8) ||
        segs.front().size != buf->size() - (4*24*12+36*2+24*8))
      ctorChk = false;
    if (m5.impl().primitiveCount() != 1)
      countChk = false;
    if (m5.impl().topology(0) != CG_NS::TopologyPoint)
      topChk = false;
    if (!m5.impl().canBind(VxTypePosition) ||
        !m5.impl().canBind(VxTypeTexCoord0) ||
        m5.impl().canBind(VxTypeTexCoord1))
      bindChk = false;

    a.push_back({L"Mesh(Data)", ctorChk});
    a.push_back({L"~Mesh()", dtorChk});
    a.push_back({L"Mesh::Impl::primitiveCount()", countChk});
    a.push_back({L"Mesh::Impl::topology()", topChk});
    a.push_back({L"Mesh::Impl::canBind()", bindChk});
    a.push_back({L"Mesh::Impl::isIndexed()", ibufChk});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* meshTest() {
  static MeshTest test;
  return &test;
}

TEST_NS_END
