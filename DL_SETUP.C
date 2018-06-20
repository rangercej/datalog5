#include <stdio.h>
#include <mem.h>
#include <conio.h>
#include "datalog.h"

/***************************************************************************
** GLOBAL EXTERNS
***************************************************************************/
extern struct CHANNELDATA channel;
extern char *devices[];
extern char *straintype[];
extern int numdevices;
extern int baud[];
extern int port[];
extern int straintypes[];

/***************************************************************************
** SETUP ROUTINES
***************************************************************************/

/******** Setup the channels ********/
setupip()
{
	int i,ch,pos,col=1;
	int quit=FALSE;
	char io[4],*p;
	struct MENUBARINFO menuinfo;

	p=savescreen();

	menuinfo.tx=12;
	menuinfo.ty=10;
	menuinfo.hilength=15;
	menuinfo.hilitebg=BLACK;
	menuinfo.normalbg=RED;
	menuinfo.num=14;
	menuinfo.step=1;

	window (1,7,80,24);
	textbackground (RED);
	clrscr();
	window (1,1,80,25);
	box (1,7,80,24,DOUBLE,SINGLE,WHITE,NOSHADOW);

	window (3,8,79,24);
	textcolor (WHITE);
	cputs ("Ch.  Dir  Type             Notes\n\r");
	cputs ("---  ---  ---------------  ------------------------------------------------");
	window (3,10,79,24);
	for (i=0; i < 14; i++) {
		if (channel.iotype[i]==INPUT) strcpy (io,"In");
		if (channel.iotype[i]==OUTPUT) strcpy (io,"Out");
		cprintf ("%2d.  %-3s  %-15s  %-48s\n\r",i+1,io,channel.equip[i],channel.notes[i]);
	}

	pos=showbar (1,1,&menuinfo);

	putstatus ("\x18\x19\x1b\x1a - Move bar    \x11ды - Change entry    ESC - Return to menu    F2 - Strain");

	do {
		ch=toupper(getch());
		switch (ch) {
			case 13: chentry (pos,col);
					 putstatus ("\x18\x19\x1b\x1a - Move bar     \x11ды - Change entry     ESC - Return to menu");
					 break;
			case 27: quit=TRUE; break;
			case 0: ch=getch();
					switch (ch) {
						case 72: pos=showbar (--pos,pos,&menuinfo); break;
						case 80: pos=showbar (++pos,pos,&menuinfo); break;
						case 75: hidebar (pos,&menuinfo);
								 menuinfo.tx=12; menuinfo.hilength=15; col=1;
								 showbar (pos,pos,&menuinfo);
								 break;
						case 77: hidebar (pos,&menuinfo);
								 menuinfo.tx=29; menuinfo.hilength=48; col=2;
								 showbar (pos,pos,&menuinfo);
								 break;
						case 60: getstrain();
								 break;
					}
		}
	} while (!quit);

	putscreen (p);
	return 0;
}

/******** Change channel information ********/
chentry(posn,col)
int posn,col;
{
	struct MENUBARINFO menuinfo;
	struct MENUBOXINFO menubox;
	char string[80];
	char outstr[80];
	char *p;
	int pos;

	switch (col) {
		case 1: p=savescreen();
				putstatus ("\x18\x19 - Move bar     \x11ды - Select");

				menuinfo.tx=34; menuinfo.ty=11;
				menuinfo.hilitebg=CYAN; menuinfo.normalbg=BLUE;
				if (channel.iotype[posn-1]==INPUT)
					menuinfo.num=numdevices;
				else
					menuinfo.num=numdevices-1;
				menuinfo.step=1;
				menuinfo.hilength=11;
				menubox.tx=34; menubox.ty=10;
				menubox.bx=46; menubox.by=11+numdevices;
				menubox.lx=2;  menubox.ly=1;
				menubox.step=1;
				menubox.pos=1;
				menubox.menubg=BLUE;
				menubox.menufg=WHITE;
				menubox.border=WHITE;

				pos=domenu (&menuinfo,&menubox,devices,DRAW);
				while (pos==0) pos=domenu (&menuinfo,&menubox,devices,NODRAW);
                strcpy (channel.equip[posn-1],devices[pos-1]);

				putscreen(p);
				window (13,posn+9,27,posn+9);
				textbackground (BLACK);
				clrscr();
				cputs (channel.equip[posn-1]);
				break;

		case 2: putstatus ("\x11ды - Accept new entry");
				textbackground (BLACK);
				window (30,posn+9,77,posn+9);
				clrscr();
				cursoron();
				strcpy (string,channel.notes[posn-1]);
				textcolor (WHITE);
				textbackground (BLACK);
				strcpy (outstr,editstring(string,49));
				strncpy (channel.notes[posn-1],outstr,49);
				channel.notes[pos-1][48]=0;
				textcolor (WHITE);
				window (30,posn+9,77,posn+9);
				clrscr();
				gotoxy (1,1);
				cputs (channel.notes[posn-1]);
				cursoroff();
				break;
	}

	return 0;
}

/******** Get strain gauge information - control routine ********/
getstrain()
{
	int pos;
	char *p;
	char *menudata[4];
	char items[3][15];
	struct MENUBARINFO menuinfo;
	struct MENUBOXINFO menubox;

	p=savescreen();

	sprintf (items[0],"Voltage: %2.2f",channel.strain.voltage);
	sprintf (items[1],"Factor:  %1.2f",channel.strain.factor);
	sprintf (items[2],"Type:    %-6s",straintype[channel.strain.type]);
	menudata[0]=&items[0][0];
	menudata[1]=&items[1][0];
	menudata[2]=&items[2][0];
	menudata[3]=NULL;

	menuinfo.tx=31;
	menuinfo.ty=11;
	menuinfo.num=3;
	menuinfo.step=1;
	menuinfo.hilength=8;
	menuinfo.hilitebg=CYAN;
	menuinfo.normalbg=LIGHTGRAY;
	menubox.tx=30; menubox.ty=10;
	menubox.bx=49; menubox.by=14;
	menubox.lx=2;  menubox.ly=1;
	menubox.step=1;
	menubox.pos=1;
	menubox.menubg=LIGHTGRAY;
	menubox.menufg=BLACK;
	menubox.border=YELLOW;

	putstatus ("\x18\x19 - Move bar    \x11ды - Change entry    ESC - Return to setup");
	pos=domenu(&menuinfo,&menubox,menudata,DRAW);
	while (pos!=0) {
		strainset (pos);
		sprintf (items[0],"Voltage: %2.2f",channel.strain.voltage);
		sprintf (items[1],"Factor:  %1.2f",channel.strain.factor);
		sprintf (items[2],"Type:    %-6s",straintype[channel.strain.type]);
		menudata[0]=&items[0][0];
		menudata[1]=&items[1][0];
		menudata[2]=&items[2][0];
		menudata[3]=NULL;
		pos=domenu(&menuinfo,&menubox,menudata,DRAW);
	}

	putscreen(p);

	return 0;
}

/******** Change strain gauge information ********/
strainset(row)
int row;
{
	char *p;
	int pos=1;
	struct MENUBARINFO menuinfo;
	struct MENUBOXINFO menubox;
	struct BOXSTRUCT boxinfo;

	p=savescreen();
	switch (row) {
		case 1: putstatus ("Enter value 0-12 volts    \x11ды - Accept    ESC - Cancel");
				boxinfo.tx=27; boxinfo.ty=11;
				boxinfo.border=YELLOW;
				boxinfo.promptbg=LIGHTGRAY;
				boxinfo.promptfg=BLACK;
				boxinfo.boxfg=WHITE;
				boxinfo.boxbg=BLACK;
				boxinfo.length=5;
				strcpy (boxinfo.prompt,"Bridge voltage:");
				boxinfo.number=channel.strain.voltage;
				boxinfo.minval=0.0;
				boxinfo.maxval=12.0;
				getnumber (&boxinfo,2);
				channel.strain.voltage=boxinfo.number;
				break;

		case 2: putstatus ("Enter value 1.9-2.59      \x11ды - Accept    ESC - Cancel");
				boxinfo.tx=28; boxinfo.ty=11;
				boxinfo.border=YELLOW;
				boxinfo.promptbg=LIGHTGRAY;
				boxinfo.promptfg=BLACK;
				boxinfo.boxfg=WHITE;
				boxinfo.boxbg=BLACK;
				boxinfo.length=4;
				strcpy (boxinfo.prompt,"Gauge factor:");
				boxinfo.number=channel.strain.factor;
				boxinfo.minval=1.9;
				boxinfo.maxval=2.59;
				getnumber (&boxinfo,2);
				channel.strain.factor=boxinfo.number;
				break;

		case 3: putstatus ("\x18\x19 - Move bar      \x11ды - Accept      ESC - Cancel");
				menuinfo.tx=36;
				menuinfo.ty=12;
				menuinfo.num=3;
				menuinfo.step=1;
				menuinfo.hilength=8;
				menuinfo.hilitebg=CYAN;
				menuinfo.normalbg=LIGHTGRAY;
				menubox.tx=35; menubox.ty=11;
				menubox.bx=45; menubox.by=15;
				menubox.lx=2;  menubox.ly=1;
				menubox.step=1;
				menubox.pos=(channel.strain.type/2)+1;
				menubox.menubg=LIGHTGRAY;
				menubox.menufg=BLACK;
				menubox.border=YELLOW;

				pos=domenu(&menuinfo,&menubox,straintype,DRAW);
				if (pos!=0) channel.strain.type=straintypes[pos-1];

				break;
	}
	putscreen(p);
	return 0;
}

/******** Setup serial port ********/
setupcm()
{

	int pos;
	char *p,*menu[3];
	char data[2][15];
	struct MENUBARINFO menuinfo;
	struct MENUBOXINFO menubox;

	p=savescreen();
	putstatus ("\x18\x19 - Move bar    \x11ды - Change entry    ESC - Return to menu");

	sprintf (data[0],"&Port: %1d",channel.port);
	sprintf (data[1],"&Baud: %-4d",channel.baud);
	menu[0]=&data[0][0];
	menu[1]=&data[1][0];
	menu[2]=NULL;

	menuinfo.tx=34;
	menuinfo.ty=12;
	menuinfo.num=2;
	menuinfo.step=1;
	menuinfo.hilength=5;
	menuinfo.hilitebg=CYAN;
	menuinfo.normalbg=LIGHTGRAY;
	menubox.tx=33; menubox.ty=11;
	menubox.bx=47; menubox.by=14;
	menubox.lx=2;  menubox.ly=1;
	menubox.step=1;
	menubox.pos=1;
	menubox.menubg=LIGHTGRAY;
	menubox.menufg=BLACK;
	menubox.hilite=RED;
	menubox.border=YELLOW;

	pos=domenu(&menuinfo,&menubox,menu,DRAW);
	while (pos!=0) {
		portset (pos);
		sprintf (data[0],"&Port: %1d",channel.port);
		sprintf (data[1],"&Baud: %-4d",channel.baud);
		menu[0]=&data[0][0];
		menu[1]=&data[1][0];
		menu[2]=NULL;
		pos=domenu(&menuinfo,&menubox,menu,DRAW);
	}
	putscreen (p);
	return 0;
}

/******** Set the port number and baud ********/
portset (row)
int row;
{
	int i,pos;
	char *p,*data[15];
	char port_s[15][10];
	struct MENUBARINFO mi;
	struct MENUBOXINFO mb;

	p=savescreen();

	switch (row) {
	  case 1: i=0;
			  do {
				itoa (port[i],port_s[i],10);
				data[i]=&port_s[i][0];
			  } while (port[++i]);
			  data[i]=NULL;

			  mi.tx=38; mi.ty=11;
			  mi.hilength=3;
			  mi.num=4; mi.step=1;
			  mi.hilitebg=CYAN;
			  mi.normalbg=BLUE;
			  mb.tx=38; mb.ty=10;
			  mb.bx=42; mb.by=15;
			  mb.lx=2;  mb.ly=1;
			  mb.step=1; mb.pos=channel.port;
			  mb.menufg=WHITE; mb.menubg=BLUE;
			  mb.border=WHITE;

			  pos=domenu(&mi,&mb,data,DRAW);
			  if (pos!=0) channel.port=port[pos-1];

			  break;

	  case 2: i=0;
			  do {
				itoa (baud[i],port_s[i],10);
				data[i]=&port_s[i][0];
			  } while (baud[++i]);
			  data[i]=NULL;

			  mi.tx=36; mi.ty=10;
			  mi.hilength=6;
			  mi.num=8; mi.step=1;
			  mi.hilitebg=CYAN;
			  mi.normalbg=BLUE;
			  mb.tx=36; mb.ty=9;
			  mb.bx=43; mb.by=18;
			  mb.lx=2;  mb.ly=1;
			  mb.step=1; mb.pos=1;
			  mb.menufg=WHITE; mb.menubg=BLUE;
			  mb.border=WHITE;

			  pos=domenu(&mi,&mb,data,DRAW);
			  if (pos!=0) channel.baud=baud[pos-1];

			  break;
	}

	putscreen (p);

	return 0;
}

/******** Save setup ********/
setupsv()
{
	char fname[15],*p;
	FILE *setupfile;
	struct BOXSTRUCT boxinfo;

	p=savescreen();
	putstatus ("\x11ды - Save setup");

	boxinfo.tx=30; boxinfo.ty=12;
	boxinfo.border=YELLOW;
	boxinfo.promptbg=LIGHTGRAY;
	boxinfo.promptfg=BLACK;
	boxinfo.boxfg=WHITE;
	boxinfo.boxbg=BLACK;
	boxinfo.length=8;
	strcpy (boxinfo.prompt,"Name?");
	strcpy (boxinfo.string,"DATALOG");
	getname (&boxinfo);

	strcpy (fname,boxinfo.string);
	strcat (fname,".DL");

	if ((setupfile=fopen(fname,"wb"))==NULL) {
		error ("Cannot create file");
	} else {
		if (fwrite (&channel,sizeof (channel),1,setupfile) != 1) {
			error ("Cannot write to file");
		}
		fclose (setupfile);
	}

	putscreen(p);

	return 0;
}

/******** Load a setup ********/
setupld()
{
	char fname[15],*p;
	FILE *setupfile;
	struct BOXSTRUCT boxinfo;

	p=savescreen();
	putstatus ("\x11ды - Load setup");

	boxinfo.tx=30; boxinfo.ty=12;
	boxinfo.border=YELLOW;
	boxinfo.promptbg=LIGHTGRAY;
	boxinfo.promptfg=BLACK;
	boxinfo.boxfg=WHITE;
	boxinfo.boxbg=BLACK;
	boxinfo.length=8;
	strcpy (boxinfo.prompt,"Name?");
	strcpy (boxinfo.string,"DATALOG");
	getname (&boxinfo);

	strcpy (fname,boxinfo.string);
	strcat (fname,".DL");

	if ((setupfile=fopen(fname,"rb"))==NULL) {
		error ("Cannot find file");
	} else {
		if (fread (&channel,sizeof (channel),1,setupfile) != 1) {
			error ("Cannot read from file");
		}
		fclose (setupfile);
	}

	putscreen(p);

	return 0;
}

