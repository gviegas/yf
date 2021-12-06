//
// SG
// NodeTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include <iostream>
#include <map>

#include "Test.h"
#include "Node.h"

using namespace TEST_NS;
using namespace SG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct NodeTest : Test {
  NodeTest() : Test(L"Node") { }

  Assertions run(const vector<string>&) {
    Assertions a;

    auto printChd = [&](Node& root) {
      wcout << "\n{" << root.name() << "}\n";
      root.traverse((function<bool(Node&)>)[&](Node& node) -> bool {
        if (node.parent() != &root)
          return false;
        else
          wcout << " --- (" << node.name() << ")";
        return true;
      }, true);
      wcout << endl;
    };

    vector<Node> nodes{6};
    for (size_t i = 0; i < nodes.size(); i++) {
      nodes[i].name() = L"#" + to_wstring(i);
      wcout << "\nNode named `" << nodes[i].name() << "` created";
    }
    wcout << endl;

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
      for (size_t i = 0; i < chdN/2; i++) {
        if (count(chds.begin(), chds.end(), chds[i]) != 2) {
          a.back().second = false;
          break;
        }
      }
    }

    for (auto& node : nodes)
      printChd(node);

    const auto& xform = nodes[0].transform();
    auto& xform2 = nodes[0].transform();
    auto prec = wcout.precision(3);

    wcout << "\ntransform() (default):\n";
    for (size_t i = 0; i < xform.rows(); i++) {
      wcout << "\n ";
      for (size_t j = 0; j < xform.columns(); j++)
        wcout << xform[j][i] << "\t";
    }
    wcout << endl;

    xform2 = scale(2.0f, 3.0f, 4.0f) * translate(-1.0f, -2.0f, -3.0f);
    wcout << "\ntransform() (changed):\n";
    for (size_t i = 0; i < xform.rows(); i++) {
      wcout << "\n ";
      for (size_t j = 0; j < xform.columns(); j++)
        wcout << xform[j][i] << "\t";
    }
    wcout << endl;

    wcout.precision(prec);
    a.push_back({L"transform()", true});

    Node nodeA;
    nodeA.name() = L"a";
    Node nodeB;
    nodeB.name() = L"b";
    Node nodeC(nodeB);

    wcout << "\nNode named `" << nodeA.name() << "` created";
    wcout << "\nNode named `" << nodeB.name() << "` created";
    wcout << "\nNode `" << nodeB.name() << "` copied\n";

    a.push_back({L"Node(other)", nodeB.name() == L"b" && nodeB.isLeaf() &&
                                 nodeC.name() == nodeB.name() &&
                                 nodeC.isLeaf()});
    printChd(nodeA);
    printChd(nodeB);
    printChd(nodeC);

    nodeA.insert(nodeC);
    nodeC.name() = L"c";
    const auto b = nodeB.name();
    nodeB = nodeC;

    wcout << "\nNode copy inserted into `" << nodeA.name() << "`";
    wcout << "\nNode copy renamed `" << nodeC.name() << "`";
    wcout << "\nNode copy assigned to `" << b << "`\n";

    a.push_back({L"=", !nodeA.isLeaf() && nodeB.parent() == &nodeA &&
                       nodeB.name() == nodeC.name() &&
                       nodeC.parent() == &nodeA && nodeC.name() == L"c"});
    printChd(nodeA);
    printChd(nodeB);
    printChd(nodeC);

    const Node cNode;
    cNode.parent();
    cNode.name();
    cNode.transform();

    static auto insertN = 0;
    static auto dropN = 0;
    static auto pruneN = 0;

    class Sub : public Node {
     protected:
      void willInsert(Node& node) {
        wcout << " in " << name() << "'s willInsert(" << node.name() << ")\n";
        insertN++;
      }
      void willDrop(Node& node) {
        wcout << " in " << name() << "'s willDrop(" << node.name() << ")\n";
        dropN++;
      }
      void willPrune(Node& node) {
        wcout << " in " << name() << "'s willPrune(" << node.name() << ")\n";
        pruneN++;
      }
    };

    wcout << "\nwill*() overrides:\n";

    Sub sub1, sub2, sub3;
    sub1.name() = L"sub1";
    sub2.name() = L"sub2";
    sub3.name() = L"sub3";

    wcout << "\nCall to sub1.insert(sub2)\n";
    sub1.insert(sub2);

    wcout << "\nCall to sub2.drop()\n";
    sub2.drop();

    wcout << "\nCall to sub2.insert(sub1)\n";
    sub2.insert(sub1);

    wcout << "\nCall to sub2.insert(sub3)\n";
    sub2.insert(sub3);

    wcout << "\nCall to sub1.drop()\n";
    sub1.drop();

    wcout << "\nCall to sub3.insert(1)\n";
    sub3.insert(sub1);

    wcout << "\nCall to sub3.prune()\n";
    sub3.prune();

    a.push_back({L"willInsert()", insertN == 5});
    a.push_back({L"willDrop()", dropN == 4});
    a.push_back({L"willPrune()", pruneN == 2});

    wcout << "\nLeaving scope...\n";

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* nodeTest() {
  static NodeTest test;
  return &test;
}

TEST_NS_END
