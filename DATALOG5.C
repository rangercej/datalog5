/***************************************************************************
**
** DATALOG -- Copyright 1996 C.Johnson. All rights reserved
**
** Controller program for TA-880 multimonitor
**
***************************************************************************/

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <dir.h>

#include "datalog.h"

#define VERSION "2.61"			/* Version number */

/***************************************************************************
** GLOBAL EXTERNS
***************************************************************************/
extern struct CURSORDATA curs;
extern struct CHANNELDATA channel;
extern int number_list;
extern int channel_list[14];
extern char output_str[14][10];
extern char out_values[14][10];

/******** The MAIN routine ********/
main(argc,argv)
int argc;
char *argv[];
{
#ifdef DEBUG
	debugfile=fopen ("c:\\datalog.dbg","at");
#endif

#ifdef DEBUG
	writedebug ("Initialising");
#endif
	init(argv[0],argc,argv);
#ifdef DEBUG
	writedebug ("Running");
#endif
	run(argv[0]);
	done();
#ifdef DEBUG
	close (debugfile);
#endif
	return 0;
}


/***************************************************************************
** MAIN CONTROL ROUTINES
***************************************************************************/

/******** Initialise the proggie ********/
init(start,argcount,arguments)
char *start;
char *arguments[];
int argcount;
{
	int i;

#ifdef DEBUG
	writedebug ("Showing copyright");
#endif
	printf ("Datalog version %s - copyright 1996 C.Johnson.\n\r",VERSION);
#ifdef TEST
	printf ("Compiled with TEST define: Serial port disabled.\n\r");
#endif
#ifdef DEBUG
	printf ("Compiled in DEBUG mode. Check file c:\\datalog.dbg\n\r");
#endif
	if (argcount > 1) {
		puts ("Error: Datalog has no command line parameters, start");
		puts ("    program simple by typing DATALOG");
#ifdef DEBUG
		for (i=0; i<argcount; i++) {
			puts (arguments[i]);
		}
#endif
		exit (1);
	}
	curs.visible=TRUE;

#ifdef DEBUG
	writedebug ("Setting up channels");
#endif
	channel.start=1;			/* These are preset and cannot be changed */
	channel.data=8;
	channel.parity=0;
	channel.stop=1;
	channel.port=1; 			/* These can be changed */
	channel.baud=2400;
	for (i=0; i<10; i++) channel.iotype[i]=INPUT;
	for (i=10; i<12; i++) channel.iotype[i]=OUTPUT;
	channel.iotype[12]=INPUT;
	channel.iotype[13]=OUTPUT;
	for (i=0; i < 14; i++) {
		strcpy (channel.equip[i],"None");
		strcpy (channel.notes[i],"");
	}
	channel.strain.voltage=00.00;
	channel.strain.factor=2.00;
	channel.strain.type=1;
	for (i=0; i < 14; i++) {
		channel_list[i]=0;
		output_str[i][0]=0;
	}
	number_list=0;

#ifdef DEBUG
	writedebug ("Setting up screen");
#endif
	clrscr();
	cursoroff();
	textbackground (CYAN);
	clrscr();
	title();
	menu();
	info();
	status();

#ifdef DEBUG
	writedebug ("Setting datalog drive and directory");
#endif
	i=strlen(start)-1;
	do {
		start[i--]=0;
	} while (start[i]!='\\');
	if (start[3]!=0) start[i]=0;
	chdir (start);
	setdisk (toupper(start[0])-'A');

#ifdef DEBUG
	writedebug ("Leaving init()");
#endif

	return 0;
}

/******** Dothe thing...ie. run it! ********/
run(st_dir)
char *st_dir;
{
	int ch,pos;
	int quit=FALSE;
	struct MENUBARINFO menuinfo;

#ifdef DEBUG
	writedebug ("In run()");
#endif
	menuinfo.tx=30;
	menuinfo.ty=10;
	menuinfo.hilength=19;
	menuinfo.num=4;
	menuinfo.hilitebg=GREEN;
	menuinfo.normalbg=BLACK;
	menuinfo.step=2;

#ifdef DEBUG
	writedebug ("Showing bar");
#endif
	pos=showbar (1,1,&menuinfo);

#ifdef DEBUG
	writedebug ("Writing status and info");
#endif
	putstatus ("1-3,0,M,S,A,Q or \x18\x19 - Move bar      \x11ฤู - Select");
	putbox ("Welcome to Datalog!");

#ifdef DEBUG
	writedebug ("Entering run():do loop");
#endif
	do {
		ch=toupper(getch());
		switch (ch) {
			case '1':
			case 'M': pos=showbar (1,pos,&menuinfo); monitor(); break;
			case '2':
			case 'S': pos=showbar (2,pos,&menuinfo); setup(); break;
			case '3':
			case 'A': pos=showbar (3,pos,&menuinfo); about(st_dir); break;
			case '0':
			case 'Q': quit=TRUE; break;
			case 0  : ch=getch();
					  switch (ch) {
						  case 72: pos=showbar(--pos,pos,&menuinfo); break;
						  case 80: pos=showbar(++pos,pos,&menuinfo); break;
					  }
					  break;
			case 13 : switch (pos) {
						  case 1: monitor(); break;
						  case 2: setup(); break;
						  case 3: about(st_dir); break;
						  case 4: quit=TRUE; break;
					  }
					  break;
		}
		switch (pos) {
			case 1: putbox ("Start monitoring the channels"); break;
			case 2: putbox ("Setup the multimonitor channels and serial port"); break;
			case 3: putbox ("About the program"); break;
			case 4: putbox ("Quit the program"); break;
		}
	} while (!quit);
#ifdef DEBUG
	writedebug ("Leaving run()");
#endif
	return 0;
}

/******** Close down the program ********/
done()
{
	window (1,1,80,25);
	cursoron();
	textbackground (BLACK);
	clrscr();
	puts ("Datalog - Copyright 1996 C.Johnson. All right reserved\n\r");

	return 0;
}


/***************************************************************************
** INIT FUNCTIONS
***************************************************************************/

/******** Put the title bit on screen ********/
title()
{
	textbackground(BLACK);
	window(1,1,80,6);
	clrscr();
	window (1,1,80,7);
	box (1,1,80,6,DOUBLE,DOUBLE,YELLOW,NOSHADOW);
	center (3,"TA-880 Controller Program",WHITE);
	center (4,"Chris Johnson 1996",WHITE);
	window (1,1,80,25);

	return 0;
}

/******** Put the menu on screen ********/
menu()
{
	int i,c;
	char *menudata[]={"&1. &Monitor","&2. &Setup","&3. &About","&0. &Quit",NULL};

	textbackground (BLACK);
	window (30,9,50,17);
	clrscr();
	window (1,1,80,25);
	box (30,9,50,17,DOUBLE,DOUBLE,YELLOW,SHADOW);

	i=-1;
	while (menudata[++i] != NULL) {
		gotoxy (35,10+(i*2));
		c=-1;
		while (menudata[i][++c] != 0) {
			if (menudata[i][c]=='&') {
			  textcolor(LIGHTCYAN);
			  c++;
			} else {
			  textcolor(WHITE);
			}
			putch (menudata[i][c]);
		}
	}

	return 0;
}

/******** Put the info box ********/
info()
{
	textbackground (BLACK);
	window (1,21,80,24);
	clrscr();
	window (1,21,80,25);
	box (1,1,80,4,DOUBLE,DOUBLE,YELLOW,NOSHADOW);
	window (1,1,80,25);

	return 0;
}

/******** Initialise the status line ********/
status()
{
	window (1,25,80,25);
	textbackground (LIGHTCYAN);
	clrscr();
	window (1,1,80,25);

	return 0;
}


/******** Start monitoring :-) ********/
monitor()
{
	int i,pos,dp=1;
	float min=0,max=0,val=0;
	char fname[15],*p;
	char *menudata[]={"&Panel view","&Log to file",NULL};
	struct MENUBARINFO menuinfo;
	struct MENUBOXINFO menubox;
	struct BOXSTRUCT boxinfo;
	unsigned long int sample=0;
	FILE *monitorfile;
	FILE *panelfile;


	p=savescreen();

	menuinfo.tx=29;
	menuinfo.ty=12;
	menuinfo.hilength=21;
	menuinfo.step=1;
	menuinfo.num=2;
	menuinfo.hilitebg=GREEN;
	menuinfo.normalbg=LIGHTGRAY;
	menubox.tx=29; menubox.ty=11;
	menubox.bx=51; menubox.by=14;
	menubox.lx=6;  menubox.ly=1;
	menubox.step=1;
	menubox.pos=1;
	menubox.menubg=LIGHTGRAY;
	menubox.menufg=BLACK;
	menubox.hilite=RED;
	menubox.border=BLACK;

	putstatus ("\x18\x19 - Move bar     \x11ฤู - Select      ESC - Cancel");
	pos=domenu (&menuinfo,&menubox,menudata,DRAW);
	if (pos==0) {
		putscreen (p);
		return 0;
	}

	putstatus ("\x11ฤู - Accept");

	boxinfo.ty=12;
	boxinfo.border=YELLOW;
	boxinfo.promptbg=LIGHTGRAY;
	boxinfo.promptfg=BLACK;
	boxinfo.boxfg=WHITE;
	boxinfo.boxbg=BLACK;
	boxinfo.length=8;
	if (pos==1) {
		boxinfo.tx=26;
		strcpy (boxinfo.prompt,"Name of panel?");
	} else {
		boxinfo.tx=27;
		strcpy (boxinfo.prompt,"Name of log?");
	}
	strcpy (boxinfo.string,"DATALOG");
	getname (&boxinfo);

	strcpy (fname,boxinfo.string);
	if (pos==1) {
		strcat (fname,".PAN");

		if ((panelfile=fopen(fname,"rb"))==NULL) {
			error ("Cannot open panel");
			putscreen (p);
			return -1;
		}
	}
	else {
		strcat (fname,".LOG");

		if ((monitorfile=fopen(fname,"w+b"))==NULL) {
			error ("Cannot create log");
			putscreen (p);
			return -1;
		}
	}

	putscreen(p);
	p=savescreen();

	if (pos==2) {
		putstatus ("\x11ฤู - Accept sample time (secs)");
		boxinfo.tx=22; boxinfo.ty=11;
		boxinfo.border=YELLOW;
		boxinfo.promptbg=LIGHTGRAY;
		boxinfo.promptfg=BLACK;
		boxinfo.boxfg=WHITE;
		boxinfo.boxbg=BLACK;
		boxinfo.length=7;
		boxinfo.number=1.0;
		boxinfo.minval=1.0;
		boxinfo.maxval=4320000.0;
		strcpy (boxinfo.prompt,"Sample frequency (secs)");

		getnumber(&boxinfo,0);
		sample=(long)boxinfo.number;
	}

	putstatus ("\x11ฤู - Accept value");
	for (i=0; i<14; i++) {
		if ((channel.iotype[i]==OUTPUT) && (channel.equip[i][0]!='N')) {
			switch (channel.equip[i][0]) {
				case 'J': min=-210; max=750; dp=1;break;
				case 'K': min=-270; max=1050; dp=1; break;
				case 'T': min=-270; max=400; dp=1; break;
				case 'E': min=0; max=860; dp=1; break;
				case 'R': min=-50; max=1650; dp=1; break;
				case 'B': min=0; max=1810; dp=1; break;
				case 'S': min=-50; max=1650; dp=1; break;
				case 'V': dp=2; if (i==13) { min=-10; max=+10; }
									else { min=-100; max=+100; }
						  break;
			}

			boxinfo.tx=24; boxinfo.ty=11;
			boxinfo.border=YELLOW;
			boxinfo.promptbg=LIGHTGRAY;
			boxinfo.promptfg=BLACK;
			boxinfo.boxfg=WHITE;
			boxinfo.boxbg=BLACK;
			boxinfo.length=7;
			sprintf (boxinfo.prompt,"Channel %2d: %-7s",i+1,channel.equip[i]);
			boxinfo.number=0.0;
			boxinfo.maxval=max;
			boxinfo.minval=min;

			getnumber (&boxinfo,dp);
			val=boxinfo.number;

			sprintf (out_values[i],"%+1.*f",dp,val);
		}
	}

	initcomport();

	if (pos==1) {
		panelmon(panelfile);
		fclose (panelfile);
	}
	else {
		startlog (monitorfile,sample);

		fputs ("\r\n\0x1A",monitorfile);
		fclose(monitorfile);
	}

	putscreen (p);
	return 0;
}

/******** Setup multimonitor ********/
setup()
{
	struct MENUBARINFO menuinfo;
	struct MENUBOXINFO menubox;

	char *menudata[]={"&1. Setup &channels",
					  "&2. Setup serial &port",
					  "&3. &Save setup",
					  "&4. &Load setup",
					  "&5. &Return to menu",
					  NULL};

	int item;
	int dodraw=DRAW;
	char *p;

	p=savescreen();

	menuinfo.tx=24; menuinfo.ty=11;
	menuinfo.hilength=31;
	menuinfo.step=1;
	menuinfo.num=5;
	menuinfo.hilitebg=GREEN;
	menuinfo.normalbg=LIGHTGRAY;
	menubox.tx=24; menubox.ty=10;
	menubox.bx=56; menubox.by=16;
	menubox.lx=5;  menubox.ly=1;
	menubox.step=1;
	menubox.pos=1;
	menubox.menubg=LIGHTGRAY;
	menubox.menufg=BLACK;
	menubox.hilite=RED;
	menubox.border=BLACK;

	putstatus ("1-5,C,P,S,L,R or \x18\x19 - Move bar      \x11ฤู - Select      ESC - Cancel");

	do {
		item=domenu (&menuinfo,&menubox,menudata,dodraw);
		dodraw=NODRAW;
		switch (item) {
			case 1: setupip(); break;
			case 2: setupcm(); break;
			case 3: setupsv(); break;
			case 4: setupld(); break;
			case 5: item=0;    break;
		}
	} while (item!=0);

	putscreen (p);

	return 0;
}

/******** About the proggie ********/
about(start)
char *start;
{
	char *p;

	p=savescreen();
	window (1,7,80,24);
	textbackground (RED);
	clrscr();
	window (1,1,80,25);
	box (1,7,80,24,DOUBLE,SINGLE,WHITE,NOSHADOW);
	window (3,8,78,25);
	textcolor (WHITE);
	cprintf ("Datalog version %s\n\r",VERSION);
	cputs ("ออออออออออออออออออออ\n\r");
	cputs ("This program is desgined for use with the RS232 version of the TA-880\n\r");
	cputs ("multimonitor.\n\n\r");
	cputs ("Copyright (c) 1996 Chris Johnson. All rights reserved.\n\r");
#ifdef TEST
	cprintf ("Compiled with TEST enabled. No serial i/o will take place.\n\r");
#endif
#ifdef DEBUG
	cprintf ("Compiled in DEBUG mode. Check file C:\\DATALOG.DBG for debug info.\n\r");
#endif
	cprintf ("DOS Version: %u.%u\n\r",_osmajor,_osminor);
	cprintf ("Memory free: %u bytes\n\r",coreleft());
	cprintf ("Datalog directory: %s",start);
	putstatus ("Hit any key ...");

	while (kbhit()) getch();
	getch();
	if (kbhit())
		while (kbhit()) getch();

	putscreen (p);

	return 0;
}



/******** Debug function ********/
#ifdef DEBUG
writedebug (char *str)
{
	fputs (str,debugfile);
	fputs ("\r\n",debugfile);

	return 0;
}
#endif

