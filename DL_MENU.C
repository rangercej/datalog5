#include <conio.h>
#include <dos.h>
#include <mem.h>
#include "datalog.h"

/***************************************************************************
** MENU FUNCTIONS
***************************************************************************/

/******** Show a bar on the main menu ********/
int showbar(pos,oldpos,menuinfo)
int pos,oldpos;
struct MENUBARINFO *menuinfo;
{
	int ypos,i=0;
	unsigned offs;
	char ch;
	char far *screen;

	screen=MK_FP(0xB800,0);
	if (pos < 1) pos=menuinfo->num;
	if (pos > menuinfo->num) pos=1;

	ypos=menuinfo->ty-menuinfo->step+(oldpos*menuinfo->step); ypos--;
	offs=2*((ypos*80)+menuinfo->tx);
	for (i=0; i < menuinfo->hilength; i++) {
		ch=screen[offs+(i*2)+1];
		ch=(ch & 0x0F) | (menuinfo->normalbg << 4);
		screen[offs+(i*2)+1]=ch;
	}

	ypos=menuinfo->ty-menuinfo->step+(pos*menuinfo->step); ypos--;
	offs=2*((ypos*80)+menuinfo->tx);
	for (i=0; i < menuinfo->hilength; i++) {
		ch=screen[offs+(i*2)+1];
		ch=(ch & 0x0F) | (menuinfo->hilitebg << 4);
		screen[offs+(i*2)+1]=ch;
	}
	return pos;
}

/******** Hide a hilight bar ********/
int hidebar (pos,menuinfo)
int pos;
struct MENUBARINFO *menuinfo;
{
	int ypos,i=0;
	unsigned offs;
	char ch;
	char far *screen;

	screen=MK_FP(0xB800,0);
	if (pos < 1) pos=menuinfo->num;
	if (pos > menuinfo->num) pos=1;

	ypos=menuinfo->ty-menuinfo->step+(pos*menuinfo->step); ypos--;
	offs=2*((ypos*80)+menuinfo->tx);
	for (i=0; i < menuinfo->hilength; i++) {
		ch=screen[offs+(i*2)+1];
		ch=(ch & 0x0F) | (menuinfo->normalbg << 4);
		screen[offs+(i*2)+1]=ch;
	}
	return pos;
}


/******** Put a menu on the screen and control it ********/
domenu (menubar,menubox,menuitems,draw)
struct MENUBARINFO *menubar;
struct MENUBOXINFO *menubox;
char *menuitems[];
int draw;
{
	int i,j,k,pos,retval,quit=FALSE;
	struct text_info txtinfo;
	char ch,hotkeys[20][10];

	gettextinfo (&txtinfo);

	for (i=0; i < 20; i++)
		for (j=0; j < 10; j++)
			hotkeys[i][j]=0;

	if (draw) {
		window (menubox->tx,menubox->ty,menubox->bx,menubox->by);
		textbackground (menubox->menubg);
		textcolor (menubox->menufg);
		clrscr();
		window (1,1,80,25);
		box (menubox->tx,menubox->ty,menubox->bx,menubox->by,DOUBLE,DOUBLE,menubox->border,SHADOW);
		gotoxy (menubox->tx+menubox->lx,menubox->ty+menubox->ly);
	}

	for (i=0; (menuitems[i]!=NULL); i++) {
		k=0;
		if (menuitems[i][0]!=0) {
			for (j=0; menuitems[i][j]!=0; j++) {
				if (menuitems[i][j]=='&') {
					textcolor (menubox->hilite);
					hotkeys[i][k]=toupper(menuitems[i][j+1]);
					k++; j++;
				}
				else
					textcolor (menubox->menufg);
				if (draw) putch (menuitems[i][j]);
			}
			if (draw) {
				for (j=0; j < menubox->step; j++)
					cputs ("\n\r");
				gotoxy (menubox->tx+menubox->lx,wherey());
			}
		}
	}


	retval=0;
	pos=showbar(menubox->pos,menubox->pos,menubar);
	do {
		ch=toupper(getch());
		switch (ch) {
			case 0	:	ch=getch();
					switch (ch) {
						case 72: pos=showbar(--pos,pos,menubar); break;
						case 80: pos=showbar(++pos,pos,menubar); break;
					}
					break;
			case 13	: 	retval=pos; quit=TRUE; break;
			case 27	: 	retval=0;	quit=TRUE; break;
			default :	i=0;
						do {
							j=0;
							while ((hotkeys[i][j]!=ch) && (hotkeys[i][j]!=0)) j++;
						} while ((i < 20) && (hotkeys[i++][j]!=ch));

						if (hotkeys[--i][j]==ch) {
							pos=showbar (i+1,pos,menubar);
							retval=pos;
							quit=TRUE;
						}
						break;
		}
	} while (!quit);

	textattr (txtinfo.attribute);

	menubox->pos=pos;

	return retval;
}

