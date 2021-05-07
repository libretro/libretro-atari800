#ifndef VKBD_DEF_H
#define VKBD_DEF_H 1

typedef struct {
	char norml[NLETT];
	char shift[NLETT];
	char ctrl[NLETT];
	int val;	
} Mvk;

Mvk MVk[NPLGN*NLIGN*2]={

	{ "Esc", "Esc", "Esc", RETROK_ESCAPE},//0
	{ " 1 ", " ! ", " \xb1 " , RETROK_1},
	{ " 2 ", " \" ", " \xb2 ", RETROK_2},
	{ " 3 ", " # ", " \xb3 ", RETROK_3},
	{ " 4 ", " $ ", " \xb4 ", RETROK_4},
	{ " 5 ", " % ", " \xb5 ", RETROK_5},
	{ " 6 ", " & ", " \xb6 ", RETROK_6},
	{ " 7 ", " \' ", " \xb7 ", RETROK_7},
	{ " 8 ", " @ ", " \xb8 ", RETROK_8},
	{ " 9 ", " ( ", " \xb9 ", RETROK_9},
	{ " 0 ", " ) ", " \xb0 ", RETROK_0},
	{ "Brk", "Brk", "Brk", RETROK_PAUSE},

	{ "Tab", "Tab", "Tab", RETROK_TAB}, //10+2
	{ " q ", " Q ", "Q \x11", RETROK_q},
	{ " w ", " W ", "W \x17", RETROK_w},
	{ " e ", " E ", "E \x05", RETROK_e},
	{ " r ", " R ", "R \x12", RETROK_r},
	{ " t ", " T ", "T \x14", RETROK_t},
	{ " y ", " Y ", "Y \x19", RETROK_y},
	{ " u ", " U ", "U \x15", RETROK_u},
	{ " i ", " I ", "I \x09", RETROK_i},
	{ " o ", " O ", "O \x0f", RETROK_o},
	{ " p ", " P ", "P \x10", RETROK_p},
	{ "Ret", "Ret", "Ret", RETROK_RETURN},

	{ "ctl", "ctl", "CTL", RETROK_LCTRL}, //20+4
	{ " a ", " A ", "A \x01", RETROK_a},
	{ " s ", " S ", "S \x13", RETROK_s},
	{ " d ", " D ", "D \x04", RETROK_d},
	{ " f ", " F ", "F \x06", RETROK_f},
	{ " g ", " G ", "G \x07", RETROK_g},
	{ " h ", " H ", "H \x08", RETROK_h},
	{ " j ", " J ", "J \x0a", RETROK_j},
	{ " k ", " K ", "K \x0b", RETROK_k},	
	{ " l ", " L ", "L \x0c", RETROK_l},
	{ " ; ", " : ", "; \x7b", RETROK_SEMICOLON},
	{ "Cap", "Cap", "Cap", RETROK_CAPSLOCK},

	{ "shf", "SHF", "shf", RETROK_LSHIFT}, //30+6
	{ " z ", " Z ", "Z \x1a", RETROK_z},
	{ " x ", " X ", "X \x18", RETROK_x},
	{ " c ", " C ", "C \x03", RETROK_c},
	{ " v ", " V ", "V \x16", RETROK_v},
	{ " b ", " B ", "B \x02", RETROK_b},
	{ " n ", " N ", "N \x0e", RETROK_n},
	{ " m ", " M ", "M \x0d", RETROK_m},
	{ " , ", " [ ", ",\x00", RETROK_COMMA},
	{ " . ", " ] ", ". \x60", RETROK_PERIOD},
	{ " / ", " ? ", " / ", RETROK_SLASH},
	{ "Inv", "Inv", "Inv", RETROK_LSUPER},

	{ "PG2", "PG2", "PG2", -2}, //40+8
	{ "Del", "Del", "Del", RETROK_DELETE},
	{ "Spc", "Spc", "Spc", RETROK_SPACE},
	{ "Clr", "Clr", "Clr", RETROK_HOME},
	{ "Ins", "Ins", "Ins", RETROK_INSERT},
	{ " + ", " \\ ", " + ", RETROK_EQUALS},
	{ " _ ", " - ", " _ ", RETROK_HASH},
	{ " = ", " | ", " = ", RETROK_EQUALS},
	{ " * ", " ^ ", " * ", RETROK_ASTERISK},
	{ " < ", " < ", " < ", 0x86},
	{ " > ", " > ", " > ", 0x27},
	{ "Spc", "Spc", "Spc", RETROK_SPACE},


	{ "Esc", "Esc", "Esc", RETROK_ESCAPE},//50+10
	{ " 1 ", " ! ", " 1 ", RETROK_1},
	{ " 2 ", " \" ", " 2 ", RETROK_2},
	{ " 3 ", " # ", " 3 ", RETROK_3},
	{ " 4 ", " $ ", " 4 ", RETROK_4},
	{ " 5 ", " % ", " 5 ", RETROK_5},
	{ " 6 ", " & ", " 6 ", RETROK_6},
	{ " 7 ", " \' ", " 7 ", RETROK_7},
	{ " 8 ", " @ ", " 8 ", RETROK_8},
	{ " 9 ", " ( ", " 9 ", RETROK_9},
	{ " 0 ", " ) ", " 0 ", RETROK_0},
	{ "Brk", "Brk", "Brk", RETROK_PAUSE},

	{ " F7", " F7", " F7", RETROK_F7}, //60+12
	{ " F8", " F8", " F8", RETROK_F8},
	{ " F9", " F9", " F9", RETROK_F9},
	{ " F0", " F0", " F0", RETROK_F10},
	{ " t ", " T ", " t ", 0x63},
	{ " /\\", " /\\", " /\\", 0x00},
	{ " u ", " U ", " u ", 0x52},
	{ " i ", " I ", " i ", 0x43},
	{ " o ", " O ", " o ", 0x42},
	{ " p ", " P ", " p ", 0x33},
	{ " @ ", " | ", " @ ", 0x32},
	{ " [ ", " [ ", " [ ", 0x21},

	{ "STA", "STA", "STA", RETROK_F4}, //70+14
	{ "RES", "RES", "RES", RETROK_F5},
	{ "HLP", "HLP", "HLP", RETROK_F6},
	{ " . ", " . ", " . ", 0x07},
	{ " <-", " <-", " <-", 0x10},
	{ "COP", "COP", "COP", 0x11},
	{ " ->", " ->", " ->", 0x01},
	{ " r ", " R ", " r ", RETROK_r},
	{ " p ", " P ", " p ", RETROK_p},
	{ " = ", " = ", " = ", RETROK_EQUALS},
	{ " * ", " * ", " * ", RETROK_ASTERISK},
	{ " # ", " # ", " # ", RETROK_HASH},

	{ "GUI", "GUI", "GUI", RETROK_F1},//80+16
	{ "OPT", "OPT", "OPT", RETROK_F2},
	{ "SEL", "SEL", "SEL", RETROK_F3},
	{ "Ent", "Ent", "Ent", 0x06},
	{ " b ", " B ", " b ", 0x66},
	{ " \\/", " \\/", " \\/", 0x02},
	{ " m ", " M ", " m ", 0x46},
	{ " , ", " < ", " , ", 0x47},
	{ " . ", " > ", " . ", 0x37},
	{ "TAP", "TAP", "TAP", -8},
	{ "EXI", "EXI", "EXI", -6},
	{ "SNA", "SNA", "SNA", -7},


	{ "PG1", "PG1", "PG1", -2},//90+18
	{ "DSK", "DSK", "DSK", -5},
	{ "MEN", "MEN", "MEN", -13},
	{ "COL", "COL", "COL", -3},
	{ "CTL", "CTL", "CTL", 0x27},
	{ "SPC", "SPC", "SPC", 0x57},
	{ "SHF", "SHF", "SHF", 0x25},
	{ "ESC", "ESC", "ESC", 0x82},
	{ "CLR", "CLR", "CLR", 0x20},
	{ "DEL", "DEL", "DEL", 0x97},
	{ "Ent", "Ent", "Ent", 0x22},
	{ "KBD", "KBD", "KBD", -4},

} ;


#endif
