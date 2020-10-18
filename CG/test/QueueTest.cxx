//
// yf
// QueueTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "CGQueue.h"
#include "Defs.h"

using namespace TEST_NS;
using namespace YF_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct QueueTest : Test {
  QueueTest() : Test(L"CGQueue") {}

  Assertions run(const vector<string>& args) {
    struct CmdBuffer : CGCmdBuffer {
      CmdBuffer(const CGQueue& owner) : _queue(owner) {}
      CGResult encode(const CGEncoder&) { return CGResult::Failure; }
      CGResult enqueue() { return CGResult::Failure; }
      CGResult reset() { return CGResult::Failure; }
      bool isReady() { return true; }
      const CGQueue& queue() const { return _queue; };
      private: const CGQueue& _queue;
    };

    struct Queue : CGQueue {
      Queue(CapabilityMask capab) : CGQueue(capab) {}
      CGCmdBuffer::Ptr makeCmdBuffer()
      { return make_unique<CmdBuffer>(*this); }
      CGResult submit(CompletionFn) { return CGResult::Failure; }
    };

    Assertions a;

    Queue q1(CGQueue::Graphics | CGQueue::Transfer);
    Queue q2(CGQueue::Compute);
    auto cb = q1.makeCmdBuffer();

    a.push_back({L"CGQueue q1(Graphics | Transfer)",
                 q1.capabilities == (CGQueue::Graphics | CGQueue::Transfer)});
    a.push_back({L"CGQueue q2(Compute)", q2.capabilities == CGQueue::Compute});
    a.push_back({L"cb = q1.makeCmdBuffer()", cb != nullptr});
    a.push_back({L"&cb->queue() == &q1", &cb->queue() == &q1});
    a.push_back({L"&cb->queue() == &q2", &cb->queue() != &q2});

    return a;
  }
};

INTERNAL_NS_END

Test* TEST_NS::queueTest() {
  static QueueTest test;
  return &test;
}
