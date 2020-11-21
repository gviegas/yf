//
// WS
// Window.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#if defined(__linux__) // TODO: add other unix systems here
//# include "unix/WindowWL.h"
# include "unix/WindowXCB.h"
# define WS_WINDOW_T WindowXCB
#elif defined(__APPLE__)
# include "macos/WindowMAC.h"
# define WS_WINDOW_T WindowMAC
#elif defined(_WIN32)
# include "win32/WindowW32.h"
# define WS_WINDOW_T WindowW32
#else
# error "Invalid platform"
#endif // defined(__linux__)

//#include "Window.h"

using namespace WS_NS;
using namespace std;

Window::Ptr Window::make(uint32_t width,
                         uint32_t height,
                         const wstring& title,
                         CreationMask mask) {

  return make_unique<WS_WINDOW_T>(width, height, title, mask);
}

Window::~Window() { }
