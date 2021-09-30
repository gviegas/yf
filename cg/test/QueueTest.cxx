//
// CG
// QueueTest.cxx
//
// Copyright © 2020-2021 Gustavo C. Viegas.
//

#include "Test.h"
#include "Queue.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct QueueTest : Test {
  QueueTest() : Test(L"Queue") { }

  Assertions run(const vector<string>&) {
    class CmdBuffer_ : public CmdBuffer {
      Queue& queue_;
     public:
      CmdBuffer_(Queue& owner) : queue_(owner) { }
      void encode(const Encoder&) { }
      void enqueue() { }
      void reset() { }
      bool isPending() { return true; }
      Queue& queue() { return queue_; }
    };

    class Queue_ : public Queue {
      CapabilityMask capab_;
     public:
      Queue_(CapabilityMask capab) : capab_(capab) { }
      CmdBuffer::Ptr cmdBuffer() { return make_unique<CmdBuffer_>(*this); }
      void submit() { }
      CapabilityMask capabilities() const { return capab_; }
    };

    Assertions a;

    Queue_ q1(Queue::Graphics | Queue::Transfer);
    Queue_ q2(Queue::Compute);
    auto cb = q1.cmdBuffer();

    a.push_back({L"Queue q1(Graphics | Transfer)",
                 q1.capabilities() == (Queue::Graphics | Queue::Transfer)});
    a.push_back({L"Queue q2(Compute)", q2.capabilities() == Queue::Compute});
    a.push_back({L"cb = q1.cmdBuffer()", cb != nullptr});
    a.push_back({L"&cb->queue() == &q1", &cb->queue() == &q1});
    a.push_back({L"&cb->queue() == &q2", &cb->queue() != &q2});

    return a;
  }
};

INTERNAL_NS_END

TEST_NS_BEGIN

Test* queueTest() {
  static QueueTest test;
  return &test;
}

TEST_NS_END
