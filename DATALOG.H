/***************************************************************************
**
** DATALOG -- Copyright 1995 C.Johnson. All rights reserved
**
** Controller program for TA-880 multimonitor
**
***************************************************************************/

/***************************************************************************
** PROGRAM DEFINES
***************************************************************************/

#define TEST				/* Run program as test - no data logging */
/* #define DEBUG */				/* Debugging mode */

#define SINGLE 1  				/* Defines for BOX function...single or */
#define DOUBLE 2				/* double lines */

#define FALSE 0
#define TRUE -1

#define INPUT 0
#define OUTPUT 1

#define NOSHADOW 0
#define SHADOW 1

#define NODRAW 0
#define DRAW 1

/***************************************************************************
** STRUCTURE DEFINITIONS
***************************************************************************/

/***** CURSOR INFORMATION *****/
struct CURSORDATA {
	unsigned int size;			/* Size of cursor */
	int visible;				/* Visible flag */
};


/***** MENU BAR STRUCTURE *****/
struct MENUBARINFO {
	int tx,ty;					/* x,y of first entry of menu */
	int hilength;				/* Length of hilight */
	int num;  					/* Number of entries */
	int hilitebg;				/* Colour of hilighted area */
	int normalbg;				/* Colour of non-hilighted area */
	int step;					/* Number of lines between each entry */
};


/***** DIALOG BOX STRUCTURE *****/
struct BOXSTRUCT {
	int tx,ty;					/* x,y posn of top left of box */
	int border;					/* Border colour */
	int promptbg;				/* Prompt background colour */
	int promptfg;				/* Prompt foreground colour */
	int boxfg;					/* Input box foreground colour */
	int boxbg;					/* Input box background colour */
	int length;					/* Length of input box */
	char prompt[30];			/* Prompt string */
	char string[50];			/* String to edit */
	float number;				/* Number to change */
	float minval;				/* Minimum value of number */
	float maxval;				/* Maximum value of number */
};

/***** MENU BOX STRUCTURE *****/
struct MENUBOXINFO {
	int tx,ty;					/* x,y posn of top left of box */
	int bx,by;					/* x,y posn of bottom right of box */
	int lx,ly;					/* x,y posn of first menu item relative to box */
	int step;					/* y-step between items */
	int pos;					/* Initial position of hilight bar */
	int menubg;					/* Menu background colour */
	int menufg;					/* Menu foreground colour */
	int hilite;					/* Hilighted character colour */
	int border;					/* Border colour */
};

struct STRAINTYPE {
	float voltage;
	float factor;
	int   type;
};

struct CHANNELDATA {            /* Multi-monitor configuration */
	char equip[14][10];				/* Transducers connected */
	struct STRAINTYPE strain; 		/* Strain guage setup */
	char notes[14][49];				/* Notes for each bit */
	int iotype[14];					/* Is channel input or output? */
	unsigned int port; 				/* Com port (1 -> 4) */
	int baud;  						/* Baud rate */
	int start;						/* Number of start bits */
	int data;						/* Number of data bits */
	int parity;						/* Number of parity bits */
	int stop;						/* Number of stop bits */
};


/***************************************************************************
** FUNCTION PROTOTYPES
***************************************************************************/
int init            (char *start,int argcount, char *arguments[]);
int run             (char *st_dir);
int done            (void);

int title           (void);
int menu            (void);
int info            (void);
int status          (void);

int showbar         (int pos,int oldpos,struct MENUBARINFO *menuinfo);
int monitor         (void);
int setup           (void);
int about           (char *start);

int setupip         (void);
int setupop         (void);
int setupcm         (void);
int setupsv         (void);
int setupld         (void);

int box             (int left,int top,int right,int bottom,int vlines,int hlines,int color,int shadow);
int center          (int line,char *string,int colour);
int cursoroff       (void);
int cursoron        (void);
char *savescreen	(void);
int putscreen       (char *scr);
int putbox          (char *string);
int putstatus       (char *string);

char *editstring 	(char *edi_str, int len);
char *getstring 	(void);
char *getname 		(struct BOXSTRUCT *boxinfo);
float getnumber 	(struct BOXSTRUCT *boxinfo, int dp);

