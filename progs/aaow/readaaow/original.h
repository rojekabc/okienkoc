/*
 * 0000 - 0018 Title
 * 0019 - 0120 Memo
 * 0121 - 0137 ??? (ustawienia)
 * 0138 - 02C7 Map
 * 02C8 - 0457 Mapa frontu i zniszczen
 *     2 najstarsze bity mówi¹ o zniszczeniach - 00 brak/01 male/02 duze/03 zniszczone
 *     2 kolejne bity mowi¹ o froncie 01 red/02 green/03 neutral
 *     4 ostatnie bity, s¹ ustawione na 6 jesli jest to front (nieznane zastosowanie)
 * 0458 - 04BB polozenia pilotow
 * 04BC - 04ED strona pilota (1 green 2 red)
 * 04EE - 051F rodzaj samolotu (1 mysliwiec 2 bombowiec)
 * 0520 - 0551 Odznaczenia pilotów 1E to wszystkie mozliwe odznaczenia - 6 odznaczen (wartosci co 5, czyli zapewne po 5 zaslugach
 *		nastepuje przypisanie medalu)
 * 0552 - 0907 Piloci [jeden pilot po 16 znakow/moze 20]
 *
 * Maksymalna liczba samolotow 50
 *
 *
 * Ustawienia:
 * Play green/red side
 * Enemy seen always/when close/when far
 * Aircraft repair rate is fast/slow/medium
 * Target repair rate is fast/slow/medium
 * Mountain are low/high
 * Your Aircraft range is long/short/medium
 * Your factories build fighters often/never/seldom
 * Your factories build bombers often/never/seldom
 * Enemy Aircraft range is long/short/medium
 * Enemy factories build fighters often/never/seldom
 * Enemy factories build bombers often/never/seldom
 *
 * Oponents with different strategy (fisrt oponent/second oponent)
 * 
 */

// zidentyfikowane elementy - position
#define DATATITLE 0x0000
#define TITLELENGTH 24
#define DATAMEMO 0x0019
#define MEMOLINELENGTH 24
#define MEMOLINESAMMOUNT 11
#define DATAMAP 0x0138
#define DATADMG 0x02C8
#define MAPHEIGHT 20
#define MAPWIDTH 20
#define DATAPILOT 0x0552
#define PILOTNAMELENGHT 19
#define PILOTMAX 50
#define DATAORDERS 0x0520
#define DATAPOS 0x458
#define DATASIDE 0x04BC
#define DATATYPE 0x04EE

#define GREEN_FLAG 13
#define RED_FLAG 12
#define GREEN_BASE 4
#define RED_BASE 3


// known map definitions and convertion it to character
char mapDefs[] = {
	// city - b 0
	'c', 'c', 'c',
	// red capital - b 1
	'r',
	// green capital - b 2
	'g',
	// airbase - b 3
	'a',
	// factory - b 4
	'f',
	// supply depot - b 5
	's',
	// fort - b 6
	'F',
	// village - b 7
	'v',
	// bridge - b 8, 9
	'-', '|',
	// red side flag - none
	'$',
	// green side flag - none
	'#',
	//compass - none
	'+',
	// forrest (12) - t 0
	'T', 'T',  'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T',
	// forrest-clear - t 0
	't', 't', 't',
	// clear with single tree (8) - t 1
	'.', '.', '.', '.', '.', '.', '.', '.', 
	// clear - t 1
	' ', ' ', ' ',
	// river (19) - t 2
	'=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', '=', 
	// mountain (15) - t 3
	'^', '^', '^', '^', '^', '^', '^', '^', '^', '^', '^', '^', '^', '^',
	// damaged clear - none
	'+', '+', '+',
	// Damages forest - none
	'*', '*', '*',
	// Unknown - none
	'X', 'X', 'X', 'X',
	// Damages forest - none
	'*'

}; // 00 - 55
// d 0, 1, 2, 3
char dmgDefs[] = {
	' ', '.', '+', '*'
};

// f 0, 1, 2, 3
char frontDefs[] = {
	' ', 'R', 'G', '|'
};

// d - 2b (damages)
// f - 2b (front line)
// t - 2b (terrain)
// b - 4b (buildings)
//
// definicje mapy dla przeksztalcenia
// 0x00 - trees
// 0x01 - clear terrain
// 0x02 - river
// 0x03 - mountains
//
unsigned char terrainDataDefs[] = {
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 10 clear
	0x02, 0x02, // 2 river
	0x01, 0x01, 0x01, // 3 clear
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 15 forest
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 11 clear
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, // 19 river
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, // 15 mountain
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 // 11 clear
};

unsigned char buildDataDefs[] = {
	0x03, 0x03, 0x03, // city
	0x07, 0x07, // capital (red/green)
	0x04, // airport
	0x02, // factory
	0x01, // supply
	0x06, // fort
	0x05, // village
	0x08, // bridge horizontal
	0x09 // bridge vertical
};

