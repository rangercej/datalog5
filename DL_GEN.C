#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <dos.h>
#include "datalog.h"

/***************************************************************************
** GLOBAL EXTERNS
***************************************************************************/
extern struct CURSORDATA curs;
extern char scratch[500];

/***************************************************************************
** GENERAL ROUTINES
***************************************************************************/

/******** Draw a box on the screen ********/
box(left,top,right,bottom,vlines,hlines,color,shadow)
int left,top,right,bottom,vlines,hlines,color,shadow;
{
	char vchar,hchar,tlchar,trchar,blchar,brchar;
	char outstr[81];
	struct {
		char ch;
		char attr;
	} far *scr;
	struct text_info txtinfo;
	int i;

	scr=MK_FP(0xB800,0);
	gettextinfo (&txtinfo);
	memset(outstr,0x20,80);
	outstr[right-left+1]=0;

	vchar=(vlines==SINGLE) ? 179:186;
	hchar=(hlines==SINGLE) ? 196:205;


	tlchar=(vlines==hlines) ? (vlines==SINGLE) ? 218:201:(vlines==SINGLE) ? 213:214;
	trchar=(vlines==hlines) ? (vlines==SINGLE) ? 191:187:(vlines==SINGLE) ? 184:183;
	blchar=(vlines==hlines) ? (vlines==SINGLE) ? 292:200:(vlines==SINGLE) ? 212:211;
	brchar=(vlines==hlines) ? (vlines==SINGLE) ? 217:188:(vlines==SINGLE) ? 190:189;

	textcolor (color);

	strnset (outstr,hchar,right-left);
	outstr[0]=tlchar;
	outstr[right-left]=trchar;
	gotoxy (left,top);
	cputs (outstr);

	outstr[0]=blchar;
	outstr[right-left]=brchar;
	gotoxy (left,bottom);
	cputs (outstr);

	strnset (outstr,0x20,right-left);
	outstr[0]=vchar;
	outstr[right-left]=vchar;
	for (i=(top+1); i < bottom; i++) {
		gotoxy (left,i);
		cputs (outstr);
	}

	if (shadow) {
		textcolor (DARKGRAY);
		textbackground (BLACK);
		for (i=0; i < right-left+1; i++) {
			scr[1+left+i+(bottom*80)].attr=DARKGRAY;
		}
		for (i=0; i < bottom-top+1; i++) {
			scr[right+((top+i)*80)].attr=DARKGRAY;
			scr[1+right+((top+i)*80)].attr=DARKGRAY;
		}
	}

	textattr (txtinfo.attribute);

	return 0;
}


/******** Center text on the screen ********/
center (line,string,color)
int line,color;
char *string;
{
	gotoxy (40-(strlen(string)/2),line);
	textcolor (color);
	cputs (string);

	return 0;
}


/******** Switch cursor off ********/
cursoroff()
{
	struct REGPACK cpuregs;

	if (curs.visible) {
		cpuregs.r_ax=0x0300;
		intr (0x10,&cpuregs);
		curs.size=cpuregs.r_cx;

		cpuregs.r_cx=0x2000;
		cpuregs.r_ax=0x0100;
		intr (0x10,&cpuregs);

		curs.visible=FALSE;
	}
	return 0;
}

/******** Switch cursor on ********/
cursoron()
{
	struct REGPACK cpuregs;

	if (!curs.visible) {
		cpuregs.r_ax=0x0100;
		cpuregs.r_cx=curs.size;
		intr (0x10,&cpuregs);

		curs.visible=TRUE;
	}
	return 0;
}

/******** Take a snapshot of the screen ********/
char *savescreen()
{
	char *buff;

	buff=malloc (4000);
	if (buff==NULL) {
		textcolor (LIGHTGRAY);
		textbackground (BLACK);
		window (1,1,80,25);
		clrscr();
		puts ("Out of memory - terminating program");
		printf ("Memory free: %u\n\r",coreleft());
		puts ("Please check your computer setup.");
		puts ("If you suspect this could be a bug, contact the author. See");
		puts ("handbook for details.");
		abort();
	}
	movedata(0xB800,0,FP_SEG(buff),FP_OFF(buff),4000);
	return buff;
}

/******** Put a saved screen back ********/
putscreen (buff)
char *buff;
{
	movedata(FP_SEG(buff),FP_OFF(buff),0xB800,0,4000);
	free (buff);
	return 0;
}

/******** Put stuff in info box. ********/
putbox (string)
char *string;
{
	window (2,22,79,23);
	textbackground (BLACK);
	textcolor (WHITE);
	clrscr();
	window (3,22,79,24);
	gotoxy (1,1);
	cputs (string);

	return 0;
}

/******** Put stuff on status line ********/
putstatus (string)
char *string;
{
	struct text_info txtinfo;

	gettextinfo (&txtinfo);
	window (1,25,80,25);
	textbackground (CYAN);
	textcolor (BLUE);
	clrscr();
	gotoxy (2,1);
	cputs (string);
	textattr (txtinfo.attribute);

	return 0;
}

/******** Error message function ********/
error (string)
char *string;
{
	int x;
	char errstr[80],*p;

	p=savescreen();

	strcpy (errstr,"Error: ");
	strcat (errstr,string);

	x=strlen(errstr)/2;
	window (40-x-2,12,40+x+2,14);
	textbackground (RED);
	clrscr();
	window (1,1,80,25);
	box (40-x-2,12,40+x+2,14,DOUBLE,DOUBLE,YELLOW,SHADOW);
	gotoxy (40-x,13);
	textcolor (WHITE);
	cputs (errstr);

	putstatus ("Hit any key...");

	while (kbhit()) getch();
	getch();
	if (kbhit()) do getch(); while (kbhit());

	putscreen(p);

	return 0;
}

/******** Edit a string - like gets(), put a string can be given ********/
char *editstring(char *string, int len)
{
	int ch,pos,quit=0;
	char orig[81];
	char work[81];

	strcpy (orig,string);
	strcpy (work,string);
	cputs (work);
	pos=strlen(work);
	do {
		ch=getch();
		switch (ch) {
			case 13:	quit=1; break;
			case 27:	strcpy (work,orig);
						break;
			case 8:		if (pos > 0) {
							pos--;
							gotoxy (wherex()-1,wherey());
							cputs (" ");
							gotoxy (wherex()-1,wherey());
							work[pos]=0;
						}
						break;
			default:    if (pos < len) {
							work[pos]=ch;
							work[pos+1]=0;
							pos++;
							cprintf ("%c",(char)ch);
							break;
						}
		}
	} while (!quit);
	strcpy (string,work);

	return string;
}


/******** Put a dialog box on the screen and get a string ********/
char *getname(boxinfo)
struct BOXSTRUCT *boxinfo;
{
	int rx,ry,bx,i;
	char usrinput[60];

	window (1,1,80,25);

	rx=boxinfo->tx+6+strlen(boxinfo->prompt)+boxinfo->length;
	ry=boxinfo->ty+2;

	textbackground (boxinfo->promptbg);
	window (boxinfo->tx,boxinfo->ty,rx,ry);
	clrscr();
	window (1,1,80,25);
	box (boxinfo->tx,boxinfo->ty,rx,ry,DOUBLE,DOUBLE,boxinfo->border,SHADOW);
	window (boxinfo->tx+2,boxinfo->ty+1,rx,ry);
	textcolor (boxinfo->promptfg);
	cputs (boxinfo->prompt);
	cputs (" ");
	textcolor (boxinfo->boxfg);
	textbackground (boxinfo->boxbg);
	cursoron();
	bx=wherex()+1;
	for (i=0; i < boxinfo->length+2; i++) cputs (" ");
	gotoxy (bx,1);
	strcpy (usrinput,boxinfo->string);
	editstring (usrinput,boxinfo->length);
	cursoroff();
	window (1,1,80,25);

	strcpy (boxinfo->string,usrinput);
	return boxinfo->string;
}


/******** Get a number from dialog box ********/
float getnumber (boxinfo,dp)
struct BOXSTRUCT *boxinfo;
int dp;
{
	char sval[10];

	do {
		sprintf (sval,"%.*f",dp,boxinfo->number);
		strcpy (boxinfo->string,sval);
		getname (boxinfo);
		sscanf (boxinfo->string,"%f",&boxinfo->number);

		if ((boxinfo->number < boxinfo->minval) || (boxinfo->number > boxinfo->maxval)) {
			sprintf (scratch,"Number must be between %.*f and %.*f",dp,boxinfo->minval,dp,boxinfo->maxval);
			error (scratch);
		}
	} while ((boxinfo->number < boxinfo->minval) || (boxinfo->number > boxinfo->maxval));

	return boxinfo->number;
}


