#define a800 0				// STD_8, STD_16, XEGS_32
#define a800_40 1			// Bounty Bob
#define a800_WILL_64 2		// 64 KB Williams cartridge
#define a800_XE_07_64 3		// XEGS 64 KB cartridge (banks 0-7)
#define a800_XE_128 4		// XEGS 128 KB cartridge
//#define a800_XE_256 4		// XEGS 256 KB cartridge.  Not sure if any exist.
//#define a800_XE_512 5		// XEGS 512 KB cartridge.  Not sure if any exist.
//#define a800_XE_1024 6	// XEGS 1 MB cartridge.  Not sure if any exist.
#define a800_MAX_128 5		// Atarimax 128 KB Flash cartridge
#define a800_MAX_1024 6		// Atarimax 1 MB Flash cartridge (old)
typedef struct {
	int type;
	char name[50];
	int size;
	ULONG crc; 	
} a800_rom;

a800_rom a800_game[] = {

 { a800,"3-D Tic-Tac-Toe",8192,0x1cf50ebe},
 { a800,"Abracadabra!",16384,0xa35c775d},
 { a800_XE_128,"Ace Of Aces",131072,0xebad3ddb},
 { a800,"Activision Decathlon",16384,0x7cc0118b},
 { a800,"Adventure Creator",16384,0x8746d9da},
 { a800_XE_128,"Airball",131072,0xa0ccb3c1},
 { a800,"Alf In Color Caves",16384,0x79df7f9d},
 { a800,"Alien Ambush",8192,0xad7bc30b},
 { a800,"Alien Garden",8192,0x3e27ed0f},
 { a800,"Alpha Shield",8192,0xa7ade454},
 { a800_MAX_1024,"Alternate Reality- The City (MF) (Proto)",1048576,0x52cbd474},
 { a800_MAX_1024,"Alternate Reality- The Dungeon (MF)v9",1048576,0x9fd8d4d3},
 { a800,"Ant Eater",8192,0xc7290722},
 { a800,"Archon- The Light And The Dark",32768,0x2636829f},
 { a800_MAX_128,"Arkanoid I, II, III (MF)",131072,0x70f7bbe5},
 { a800,"Asteroids",8192,0xf9fff4a4},
 { a800,"Astro Chase (First Star Software)",16384,0x18752991},
 { a800,"Astro Chase",16384,0x11f1c7fa},
 { a800,"Atlantis",8192,0xbe0b390c},
 { a800,"Attack At EP-CYG-4",163834,0xdca02ca0},
 { a800,"Attack Of The Mutant Camels",163834,0xc933d741},
 { a800_XE_07_64,"Ballblazer",65536,0x820e5ce5},
 { a800,"Baseball",16384,0x065c3fd2},
 { a800,"Basketball",8192,0x79934851},
 { a800_XE_07_64,"Battlezone",65536,0x692515f2},
 { a800,"BC's Quest For Tires",16384,0xdddc6e36},
 { a800,"Beamrider",16384,0x2b05b8df},
 { a800_MAX_128,"Beyond Castle Wolfenstein (MF)",131926,0xf454689b},
 { a800,"Blue Max",32768,0x003f41ac},
 { a800,"Blaster",16384,0xce1126a2},
 { a800,"Boulderdash",16384,0xaf778329},
 { a800,"Boulders And Bombs",16384,0xab2ec21c},
 { a800_40,"Bounty Bob Strikes Back!",40960,0x0d00f072},
 { a800,"Bristles",16384,0x4263d64d},
 { a800_XE_07_64,"Bruce Lee (Repro)",65536,0x255dfc59},
 { a800,"Buck Rogers - Planet Of Zoom",16384,0x84dd597c},
 { a800,"Captain Beeble",16384,0xad8400b1},
 { a800,"Carnival Massacre (CS)",16384,0xea764851},
 { a800,"Carnival Massacre",16384,0x1baf0c97},
 { a800,"Castle Crisis",32768,0x47acda14},
 { a800,"Castle Hassle",16384,0x5a9e938a},
 { a800,"Castles And Keys",16384,0xbe14c091},
 { a800,"Caverns Of Mars",16384,0x8b9b2f5e},
 { a800,"Centipede",8192,0x44bb1842},
 { a800,"Chess",8192,0x72860db1},
 { a800,"Chicken",8192,0x1ab4d8d8},
 { a800,"Choplifter! (Broderbund)",16384,0x3ebc05ff},
 { a800_XE_07_64,"Choplifter!",65536,0xd426eccc},
 { a800,"Claim Jumper",16384,0x1a333c4a},
 { a800,"Cloudburts",8192,0x259aa18b},
 { a800_XE_128,"Commando",131072,0x28288df4}, // Autodetected as xex. Check afile.c for correct detection hack.
 { a800,"Computer Chess",8192,0x5ce06e94},
 { a800,"Computer War",16384,0x4922aac6},
 { a800_MAX_1024,"Conan (MF)",1048576,0x25feb642},
 { a800,"Congo Bongo",16384,0x7a588045},
 { a800,"Cosmic Life",16384,0x919eaaa9},
 { a800,"Cosmic Tunnels",16384,0xad56c2bf},
 { a800,"Crossfire",8192,0x4d7e0503},
 { a800,"Crystal Castles",32768,0x998fa803},
 { a800,"Dance Fantasy",8192,0xfaec94e7},
 { a800_XE_07_64,"Dark Chambers",65536,0xfa2f132c},
 { a800_XE_07_64,"David's Midnight Magic",65536,0xb7ca61a2},
 { a800,"Defender",16384,0x782a81e4},
 { a800_XE_07_64,"Deflektor",65536,0xe4ed154e},
 { a800,"Deluxe Invaders",8192,0x15dc9b31},
 { a800,"Demo Cart",16384,0x9bedcdf3},
 { a800,"Demon Attack",8192,0x91328072},
 { a800_XE_07_64,"Desert Falcon",65536,0xa8f9324d},
 { a800,"Diamond Mine",16384,0xd8f9b867},
 { a800,"Dig Dug (1983)",16384,0xfdbc57ed},
 { a800,"Dig Dug",16384,0x6d68114e},
 { a800,"Donkey Kong Jr.",16384,0xa4eb70ef},
 { a800,"Donkey Kong",16384,0xf2b76a27},
 { a800,"Dreadnaught Factor",8192,0xc26fbb5b},
 { a800,"Droids",8192,0x5bb0c159},
 { a800,"Ducks Ahoy",16384,0x4cdfceb9},
 { a800_WILL_64,"DynaKillers",65536,0xad050724},
 { a800,"Eastern Front (1941)",16384,0xccff4a03},
 { a800,"Enduro",8192,0xa538a1bb},
 { a800_MAX_128,"Eidolon- The (MF)",131072,0x46f363f7},
 { a800,"Embargo",8192,0x07b1560c},
 { a800,"Espial",16384,0x64946757},
 { a800,"E.T. Phone Home",16384,0xbce4ef51},
 { a800,"Fantastic Voyage",8192,0x7bde2593},
 { a800,"Fast Eddie",8192,0x53ac386a},
 { a800,"Fast Food",8192,0x77223249},
 { a800_XE_128,"Fight Night",131072,0x4440d167},
 { a800,"Final Legacy",16384,0x6bd0d8e4},
 { a800,"Final Orbit",8192,0x177007e9},
 { a800,"Firebird",8192,0xe3c0b5f1},
 { a800,"Flapper",16384,0x18803c52},
 { a800_XE_128,"Flight Simulator II",131072,0x10cfc489},
 { a800,"Flip And Flop",16384,0x8ae057be},
 { a800_MAX_1024,"Flob 1.0.3b",1048576,0xff236e36},
 { a800,"Food Fight",32767,0x4236f0ea},
 { a800,"Fort Apocalypse",16384,0xf79b33f0},
 { a800,"Fortune Hutner",8192,0x03255972},
 { a800,"Frogger II - Threeedeep!",16384,0xed35f49c},
 { a800,"Frogger",8192,0x40e9476c},
 { a800,"Galaxian",8192,0xd60027be},
 { a800,"Gateway To Apshai",16384,0x3a0c6eb0},
 { a800_XE_128,"G.A.T.O.",131072,0xab06a3f5},
 { a800_MAX_128,"Gauntlet (MF)",131072,0x48cface0},
 { a800,"Gold Mine",8192,0x8459b11e},
 { a800,"Gorf",8192,0x90d0c7d7},
 { a800,"Gridrunner",8192,0x752dd5fe},
 { a800,"Gyruss",16384,0x1da47d01},
 { a800,"H.E.R.O.",16384,0x6062d3ce},
 { a800,"Halftime Battlin Bands",16384,0x8d14b5d3},
 { a800_XE_07_64,"Hardball",65536,0xbf0c6df2},
 { a800,"Hypnotic Land",16384,0x4fb75909},
 { a800,"Into The Eagles Nest",32768,0xf31321c2},
 { a800,"James Bond 007",16384,0x19b4e3a1},
 { a800,"Jawbreaker II",8192,0xe2a63a2d},
 { a800_MAX_1024,"Jim Slide XL",1048576,0x5f74de4a},
 { a800,"Journey To The Planets",16384,0xdce59b65},
 { a800,"Joust",16384,0xf6ec618c},
 { a800,"Jumbo Jet Pilot",16384,0xf046332b},
 { a800_MAX_128,"Jumpman",131072,0x4b7beb03},
 { a800,"Jumpman Jr.",16384,0x6c79bbad},
 { a800,"Jungle Hunt",16384,0x1847a7d4},
 { a800,"K-Razy Antiks",8192,0x84ab21b0},
 { a800,"K-razy Kritters (K-Byte)",8192,0x50354927},
 { a800,"K-razy Kritters",8192,0xf854a3b4},
 { a800,"K-razy Shoot-Out (CBS)",8192,0x636a01f5},
 { a800,"K-razy Shoot-Out (K-Byte)",8192,0x4300f6ff},
 { a800,"K-Star Patrol",8192,0x44c71fae},
 { a800,"Kangaroo",16384,0x1ef94906},
 { a800_XE_128,"Karateka",131072,0x97646f16},
 { a800,"Keystone Kapers",8192,0x465e1763},
 { a800,"Kickback",8192,0x2480ed0a},
 { a800_MAX_128,"Koronis Rift (MF)",131072,0xd385b89c},
 { a800_XE_128,"Lode Runner",131072,0x7790f474},
 { a800_MAX_1024,"Lords Of Conquest (MF)",1048576,0x3d6c01bb},
 { a800_MAX_1024,"M.U.L.E. (MF)",1048576,0x0b84f03e},
 { a800,"Major League Hockey",8192,0x4ffbc999},
 { a800_XE_07_64,"Mario Bros. XE",65536,0x7ba07c34},
 { a800,"M.A.S.H.",8192,0xfa041093},
 { a800,"Matterhorn",16384,0xdcc308cf},
 { a800,"Megamania",8192,0xb3c5130c},
 { a800_XE_128,"Mean 18 (Proto)",131072,0x0ee74a89},
 { a800_XE_128,"Midi Maze (Proto)",131072,0x193a53f6},
 { a800,"Millipede",16384,0xfb7e45da},
 { a800,"Miner 2049er",16384,0x6b1478bf},
 { a800,"Missile Command",8192,0xd2e36392},
 { a800,"Missile Command (XEGS built in)",8192,0xbdca01fb},
 { a800,"Mogul Maniac",16384,0x0c391600},
 { a800,"Monster Maze",16384,0x37049e57},
 { a800,"Moon Patrol",16384,0xb845edb8},
 { a800,"Mountain King",8192,0x79748c93},
 { a800,"Mr. Cool",8192,0x1345d10c},
 { a800,"Mr. Do's Castle",8192,0xf1b9a24a},
 { a800,"Mr. TNT",8192,0x701dbdea},
 { a800,"Mrs. Pac-Man",16384,0xf91d18cf},
 { a800_MAX_128,"Mysterious Adventures (MF)",131072,0xadd506c5},
 { a800,"Necromancer",16384,0x39250ff2},
 { a800,"Nightstrike",8192,0x61245a75},
 { a800,"Oil's Well",16384,0x030ecad6},
 { a800,"One-On-One - Dr J. Vs Larry Bird",32768,0xab060567},
 { a800_MAX_1024,"onEscape (121422) (MF)",1048576,0x083fb021},
 { a800,"Orc Attack",16384,0xfbfaefcd},
 { a800,"Ozzy's Orchard",16384,0x1554b983},
 { a800,"Pac-Man",8192,0x61cf6167},
 { a800_MAX_128,"Pac-Man Arcade",131072,0x19bc1482},
 { a800,"Pastfinder",16384,0x12694c3f},
 { a800,"Peanut Butter Panic",8192,0xcdeb7759},
 { a800,"Pengo",16384,0xd8a9fe0a},
 { a800,"Picnic Paranoia",16384,0xe386a621},
 { a800,"Pitfall! II - The Lost Caverns",16384,0x1668cf3b},
 { a800,"Pitfall!",8192,0xb58bdf1c},
 { a800,"Pitstop",16384,0xd49ebf91},
 { a800,"Plattermania",8192,0x6cef6f94},
 { a800,"Pole Position",16384,0x581570C4},
 { a800,"Pool 400",8192,0xa6c2130f},
 { a800,"Popeye",16384,0x00fce79a},
 { a800,"Porky's",16384,0x1733d3fc},
 { a800,"Powerstar",16384,0xdc0dca6e},
 { a800,"Princes And The Frog",8192,0x7ce79281},
 { a800_MAX_1024,"Prince of Persia 211206 (MF)",1048576,0x52df819c},
 { a800,"Protector II",16384,0x374f311f},
 { a800,"Q-bert",8192,0xff3f0472},
 { a800,"Qix",8192,0x967b8051},
 { a800,"Rack'em Up",16384,0x5335d935},
 { a800,"Rally Speedway",16384,0x0a0f6ea2},
 { a800,"Realsports Football",16384,0x5e8951f4},
 { a800,"Realsports Tennis",16384,0x9a34cbdc},
 { a800_XE_07_64,"Rescue On Fractalus",65536,0x1ca549ad},
 { a800,"Risk (Proto)",8192,0x688b0a0c},
 { a800,"River Raid",8192,0x6e601d81},
 { a800,"River Rescue",16384,0xc018c8a0},
 { a800,"Robotron 2084",16384,0x528fc44a},
 { a800,"Satan's Hollow",16384,0x0f7c7934},
 { a800_MAX_128,"Scott Adams Adventures (MF)",131072,0x5446bcb1},
 { a800,"Sea Chase",8192,0x99b5a1dd},
 { a800,"Sea Fox",16384,0x932cc9a8},
 { a800,"Serpentine",8192,0x1b555b41},
 { a800,"Shamus",16384,0xbd3f06ee},
 { a800,"Silicon Warrior",16384,0x0736c8ae},
 { a800,"Slime",16384,0x1babcad6},
 { a800,"Soccer",8192,0x784c7060},
 { a800_MAX_1024,"Space Harrier",1048576,0xca98abfc},
 { a800,"Space Invaders",8192,0x6c811a10},
 { a800,"Space Shuttle",16384,0x66832f68},
 { a800,"Spark Bugs",8192,0xf56eced2},
 { a800,"Speedway Blast",8192,0xf895cc47},
 { a800,"Spider City",8192,0x8f8c3841},
 { a800,"Springer",16384,0x81466b55},
 { a800,"Spy Hunter",16384,0x34df8ffc},
 { a800,"Squish Em",8192,0xe01600b8},
 { a800,"Stargate (Proto)",16384,0xf527b721},
 { a800,"Star Raiders II",32768,0x737d4196},
 { a800,"Star Raiders",8192,0x5ec023ba},
 { a800,"Star Trek - Strategic Operations Simulator",16384,0x9df169d9},
 { a800,"Star Wars - Arcade Game - The",16384,0xaea795f7},
 { a800,"Star Wars - Death Star Battle",16384,0xd9f5cac7},
 { a800,"Starion",16384,0x23faf9b3},
 { a800,"Submarine Commander",16384,0x77198b2b},
 { a800_MAX_1024,"Summer Games (MF)",1048576,0x92325dc3},
 { a800_XE_128,"Summer Games",131072,0x95e6932d},
 { a800,"Super Cobra",8192,0x2af38d2f},
 { a800,"Super Pac-Man (Early Proto)",16384,0xb985be78},
 { a800,"Super Pac-Man (Proto)",16384,0xb518dda8},
 { a800,"Super Zaxxon",16384,0x9e64e13b},
 { a800,"Survival Of The Fittest",8192,0x7f48fbc5},
 { a800_XE_07_64,"Tapper (Cart)",65536,0xeff12440},
 { a800_XE_07_64,"Thunderfox",65536,0xb15ccef2},
 { a800,"Topper",16384,0x0286eea6},
 { a800_XE_07_64,"Tower Toppler (Proto)",65536,0x6b5333bd},
 { a800,"Track And Field",16384,0x9e2484c8},
 { a800,"Turmoil",8192,0xfe48aadf},
 { a800,"Typo Attack",16384,0x89da4ff7},
 { a800,"Typo",8192,0x70585854},
 { a800_MAX_1024,"Ultima II - Revenge of the Enchantress",1048576,0xd2a6db16},
 { a800_MAX_1024,"Ultima III (MF)",1048576,0xb132a1da},
 { a800_MAX_1024,"Ultima IV (MF)",1048576,0x62b1571f},
 { a800,"Up'n Down",16384,0x53ea3bf6},
 { a800,"Wizard Of Wor",16384,0x8017e56a},
 { a800_MAX_128,"World Karate Championship (MF)",131072,0x83c87b17},
 { a800,"Worm War I",8291,0x79934b01},
 { a800_XE_128,"Xenophobe",131072,0x68466666},
 { a800,"Zaxxon",16384,0x21579706},
 { a800,"Zenji",16384,0xebc6ec2e},
 { a800,"Zone Ranger",16384,0x8f1e72e7},
 { -1,"",0,0},
} ;
