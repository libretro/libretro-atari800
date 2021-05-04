#ifndef VKBD_DEF_H
#define VKBD_DEF_H 1

typedef struct {
	char norml[NLETT];
	char shift[NLETT];
	int val;	
} Mvk;

Mvk MVk[NPLGN*NLIGN*2]={

	{ "ESC" ,"ESC"  ,RETROK_ESCAPE },//0
	{ " 1" ," !" , RETROK_1 },//0
	{ " 2" ," \"" ,RETROK_2 },
	{ " 3" ," #"  ,RETROK_3 },
	{ " 4" ," $"  ,RETROK_4 },
	{ " 5" ," %"  ,RETROK_5 },
	{ " 6" ," &"  ,RETROK_6 },
	{ " 7" ," \'"  ,RETROK_7 },
	{ " 8" ," ("  ,RETROK_8 },
	{ " 9" ," )"  ,RETROK_9 },
	{ " 0" ," _"  ,RETROK_0 },
	{ " ^" ,"Pnd"  ,0x30 },

	{ " q" ," Q"  ,RETROK_q}, //10+2
	{ " w" ," W"  ,RETROK_w},
	{ " e" ," E"  ,RETROK_e},
	{ " r" ," R"  ,RETROK_r},
	{ " t" ," T"  ,RETROK_t},
	{ " y" ," Y"  ,RETROK_y},
	{ " u" ," U"  ,RETROK_u},
	{ " i" ," I"  ,RETROK_i},
	{ " o" ," O"  ,RETROK_o},
	{ " p" ," P"  ,RETROK_p},
	{ " @" ," |"  ,RETROK_QUOTEDBL},
	{ " [" ," ["  ,RETROK_LEFTBRACKET},

	{ " a" ," A"  ,RETROK_a}, //20+4
	{ " s" ," S"  ,RETROK_s},
	{ " d" ," D"  ,RETROK_d},
	{ " f" ," F"  ,RETROK_f},
	{ " g" ," G"  ,RETROK_g},
	{ " h" ," H"  ,RETROK_h},
	{ " j" ," J"  ,RETROK_j},
	{ " k" ," K"  ,RETROK_k},	
	{ " l" ," L"  ,RETROK_l},
	{ " :" ," *"  ,RETROK_COLON},
	{ " ;" ," +"  ,RETROK_SEMICOLON},
	{ " ]" ," ]"  ,RETROK_RIGHTBRACKET},

	{ " z" ," Z"  ,RETROK_z},//30+6
	{ " x" ," X"  ,RETROK_x},
	{ " c" ," C"  ,RETROK_c},
	{ " v" ," V"  ,RETROK_v},
	{ " b" ," B"  ,RETROK_b},
	{ " n" ," N"  ,RETROK_n},
	{ " m"," M"   ,RETROK_m},
	{ " ,"," <"   ,RETROK_COMMA},
	{ " ."," >"   ,RETROK_PERIOD},
	{ " /" ," ?"  ,RETROK_SLASH},
	{ " \\"," \\"   ,RETROK_BACKSLASH},
	{ "SHFT" ,"SHFT"  ,0x25},

	{ "PG2","PG2" ,-2}, //40+8
	{ "TAB","TAB" ,RETROK_TAB},
	{ "CPSL" ,"CPSL"  ,0x86},
	{ "RET" ,"RET"  ,RETROK_RETURN},
	{ "DEL" ,"DEL"  ,0x97},
	{ "CTRL" ,"CTRL"  ,0x27},
	{ "CLR" ,"CLR" , 0x20},
	{ "Spc" ,"Spc",RETROK_SPACE},
	{ "= " ,"= "  ,RETROK_EQUALS},
	{ " *" ," *",  RETROK_ASTERISK},
	{ "# " ,"# "  ,RETROK_HASH},
	{ "Ent" ,"Ent",0x06},


	{ "ESC" ,"ESC"  ,RETROK_ESCAPE },//50+1°
	{ " 1" ," !" , RETROK_1 },//0
	{ " 2" ," \"" ,RETROK_2 },
	{ " 3" ," #"  ,RETROK_3 },
	{ " 4" ," $"  ,RETROK_4 },
	{ " 5" ," %"  ,RETROK_5 },
	{ " 6" ," &"  ,RETROK_6 },
	{ " 7" ," \'"  ,RETROK_7 },
	{ " 8" ," ("  ,RETROK_8 },
	{ " 9" ," )"  ,RETROK_9 },
	{ " 0" ," _"  ,RETROK_0 },
	{ " ^" ,"Pnd"  ,0x30 },

	{ " F7" ," F7"  ,RETROK_F7}, //60+12
	{ " F8" ," F8"  ,RETROK_F8},
	{ " F9" ," F9"  ,RETROK_F9},
	{ " F0" ," F0"  ,RETROK_F10},
	{ " t" ," T"  ,0x63},
	{ " /\\" ," /\\"  ,0x00},
	{ " u" ," U"  ,0x52},
	{ " i" ," I"  ,0x43},
	{ " o" ," O"  ,0x42},
	{ " p" ," P"  ,0x33},
	{ " @" ," |"  ,0x32},
	{ " [" ," ["  ,0x21},

	{ "STA" ,"STA"  ,RETROK_F4}, //70+14
	{ "RES." ,"RES."  ,RETROK_F5},
	{ "HELP" ,"HELP"  ,RETROK_F6},
	{ " ." ," ."  ,0x07},
	{ " <-" ," <-"  ,0x10},
	{ "COPY" ,"COPY"  ,0x11},
	{ " ->" ," ->"  ,0x01},
	{ " r" ," R"  ,RETROK_r},
	{ " p" ," P"  ,RETROK_p},
	{ "= " ,"= "  ,RETROK_EQUALS},
	{ " *" ," *",  RETROK_ASTERISK},
	{ "# " ,"# "  ,RETROK_HASH},

	{ "MENU" ,"MENU"  ,RETROK_F1},//80+16
	{ "OPT" ,"OPT"  ,RETROK_F2},
	{ "SEL" ,"SEL"  ,RETROK_F3},
	{ "Ent" ,"Ent"  ,0x06},
	{ " b" ," B"  ,0x66},
	{ " \\/" ," \\/"  ,0x02},
	{ " m"," M"   ,0x46},
	{ " ,"," <"   ,0x47},
	{ " ."," >"   ,0x37},
	{ "TAPE" ,"TAPE"  ,-8},
	{ "EXIT","EXIT"   ,-6},
	{ "SNA" ,"SNA"  ,-7},


	{ "PG1","PG1"  ,-2},//90+18
	{ "DSK","DSK"  ,-5},
	{ "GUI","GUI"  ,-13},
	{ "COL" ,"COL",-3},
	{ "CTRL" ,"CTRL" ,0x27},
	{ "SPC" ,"SPC" ,0x57},
	{ "SHFT" ,"SHFT" ,0x25},
	{ "ESC","ESC",0x82},
	{ "CLR" ,"CLR",0x20},
	{ "DEL" ,"DEL",0x97},
	{ "Ent" ,"Ent",0x22},
	{ "KBD" ,"KBD",-4},

} ;


#endif
