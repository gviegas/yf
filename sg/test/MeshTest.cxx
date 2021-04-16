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

    auto print = [] {
      wcout << "\n#Segments#\n";
      for (const auto& s : Mesh::Impl::segments_) {
        wcout << "[" << s.offset << ":" << s.size << "]\n";
      }
      wcout << "#" << Mesh::Impl::segments_.size() << endl;
    };

    bool ctorChk = true;
    bool dtorChk = true;
    bool bindChk = true;
    bool ibufChk = true;

    auto& buf = Mesh::Impl::buffer_;
    auto& segs = Mesh::Impl::segments_;

    a.push_back({L"Mesh::Impl::buffer_", buf != nullptr});
    a.push_back({L"Mesh::Impl::segments_", segs.size() == 1 &&
                                           segs.front().offset == 0 &&
                                           segs.front().size == buf->size_});

    wcout << "\n#Buffer#\n" << "#" << Mesh::Impl::buffer_->size_ << endl;
    print();

    Mesh::Data data;
    data.data.push_back(make_unique<uint8_t[]>(2048));
    data.vxAccessors.emplace(VxTypePosition,
                             Mesh::Data::Accessor{0, 0, 24, 12});

    Mesh m1(data);
    print();
    if (buf->size_ == (24*12)) {
      if (segs.size() != 0)
        ctorChk = false;
    } else {
      if (segs.size() != 1 || segs.front().offset != (24*12) ||
          segs.front().size != buf->size_ - (24*12))
        ctorChk = false;
    }
    if (!m1.impl().canBind(VxTypePosition) || m1.impl().canBind(VxTypeNormal))
      bindChk = false;
    if (m1.impl().isIndexed())
      ibufChk = false;

    Mesh m2(data);
    print();
    if (buf->size_ == (2*24*12)) {
      if (segs.size() != 0)
        ctorChk = false;
    } else {
      if (segs.size() != 1 || segs.front().offset != (2*24*12) ||
          segs.front().size != buf->size_ - (2*24*12))
        ctorChk = false;
    }
    if (!m2.impl().canBind(VxTypePosition) || m2.impl().canBind(VxTypeJoints0))
      bindChk = false;

    data.ixAccessor.dataIndex = 0;
    data.ixAccessor.dataOffset = 1024;
    data.ixAccessor.elementN = 36;
    data.ixAccessor.elementSize = 2;

    Mesh m3(data);
    print();
    if (segs.size() != 1 || segs.front().offset != (3*24*12+36*2) ||
        segs.front().size != buf->size_ - (3*24*12+36*2))
      ctorChk = false;
    if (!m3.impl().canBind(VxTypePosition) ||
        m3.impl().canBind(VxTypeTexCoord0))
      bindChk = false;
    if (!m3.impl().isIndexed())
      ibufChk = false;

    m2.~Mesh();
    print();
    if (segs.size() != 2 || segs.front().offset != (24*12) ||
        segs.front().size != (24*12) ||
        (++segs.begin())->offset != (3*24*12+36*2) ||
        (++segs.begin())->size != buf->size_ - (3*24*12+36*2))
      dtorChk = false;

    data.ixAccessor = {};
    Mesh m4(data);
    print();
    if (segs.size() != 1 || segs.front().offset != (3*24*12+36*2) ||
        segs.front().size != buf->size_ - (3*24*12+36*2))
      ctorChk = false;
    if (!m4.impl().canBind(VxTypePosition) ||
        m4.impl().canBind(VxTypeTexCoord1))
      bindChk = false;
    if (m4.impl().isIndexed())
      ibufChk = false;

    data.vxAccessors.emplace(VxTypeTexCoord0,
                             Mesh::Data::Accessor{0, 24*12, 24, 8});
    Mesh m5(data);
    print();
    if (segs.size() != 1 || segs.front().offset != (4*24*12+36*2+24*8) ||
        segs.front().size != buf->size_ - (4*24*12+36*2+24*8))
      ctorChk = false;
    if (!m5.impl().canBind(VxTypePosition) ||
        !m5.impl().canBind(VxTypeTexCoord0) ||
        m5.impl().canBind(VxTypeTexCoord1))
      bindChk = false;

    a.push_back({L"Mesh(Data)", ctorChk});
    a.push_back({L"~Mesh()", dtorChk});
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
