//
// WS
// Event.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#if defined(__linux__) // TODO: add other unix systems here
//# include "unix/EventWL.h"
# include "unix/EventXCB.h"
# define WS_EVENT_T EventXCB
#elif defined(__APPLE__)
# include "macos/EventMAC.h"
# define WS_EVENT_T EventMAC
#elif defined(_WIN32)
# include "win32/EventW32.h"
# define WS_EVENT_T EventW32
#else
# error "Invalid platform"
#endif // defined(__linux__)

//#include "Event.h"

using namespace WS_NS;

Event::~Event() { }

Event& Event::get() {
  return WS_EVENT_T::get();
}
