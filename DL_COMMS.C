#include <dos.h>
#include "datalog.h"

/***************************************************************************
** GLOBAL EXTERNS
***************************************************************************/
extern struct CHANNELDATA channel;

/******** Setup serial port ********/
initcomport()
{
	unsigned char initbits=0;
	union REGS regs;

	switch (channel.baud) {
		case 110: initbits=initbits | 0x00; break;	/* Baud rates */
		case 150: initbits=initbits | 0x20; break;
		case 300: initbits=initbits | 0x40; break;
		case 600: initbits=initbits | 0x60; break;
		case 1200: initbits=initbits | 0x80; break;
		case 2400: initbits=initbits | 0xA0; break;
		case 4800: initbits=initbits | 0xC0; break;
		case 9600: initbits=initbits | 0xE0; break;
	}
	switch (channel.parity) {
		case 0: initbits=initbits | 0x00; break;	/* None */
		case 1: initbits=initbits | 0x08; break; 	/* Odd  */
		case 2: initbits=initbits | 0x18; break;	/* Even */
	}
	switch (channel.stop) {
		case 1: initbits=initbits | 0x00; break;	/* 1 stop bit  */
		case 2: initbits=initbits | 0x04; break;	/* 2 stop bits */
	}
	switch (channel.data) {
		case 7: initbits=initbits | 0x02; break;	/* 7 data bits */
		case 8: initbits=initbits | 0x03; break;	/* 8 data bits */
	}

	regs.h.ah=0x00;
	regs.h.al=initbits;
	regs.x.dx=channel.port-1;

	int86 (0x14,&regs,&regs);
	return 0;
}

/******** Output a string to the monitor ********/
putstring(char *str) {
	int i=0;
	char outstr[10];
	union REGS regs;
	strcpy (outstr,str);
	strcat (outstr,"\r\n");
	while (outstr[i]) {
		regs.h.ah=0x01;
		regs.h.al=outstr[i];
		regs.x.dx=channel.port-1;
		int86 (0x14,&regs,&regs);
		i++;
	}
	return 0;
}

/******** Get a string from the monitor ********/
char *getstring () {
	char c=0;
	int i=0;
	char outstr[255];
	union REGS regs;
	while (c != 0x0d) {
		regs.h.ah=2;
		regs.x.dx=channel.port-1;
		int86 (0x14,&regs,&regs);
		c=regs.h.al;
		if (c && (c!=0x0d) && (c!=0x0a)) {
			outstr[i++]=c;
		}
	}
	outstr[i]=0;
	return outstr;
}

