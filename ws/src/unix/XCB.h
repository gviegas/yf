//
// WS
// XCB.h
//
// Copyright © 2020 Gustavo C. Viegas.
//

#ifndef YF_WS_XCB_H
#define YF_WS_XCB_H

#include <xcb/xcb.h>

#include "Defs.h"

WS_NS_BEGIN

/// XCB variables.
///
struct VarsXCB {
  xcb_connection_t* connection;
  xcb_visualid_t visualId;
  xcb_window_t root;
  uint32_t whitePixel;
  uint32_t blackPixel;
  xcb_atom_t protocolAtom;
  xcb_atom_t deleteAtom;
  xcb_atom_t titleAtom;
  xcb_atom_t utf8Atom;
  xcb_atom_t classAtom;
};

/// Gets the shared variables instance.
///
const VarsXCB& varsXCB();

/// Initializes XCB.
///
void initXCB();

/// Deinitializes XCB.
///
void deinitXCB();

/// Dynamic symbols.
///
extern xcb_connection_t*
(*connectXCB)(const char*, int*);

extern void
(*disconnectXCB)(xcb_connection_t*);

extern int
(*flushXCB)(xcb_connection_t*);

extern int
(*connectionHasErrorXCB)(xcb_connection_t*);

extern uint32_t
(*generateIdXCB)(xcb_connection_t*);

extern xcb_generic_event_t*
(*pollForEventXCB)(xcb_connection_t*);

extern xcb_generic_error_t*
(*requestCheckXCB)(xcb_connection_t*, xcb_void_cookie_t);

extern const struct xcb_setup_t*
(*getSetupXCB)(xcb_connection_t*);

extern xcb_screen_iterator_t
(*setupRootsIteratorXCB)(const xcb_setup_t*);

extern xcb_void_cookie_t
(*createWindowCheckedXCB)(xcb_connection_t*, uint8_t, xcb_window_t,
                          xcb_window_t, int16_t, int16_t, uint16_t,
                          uint16_t, uint16_t, uint16_t, xcb_visualid_t,
                          uint32_t, const void*);

extern xcb_void_cookie_t
(*destroyWindowXCB)(xcb_connection_t*, xcb_window_t);

extern xcb_void_cookie_t
(*mapWindowCheckedXCB)(xcb_connection_t*, xcb_window_t);

extern xcb_void_cookie_t
(*unmapWindowCheckedXCB)(xcb_connection_t*, xcb_window_t);

extern xcb_void_cookie_t
(*configureWindowCheckedXCB)(xcb_connection_t*, xcb_window_t, uint32_t,
                             const void*);

extern xcb_intern_atom_cookie_t
(*internAtomXCB)(xcb_connection_t*, uint8_t, uint16_t, const char*);

extern xcb_intern_atom_reply_t*
(*internAtomReplyXCB)(xcb_connection_t*, xcb_intern_atom_cookie_t,
                      xcb_generic_error_t**);

extern xcb_void_cookie_t
(*changePropertyCheckedXCB)(xcb_connection_t*, uint8_t, xcb_window_t,
                            xcb_atom_t, xcb_atom_t, uint8_t, uint32_t,
                            const void*);

extern xcb_void_cookie_t
(*changeKeyboardControlCheckedXCB)(xcb_connection_t*, uint32_t, const void*);

WS_NS_END

#endif // YF_WS_XCB_H
