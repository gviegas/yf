//
// WS
// Window.cxx
//
// Copyright © 2020 Gustavo C. Viegas.
//

#include "Window.h"

using namespace std;

WS_NS_BEGIN

wstring Window::appId{};

// `Platform` provides the implementation.
Window::Ptr makeWindow(uint32_t width,
                       uint32_t height,
                       const wstring& title,
                       Window::CreationMask mask);

Window::Ptr Window::make(uint32_t width,
                         uint32_t height,
                         const wstring& title,
                         CreationMask mask) {

  return makeWindow(width, height, title, mask);
}

Window::~Window() { }

WS_NS_END
