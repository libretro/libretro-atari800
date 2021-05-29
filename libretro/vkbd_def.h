#ifndef VKBD_DEF_H
#define VKBD_DEF_H 1

typedef struct {
	char norml[NLETT];
	char shift[NLETT];
	char ctrl[NLETT];
	int val;	
} Mvk;

// I left 2 pages of Virtual Keyboard allowed to work, but everything now fits on one.
Mvk MVk[NPLGN*NLIGN*2]={

	{ "HLP", "HLP", "HLP", RETROK_F6}, // page 1, line 1
	{ "STA", "STA", "STA", RETROK_F4},
	{ "SEL", "SEL", "SEL", RETROK_F3},
	{ "OPT", "OPT", "OPT", RETROK_F2},
	{ "RES", "RES", "RES", RETROK_F5},
	{ " F1", " F1", " F1", RETROK_F7},
	{ " F2", " F2", " F2", RETROK_F8},
	{ " F3", " F3", " F3", RETROK_F9},
	{ " F4", " F4", " F4", RETROK_F10},
	{ "Clr", "Clr", "Clr", RETROK_HOME},
	{ "Ins", "Ins", "Ins", RETROK_INSERT},
	{ "Bsp", "Del", "Del", RETROK_BACKSPACE},

	{ "Esc", "Esc", "Esc", RETROK_ESCAPE}, // page 1, line 2
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

	{ "Tab", "Tab", "Tab", RETROK_TAB},  // page 1, line 3
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

	{ "Ctl", "Ctl", "Ctl", RETROK_LCTRL},  // page 1, line 4
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

	{ "Shf", "Shf", "Shf", RETROK_LSHIFT},  // page 1, line 5
	{ " z ", " Z ", "Z \x1a", RETROK_z},
	{ " x ", " X ", "X \x18", RETROK_x},
	{ " c ", " C ", "C \x03", RETROK_c},
	{ " v ", " V ", "V \x16", RETROK_v},
	{ " b ", " B ", "B \x02", RETROK_b},
	{ " n ", " N ", "N \x0e", RETROK_n},
	{ " m ", " M ", "M \x0d", RETROK_m},
	{ " , ", " [ ", ", \x00", RETROK_COMMA},
	{ " . ", " ] ", ". \x60", RETROK_PERIOD},
	{ " \x9c ", " \x9c ", " \x9c ", RETROK_UP},
	{ "Inv", "Inv", "Inv", RETROK_LSUPER},

	{ "OPC", "OPC", "OPC", -5}, // page 1, line 6
	{ "Spc", "Spc", "Spc", RETROK_SPACE},
	{ " + ", " \\ ", " \x1e ", RETROK_PLUS},
	{ " _ ", " - ", " \x1c ", RETROK_UNDERSCORE},
	{ " = ", " | ", " \x1d ", RETROK_EQUALS},
	{ " * ", " ^ ", " \x1f ", RETROK_ASTERISK},
	{ " / ", " ? ", " / ", RETROK_SLASH},
	{ " < ", " < ", " < ", RETROK_LESS},
	{ " > ", " > ", " > ", RETROK_GREATER},
	{ " \x9e ", " \x9e ", " \x9e ", RETROK_LEFT},
	{ " \x9d ", " \x9d ", " \x9d ", RETROK_DOWN},
	{ " \x9f ", " \x9f ", " \x9f ", RETROK_RIGHT},


	{ "Esc", "Esc", "Esc", RETROK_ESCAPE}, // page 2, line 1
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

	{ "Esc", "Esc", "Esc", RETROK_ESCAPE}, // page 2, line 2
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

	{ "Tab", "Tab", "Tab", RETROK_TAB}, // page 2, line 3
	{ " q ", " Q ", "Q \x11", RETROK_q},
	{ " w ", " W ", "W \x17", RETROK_w},
	{ " e ", " E ", "E \x05", RETROK_e},
	{ " r ", " R ", "R \x12", RETROK_r},
	{ " \x9c ", " \x9c ", " \x9c ", RETROK_UP},
	{ " y ", " Y ", "Y \x19", RETROK_y},
	{ " u ", " U ", "U \x15", RETROK_u},
	{ " i ", " I ", "I \x09", RETROK_i},
	{ " o ", " O ", "O \x0f", RETROK_o},
	{ " p ", " P ", "P \x10", RETROK_p},
	{ "Ret", "Ret", "Ret", RETROK_RETURN},

	{ "Ctl", "Ctl", "Ctl", RETROK_LCTRL}, // page 2, line 4
	{ " a ", " A ", "A \x01", RETROK_a},
	{ " s ", " S ", "S \x13", RETROK_s},
	{ " d ", " D ", "D \x04", RETROK_d},
	{ " \x9e ", " \x9e ", " \x9e ", RETROK_LEFT},
	{ "Ret", "Ret", "Ret", RETROK_RETURN},
	{ " \x9f ", " \x9f ", " \x9f ", RETROK_RIGHT},
	{ " j ", " J ", "J \x0a", RETROK_j},
	{ " k ", " K ", "K \x0b", RETROK_k},	
	{ " l ", " L ", "L \x0c", RETROK_l},
	{ " ; ", " : ", "; \x7b", RETROK_SEMICOLON},
	{ "Cap", "Cap", "Cap", RETROK_CAPSLOCK},

	{ "Shf", "Shf", "Shf", RETROK_LSHIFT}, // page 2, line 5
	{ " z ", " Z ", "Z \x1a", RETROK_z},
	{ " x ", " X ", "X \x18", RETROK_x},
	{ " c ", " C ", "C \x03", RETROK_c},
	{ " v ", " V ", "V \x16", RETROK_v},
	{ " \x9d ", " \x9d ", " \x9d ", RETROK_DOWN},
	{ " n ", " N ", "N \x0e", RETROK_n},
	{ " m ", " M ", "M \x0d", RETROK_m},
	{ " , ", " [ ", ", \x00", RETROK_COMMA},
	{ " . ", " ] ", ". \x60", RETROK_PERIOD},
	{ " / ", " ? ", " / ", RETROK_SLASH},
	{ "Inv", "Inv", "Inv", RETROK_LSUPER},

	{ "PG1", "PG1", "PG1", -2}, // page 2, line 6
	{ "Spc", "Spc", "Spc", RETROK_SPACE},
	{ "Del", "Del", "Del", RETROK_DELETE},
	{ "Clr", "Clr", "Clr", RETROK_HOME},
	{ "Ins", "Ins", "Ins", RETROK_INSERT},
	{ " + ", " \\ ", " \x1e ", RETROK_PLUS},
	{ " _ ", " - ", " \x1c ", RETROK_UNDERSCORE},
	{ " = ", " | ", " \x1d ", RETROK_EQUALS},
	{ " * ", " ^ ", " \x1f ", RETROK_ASTERISK},
	{ " < ", " < ", " < ", RETROK_LESS},
	{ " > ", " > ", " > ", RETROK_GREATER},
	{ "PG1", "PG1", "PG1", -2},

} ;


#endif
