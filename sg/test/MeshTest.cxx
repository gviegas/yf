//
// SG
// MeshTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include <iostream>

#include "UnitTests.h"

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

    auto& buf = Mesh::Impl::buffer_;
    auto& segs = Mesh::Impl::segments_;

    a.push_back({L"Mesh::Impl::buffer_", buf != nullptr});
    a.push_back({L"Mesh::Impl::segments_", segs.size() == 1 &&
                                           segs.front().offset == 0 &&
                                           segs.front().size == buf->size_});

    wcout << "\n#Buffer#\n" << "#" << Mesh::Impl::buffer_->size_ << endl;
    print();

    uint8_t b[2048];
    Mesh::Data data;
    data.vertex.data = b;
    data.vertex.count = 10;
    data.vertex.stride = 20;
    data.index.data = b;
    data.index.count = 50;
    data.index.stride = 2;

    Mesh m1(data);
    print();
    if (segs.size() != 1 || segs.front().offset != (10*20+50*2) ||
        segs.front().size != buf->size_ - (10*20+50*2))
      ctorChk = false;

    Mesh m2(data);
    print();
    if (segs.size() != 1 || segs.front().offset != ((10*20+50*2)*2) ||
        segs.front().size != buf->size_ - ((10*20+50*2)*2))
      ctorChk = false;

    data.index.data = nullptr;
    data.index.count = 0;
    data.index.stride = 0;

    Mesh m3(data);
    print();
    if (segs.size() != 1 || segs.front().offset != ((10*20+50*2)*2+10*20) ||
        segs.front().size != buf->size_ - ((10*20+50*2)*2+10*20))
      ctorChk = false;

    m2.~Mesh();
    print();
    if (segs.size() != 2 || segs.front().offset != (10*20+50*2) ||
        segs.front().size != (10*20+50*2) ||
        (++segs.begin())->offset != ((10*20+50*2)*2+10*20) ||
        (++segs.begin())->size != buf->size_ - ((10*20+50*2)*2+10*20))
      dtorChk = false;

    data.vertex.count = 15;

    Mesh m4(data);
    print();
    if (segs.size() != 1 || segs.front().offset != ((10*20+50*2)*2+10*20) ||
        segs.front().size != buf->size_ - ((10*20+50*2)*2+10*20))
      ctorChk = false;

    a.push_back({L"Mesh(Data)", ctorChk});
    a.push_back({L"~Mesh()", dtorChk});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::meshTest() {
  static MeshTest test;
  return &test;
}
