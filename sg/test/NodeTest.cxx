//
// SG
// NodeTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <iostream>
#include <map>

#include "UnitTests.h"
#include "Node.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct NodeTest : Test {
  NodeTest() : Test(L"Node") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    vector<Node> nodes{6};
    map<Node*, int> ids;
    for (size_t i = 0; i < nodes.size(); ++i) {
      ids.emplace(&nodes[i], i);
      nodes[i].setName(L"Node " + to_wstring(i));
      wcout << "\nNode named `" << nodes[i].name() << "` created";
    }
    wcout << endl;

    auto printChd = [&](Node& root) {
      wcout << "\n{" << ids[&root] << "}\n";
      root.traverse((function<bool(Node&)>)[&](Node& node) -> bool {
        if (node.parent() != &root)
          return false;
        else
          wcout << " --- (" << ids[&node] << ")";
        return true;
      }, true);
      wcout << endl;
    };

    a.push_back({L"Node() [6]", true});

    a.push_back({L"count()", true});
    for (auto& node : nodes) {
      if (node.count() != 1) {
        a.back().second = false;
        break;
      }
    }

    nodes[0].insert(nodes[1]);
    a.push_back({L"#0.insert(#1)", nodes[0].count() == 2 &&
                                   nodes[1].parent() == &nodes[0]});
    nodes[0].insert(nodes[2]);
    a.push_back({L"#0.insert(#2)", nodes[0].count() == 3 &&
                                   nodes[2].parent() == &nodes[0]});
    nodes[1].insert(nodes[3]);
    a.push_back({L"#1.insert(#3)", nodes[1].count() == 2 &&
                                   nodes[3].parent() == &nodes[1] &&
                                   nodes[0].count() == 4});
    nodes[2].insert(nodes[4]);
    a.push_back({L"#2.insert(#4)", nodes[2].count() == 2 &&
                                   nodes[4].parent() == &nodes[2] &&
                                   nodes[0].count() == 5});
    nodes[4].insert(nodes[5]);
    a.push_back({L"#4.insert(#5)", nodes[4].count() == 2 &&
                                   nodes[5].parent() == &nodes[4] &&
                                   nodes[0].count() == 6});
    nodes[0].insert(nodes[4]);
    a.push_back({L"#0.insert(#4)", nodes[0].count() == 6 &&
                                   nodes[4].parent() == &nodes[0] &&
                                   nodes[2].count() == 1});
    nodes[1].drop();
    a.push_back({L"#1.drop()", nodes[1].parent() == nullptr &&
                               nodes[3].parent() == &nodes[1] &&
                               nodes[1].count() == 2 &&
                               nodes[3].count() == 1 &&
                               nodes[0].count() == 4});
    nodes[0].prune();
    a.push_back({L"#0.prune()", nodes[0].parent() == nullptr &&
                                nodes[2].parent() == nullptr &&
                                nodes[4].parent() == nullptr &&
                                nodes[0].count() == 1 &&
                                nodes[2].count() == 1 &&
                                nodes[4].count() == 2});
    nodes[1].insert(nodes[4]);
    a.push_back({L"#1.insert(#4)", nodes[1].count() == 4 &&
                                   nodes[3].count() == 1 &&
                                   nodes[4].parent() == &nodes[1] &&
                                   nodes[4].count() == 2 &&
                                   nodes[5].parent() == &nodes[4] &&
                                   nodes[5].count() == 1 &&
                                   nodes[3].isDescendantOf(nodes[1]) &&
                                   nodes[4].isDescendantOf(nodes[1]) &&
                                   nodes[5].isDescendantOf(nodes[1]) &&
                                   nodes[5].isDescendantOf(nodes[4]) &&
                                   !nodes[4].isDescendantOf(nodes[3]) &&
                                   !nodes[1].isDescendantOf(nodes[4])});

    auto chds = nodes[1].children();
    auto chdN = nodes[1].children(chds);
    a.push_back({L"#1.children(...)", true});
    if (chdN != 2 || chdN*2 != chds.size()) {
      a.back().second = false;
    } else {
      for (size_t i = 0; i < chdN/2; ++i) {
        if (count(chds.begin(), chds.end(), chds[i]) != 2) {
          a.back().second = false;
          break;
        }
      }
    }

    for (auto& node : nodes)
      printChd(node);

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::nodeTest() {
  static NodeTest test;
  return &test;
}
