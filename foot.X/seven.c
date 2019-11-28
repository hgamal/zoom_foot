#include "seven.h"

unsigned char to7(unsigned char value)
{
// #    PGFE DCBA	Anode Cathode
// 0 	1100 0000 	C0	3F
// 1 	1111 1001 	F9	06
// 2 	1010 0100 	A4	5B
// 3 	1011 0000 	B0	4F
// 4 	1001 1001 	99	66
// 5 	1001 0010 	92	6D
// 6 	1000 0010 	82	7D
// 7 	1111 1000 	F8	07
// 8 	1000 0000 	80	7f
// 9 	1001 1000 	98	67
// A	1111 0111	F7	08
// B	1111 1100	FC	03
// C	1011 1001	B9	46
// D	1101 1110	DE	21
// E	1111 1001	F9	06
// F	1111 0001	F1	1E

	switch (value) {
#ifdef COMMON_CATHODE
		case 0:
			return 0x3f; //return 0xc0;
		case 1:
			return 0x06; //return 0xf9;
		case 2:
			return 0x5b; //return 0xa4;
		case 3:
			return 0x4f; //return 0xb0;
		case 4:
			return 0x66; //return 0x99;
		case 5:
			return 0x6d; //return 0x92;
		case 6:
			return 0x7d; //return 0x82;
		case 7:
			return 0x07; //return 0xf8;
		case 8:
			return 0x7f; //return 0x80;
		case 9:
			return 0x67; //return 0x98;
		case 10:
			return 0x77; //return 0x08;
		case 11:
			return 0x7c; //return 0x03
		case 12:
			return 0x39; //return 0x46
		case 13:
			return 0x5e; //return 0x21;
		case 14:
			return 0x79; //return 0x06;
		case 15:
			return 0x71; //return 0x1e;
		default:
			return 0;
#else
		case 0:
			return 0xc0;
		case 1:
			return 0xf9;
		case 2:
			return 0xa4;
		case 3:
			return 0xb0;
		case 4:
			return 0x99;
		case 5:
			return 0x92;
		case 6:
			return 0x82;
		case 7:
			return 0xf8;
		case 8:
			return 0x80;
		case 9:
			return 0x98;
		case 10:
			return 0x88;
		case 11:
			return 0x83;
		case 12:
			return 0xc6;
		case 13:
			return 0xa1;
		case 14:
			return 0x86;
		case 15:
			return 0x8e;
		defautl:
			return 0xff;
#endif
	}
	return 0;
}


