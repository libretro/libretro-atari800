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
	{ " 1", " !", " 1", RETROK_1},
	{ " 2", " \"", " 2", RETROK_2},
	{ " 3", " #", " 3", RETROK_3},
	{ " 4", " $", " 4", RETROK_4},
	{ " 5", " \%", " 5", RETROK_5},
	{ " 6", " &", " 6", RETROK_6},
	{ " 7", " \'", " 7", RETROK_7},
	{ " 8", " @", " 8", RETROK_8},
	{ " 9", " (", " 9", RETROK_9},
	{ " 0", " )", " 0", RETROK_0},
	{ "Brk", "Brk", "Brk", RETROK_PAUSE},

	{ "Tab", "Tab", "Tab", RETROK_TAB}, //10+2
	{ " q", " Q", " q", RETROK_q},
	{ " w", " W", " w", RETROK_w},
	{ " e", " E", " e", RETROK_e},
	{ " r", " R", " r", RETROK_r},
	{ " t", " T", " t", RETROK_t},
	{ " y", " Y", " y", RETROK_y},
	{ " u", " U", " u", RETROK_u},
	{ " i", " I", " i", RETROK_i},
	{ " o", " O", " o", RETROK_o},
	{ " p", " P", " p", RETROK_p},
	{ "Ret", "Ret", "Ret", RETROK_RETURN},

	{ "ctrl", "ctrl", "CTRL", RETROK_LCTRL}, //20+4
	{ " a", " A", " a", RETROK_a},
	{ " s", " S", " s", RETROK_s},
	{ " d", " D", " d", RETROK_d},
	{ " f", " F", " f", RETROK_f},
	{ " g", " G", " g", RETROK_g},
	{ " h", " H", " h", RETROK_h},
	{ " j", " J", " j", RETROK_j},
	{ " k", " K", " k", RETROK_k},	
	{ " l", " L", " l", RETROK_l},
	{ " ;", " :", " ;", RETROK_SEMICOLON},
	{ "Cap", "Cap", "Cap", RETROK_CAPSLOCK},

	{ "shft", "SHFT", "shft", RETROK_LSHIFT}, //30+6
	{ " z", " Z", " z", RETROK_z},
	{ " x", " X", " x", RETROK_x},
	{ " c", " C", " c", RETROK_c},
	{ " v", " V", " v", RETROK_v},
	{ " b", " B", " b", RETROK_b},
	{ " n", " N", " n", RETROK_n},
	{ " m", " M", " m", RETROK_m},
	{ " ,", " [", " ,", RETROK_COMMA},
	{ " .", " ]", " .", RETROK_PERIOD},
	{ " /", " ?", " /", RETROK_SLASH},
	{ "Inv", "Inv", "Inv", RETROK_LSUPER},

	{ "PG2", "PG2", "PG2", -2}, //40+8
	{ "Del", "Del", "Del", RETROK_DELETE},
	{ "Spc", "Spc", "Spc", RETROK_SPACE},
	{ "Clr", "Clr", "Clr", RETROK_HOME},
	{ "Ins", "Ins", "Ins", RETROK_INSERT},
	{ " +", " \\", " +", RETROK_EQUALS},
	{ " _", " -", " _", RETROK_HASH},
	{ " =", " |", " =", RETROK_EQUALS},
	{ " *", " ^", " *", RETROK_ASTERISK},
	{ " <", " <", " <", 0x86},
	{ " >", " >", " >", 0x27},
	{ "Spc", "Spc", "Spc", RETROK_SPACE},


	{ "Esc", "Esc", "Esc", RETROK_ESCAPE},//50+10
	{ " 1", " !", " 1", RETROK_1},
	{ " 2", " \"", " 2", RETROK_2},
	{ " 3", " #", " 3", RETROK_3},
	{ " 4", " $", " 4", RETROK_4},
	{ " 5", " \%", " 5", RETROK_5},
	{ " 6", " &", " 6", RETROK_6},
	{ " 7", " \'", " 7", RETROK_7},
	{ " 8", " @", " 8", RETROK_8},
	{ " 9", " (", " 9", RETROK_9},
	{ " 0", " )", " 0", RETROK_0},
	{ "Brk", "Brk", "Brk", RETROK_PAUSE},

	{ " F7", " F7", " F7", RETROK_F7}, //60+12
	{ " F8", " F8", " F8", RETROK_F8},
	{ " F9", " F9", " F9", RETROK_F9},
	{ " F0", " F0", " F0", RETROK_F10},
	{ " t", " T", " t", 0x63},
	{ " /\\", " /\\", " /\\", 0x00},
	{ " u", " U", " u", 0x52},
	{ " i", " I", " i", 0x43},
	{ " o", " O", " o", 0x42},
	{ " p", " P", " p", 0x33},
	{ " @", " |", " @", 0x32},
	{ " [", " [", " [", 0x21},

	{ "STA", "STA", "STA", RETROK_F4}, //70+14
	{ "RES.", "RES.", "RES.", RETROK_F5},
	{ "HELP", "HELP", "HELP", RETROK_F6},
	{ " .", " .", " .", 0x07},
	{ " <-", " <-", " <-", 0x10},
	{ "COPY", "COPY", "COPY", 0x11},
	{ " ->", " ->", " ->", 0x01},
	{ " r", " R", " r", RETROK_r},
	{ " p", " P", " p", RETROK_p},
	{ "= ", "= ", " =", RETROK_EQUALS},
	{ " *", " *", " *", RETROK_ASTERISK},
	{ "# ", "# ", " #", RETROK_HASH},

	{ "MENU", "MENU", "MENU", RETROK_F1},//80+16
	{ "OPT", "OPT", "OPT", RETROK_F2},
	{ "SEL", "SEL", "SEL", RETROK_F3},
	{ "Ent", "Ent", "Ent", 0x06},
	{ " b", " B", " b", 0x66},
	{ " \\/", " \\/", " \\/", 0x02},
	{ " m", " M", " m", 0x46},
	{ " ,", " <", " ,", 0x47},
	{ " .", " >", " .", 0x37},
	{ "TAPE", "TAPE", "TAPE", -8},
	{ "EXIT", "EXIT", "EXIT", -6},
	{ "SNA", "SNA", "SNA", -7},


	{ "PG1", "PG1", "PG1", -2},//90+18
	{ "DSK", "DSK", "DSK", -5},
	{ "GUI", "GUI", "GUI", -13},
	{ "COL", "COL", "COL", -3},
	{ "CTRL", "CTRL", "CTRL", 0x27},
	{ "SPC", "SPC", "SPC", 0x57},
	{ "SHFT", "SHFT", "SHFT", 0x25},
	{ "ESC", "ESC", "ESC", 0x82},
	{ "CLR", "CLR", "CLR", 0x20},
	{ "DEL", "DEL", "DEL", 0x97},
	{ "Ent", "Ent", "Ent", 0x22},
	{ "KBD", "KBD", "KBD", -4},

} ;


#endif
