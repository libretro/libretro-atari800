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
	{ " 5 ", " %% ", " \xb5 ", RETROK_5},
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
	{ " , ", " [ ", ", \x00", RETROK_COMMA},
	{ " . ", " ] ", ". \x60", RETROK_PERIOD},
	{ " / ", " ? ", " / ", RETROK_SLASH},
	{ "Inv", "Inv", "Inv", RETROK_LSUPER},

	{ "PG2", "PG2", "PG2", -2}, //40+8
	{ "Spc", "Spc", "Spc", RETROK_SPACE},
	{ "Del", "Del", "Del", RETROK_DELETE},
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
	{ "HLP", "HLP", "HLP", RETROK_F6},
	{ "STA", "STA", "STA", RETROK_F4},
	{ "SEL", "SEL", "SEL", RETROK_F3},
	{ "OPT", "OPT", "OPT", RETROK_F2},
	{ "RES", "RES", "RES", RETROK_F5},
	{ " F1", " F1", " F1", RETROK_F7},
	{ " F2", " F2", " F2", RETROK_F8},
	{ " F3", " F3", " F3", RETROK_F9},
	{ " F4", " F4", " F4", RETROK_F10},
	{ "GUI", "GUI", "GUI", RETROK_F1},
	{ "Brk", "Brk", "Brk", RETROK_PAUSE},

	{ "Tab", "Tab", "Tab", RETROK_TAB}, //60+12
	{ " q ", " Q ", "Q \x11", RETROK_q},
	{ " w ", " W ", "W \x17", RETROK_w},
	{ " e ", " E ", "E \x05", RETROK_e},
	{ " r ", " R ", "R \x12", RETROK_r},
	{ " \x1c ", " \x1c ", " \x1c ", RETROK_UP},
	{ " y ", " Y ", "Y \x19", RETROK_y},
	{ " u ", " U ", "U \x15", RETROK_u},
	{ " i ", " I ", "I \x09", RETROK_i},
	{ " o ", " O ", "O \x0f", RETROK_o},
	{ " p ", " P ", "P \x10", RETROK_p},
	{ "Ret", "Ret", "Ret", RETROK_RETURN},

	{ "ctl", "ctl", "CTL", RETROK_LCTRL}, //70+14
	{ " a ", " A ", "A \x01", RETROK_a},
	{ " s ", " S ", "S \x13", RETROK_s},
	{ " d ", " D ", "D \x04", RETROK_d},
	{ " \x1e ", " \x1e ", " \x1e ", RETROK_LEFT},
	{ "Ret", "Ret", "Ret", RETROK_RETURN},
	{ " \x1f ", " \x1f ", " \x1f ", RETROK_RIGHT},
	{ " j ", " J ", "J \x0a", RETROK_j},
	{ " k ", " K ", "K \x0b", RETROK_k},	
	{ " l ", " L ", "L \x0c", RETROK_l},
	{ " ; ", " : ", "; \x7b", RETROK_SEMICOLON},
	{ "Cap", "Cap", "Cap", RETROK_CAPSLOCK},

	{ "shf", "SHF", "shf", RETROK_LSHIFT}, //80+16
	{ " z ", " Z ", "Z \x1a", RETROK_z},
	{ " x ", " X ", "X \x18", RETROK_x},
	{ " c ", " C ", "C \x03", RETROK_c},
	{ " v ", " V ", "V \x16", RETROK_v},
	{ " \x1d ", " \x1d ", " \x1d ", RETROK_DOWN},
	{ " n ", " N ", "N \x0e", RETROK_n},
	{ " m ", " M ", "M \x0d", RETROK_m},
	{ " , ", " [ ", ", \x00", RETROK_COMMA},
	{ " . ", " ] ", ". \x60", RETROK_PERIOD},
	{ " / ", " ? ", " / ", RETROK_SLASH},
	{ "Inv", "Inv", "Inv", RETROK_LSUPER},


	{ "PG1", "PG1", "PG1", -2},//90+18
	{ "Spc", "Spc", "Spc", RETROK_SPACE},
	{ "Del", "Del", "Del", RETROK_DELETE},
	{ "Clr", "Clr", "Clr", RETROK_HOME},
	{ "Ins", "Ins", "Ins", RETROK_INSERT},
	{ " + ", " \\ ", " + ", RETROK_EQUALS},
	{ " _ ", " - ", " _ ", RETROK_HASH},
	{ " = ", " | ", " = ", RETROK_EQUALS},
	{ " * ", " ^ ", " * ", RETROK_ASTERISK},
	{ " < ", " < ", " < ", 0x86},
	{ " > ", " > ", " > ", 0x27},
	{ "Spc", "Spc", "Spc", RETROK_SPACE},

} ;


#endif
