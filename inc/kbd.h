//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __KBD_H__
#define __KBD_H__

#define KBS_DIB		0x01
#define NO              0

#define SHIFT           (1 << 0)
#define CTL             (1 << 1)
#define ALT             (1 << 2)
#define CAPSLOCK        (1 << 3)
#define NUMLOCK         (1 << 4)
#define SCROLLLOCK      (1 << 5)
#define E0ESC           (1 << 6)

// Special keycodes
#define KEY_HOME	0xE0
#define KEY_END		0xE1
#define KEY_UP		0xE2
#define KEY_DN		0xE3
#define KEY_LF		0xE4
#define KEY_RT		0xE5
#define KEY_PGUP	0xE6
#define KEY_PGDN	0xE7
#define KEY_INS		0xE8
#define KEY_DEL		0xE9
#define KEY_LGUI	0xEA
#define KEY_RGUI	0xEB
#define KEY_RCTRL	0xEC
#define KEY_RALT	0xED

#define F1	0xF1
#define F2	0xF2
#define F3	0xF3
#define F4	0xF4
#define F5	0xF5
#define F6	0xF6
#define F7	0xF7
#define F8	0xF8
#define F9	0xF9
#define F10	0xF10
#define F11	0xF11
#define F12	0xF12

#endif
