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

/// Initializes XCB.
///
bool initXCB();

/// Deinitializes XCB.
///
void deinitXCB();

/// Dynamic symbols.
///
extern xcb_connection_t
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
(poolForEventXCB)(xcb_connection_t*);

extern xcb_generic_error_t*
(*requestCheckXCB)(xcb_connection_t*, xcb_void_cookie_t);

extern const struct scb_setup_t*
(getSetupXCB)(xcb_connection_t*);

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
