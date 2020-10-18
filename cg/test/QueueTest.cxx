//
// cg
// QueueTest.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "UnitTests.h"
#include "yf/cg/Queue.h"

using namespace TEST_NS;
using namespace CG_NS;
using namespace std;

INTERNAL_NS_BEGIN

struct QueueTest : Test {
  QueueTest() : Test(L"Queue") {}

  Assertions run(const vector<string>&) {
    struct CmdBuffer_ : CmdBuffer {
      CmdBuffer_(const Queue& owner) : _queue(owner) {}

      Result encode(const Encoder&) {
        return Result::Failure;
      }
      Result enqueue() {
        return Result::Failure;
      }
      Result reset() {
        return Result::Failure;
      }
      bool isReady() {
        return true;
      }
      const Queue& queue() const {
        return _queue;
      }

     private:
      const Queue& _queue;
    };

    struct Queue_ : Queue {
      Queue_(CapabilityMask capab) : Queue(capab) {}

      CmdBuffer::Ptr makeCmdBuffer() {
        return make_unique<CmdBuffer>(*this);
      }
      Result submit(CompletionFn) {
        return Result::Failure;
      }
    };

    Assertions a;

    Queue_ q1(Queue::Graphics | Queue::Transfer);
    Queue_ q2(Queue::Compute);
    auto cb = q1.makeCmdBuffer();

    a.push_back({L"Queue q1(Graphics | Transfer)",
                 q1.capabilities == (Queue::Graphics | Queue::Transfer)});
    a.push_back({L"Queue q2(Compute)", q2.capabilities == Queue::Compute});
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
