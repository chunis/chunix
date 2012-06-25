#include <types.h>
#include <x86.h>
#include "console.h"
#include "kbd.h"

// process keyboard input
// TODO: scan code for "Print Screen" and "Pause"
const uint8_t norm_map[128] = {
	0x0, 0x0, '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=', '\b', '\t',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '[', ']', '\n', 0x0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0x0, '*',
	0x0, ' ', 0x0, F1, F2, F3, F4, F5,
	F6, F7, F8, F9, F10
};

const uint8_t shift_map[128] = {
	0x0, 0x0, '!', '@', '#', '$', '%', '^',
	'&', '*', '(', ')', '_', '+', '\b', '\t',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
	'O', 'P', '{', '}', '\n', 0x0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
	'"', '~', 0x0, '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0x0, '*',
	0x0, ' '
};

#define C(x) (x - '@')

const uint8_t ctrl_map[128] = {
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	C('Q'), C('W'), C('E'), C('R'), C('T'), C('Y'), C('U'), C('I'),
	C('O'), C('P'), 0x0, 0x0, '\r', 0x0, C('A'), C('S'),
	C('D'), C('F'), C('G'), C('H'), C('J'), C('K'), C('L'), 0x0,
	0x0, 0x0, 0x0, C('\\'),C('Z'), C('X'), C('C'), C('V'),
	C('B'), C('N'), C('M'), 0x0, 0x0, C('/'), 0x0, 0x0,
};

const uint8_t e0_map[128] = {
	[0x1C] = '\n', //*KP_Enter
	[0x1D] = KEY_RCTRL, //*KP_Enter
	[0x35] = '/',  //*KP_Div
	[0x38] = KEY_RALT,  //*KP_Div
	[0x48] = KEY_UP,
	[0x47] = KEY_HOME,
	[0x49] = KEY_PGUP,
	[0x4B] = KEY_LF,
	[0x4D] = KEY_RT,
	[0x4F] = KEY_END, 
	[0x50] = KEY_DN,
	[0x51] = KEY_PGDN,
	[0x52] = KEY_INS,
	[0x53] = KEY_DEL,
	[0x5B] = KEY_LGUI,
	[0x5C] = KEY_RGUI,
};

static uint8_t shiftmap[128] =
{
	[0x1D] = CTL,
	[0x2A] = SHIFT,
	[0x36] = SHIFT,
	[0x38] = ALT,
};

static uint8_t togglemap[128] =
{
	[0x3A] = CAPSLOCK,
	[0x45] = NUMLOCK,
	[0x46] = SCROLLLOCK
};

const uint8_t *kb_map[4] = {
	norm_map,
	shift_map,
	ctrl_map,
	ctrl_map
};

// process keyboard data, return character if possible
// return 0 if character not finished, or -1 if no data
int read_kbd(void)
{
	char val;
	int sc, _sc;	// scan code
	static uint32_t flag;  // flag used to record 'Ctrl/Alt/Shift'
	int e0_flag = 0;

	// no data in keyboard buffer
	if ((inb(0x64) & KBS_DIB) == 0)
		return -1;

	sc = inb(0x60);
	_sc = (sc & 0x7f);

	if(sc == 0xE0){
		flag |= E0ESC;
		return 0;
	} else if(sc & 0x80){	// key released
		flag &= ~(shiftmap[_sc] | E0ESC);
		return 0;
	} else if(flag & E0ESC){
		flag &= ~E0ESC;
		e0_flag = 1;
	}

	flag |= shiftmap[_sc];
	flag ^= togglemap[_sc];

	if(e0_flag)
		return e0_map[_sc];

	val = kb_map[flag & (CTL | SHIFT)][_sc];
	if (flag & CAPSLOCK) {
		if ('a' <= val && val <= 'z')
			val += 'A' - 'a';
		else if ('A' <= val && val <= 'Z')
			val += 'a' - 'A';
	}
	return val;
}

char read_char(void)
{
	int c;
	char val;

	while((c = read_kbd()) != -1){
		if(c == 0)
			continue;
		val = (char)c;
		cbuf.buf[cbuf.wpos++] = val;
		if(cbuf.wpos >= CONS_SIZE){
			cbuf.wpos = 0;
		}
		process_char(val);
	}

	/* finish handle key and re-enable interrupt */
	val = inb(0x61);
	outb(0x61, val|0x80);
	outb(0x61, val&0x7f);

	outb(0x20, 0x20);
}

// process char. At here, we can process inputs like ''C-c' and 'F1'.
void process_char(uint8_t c)
{
	// only output ascii characters now
	if(c <= 0x7f)
		put_c(c);
}

void keyboard_isr(void)
{
	read_char();
}

void init_keyboard(void)
{
	outb(0x21, inb(0x21)&0xfd);
}
