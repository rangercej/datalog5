#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include "datalog.h"

/***************************************************************************
** GLOBAL EXTERNS
***************************************************************************/
extern struct CHANNELDATA channel;
extern char out_values[14][10];
extern char output_str[14][10];
extern char units[14][4];
extern char *months[];
extern char *straintype[];
extern int channel_list[14];
extern int number_list;

/***************************************************************************
** MONITOR ROUTINES
***************************************************************************/

/******** Start the logging ********/
startlog (output,freq)
FILE *output;
unsigned long int freq;
{
	int i,oldtime,numchan=0;
	char buffer[BUFSIZ];
	unsigned long int seconds,numsamp=0;
	struct time thetime;
	struct date thedate;
	float value;

	startlogfile (output,freq);

	for (i=0; i<number_list; i++) {
		switch (channel_list[i]) {
			case 1: strcpy (output_str[i],"I1"); break;
			case 2: strcpy (output_str[i],"I2"); break;
			case 3: strcpy (output_str[i],"I3"); break;
			case 4: strcpy (output_str[i],"I4"); break;
			case 5: strcpy (output_str[i],"I5"); break;
			case 6: strcpy (output_str[i],"I6"); break;
			case 7: strcpy (output_str[i],"I7"); break;
			case 8: strcpy (output_str[i],"I8"); break;
			case 9: strcpy (output_str[i],"I9"); break;
			case 10: strcpy (output_str[i],"I10"); break;
			case 11: strcpy (output_str[i],"O11"); break;
			case 12: strcpy (output_str[i],"O12"); break;
			case 13: strcpy (output_str[i],"I13"); break;
			case 14: strcpy (output_str[i],"O14"); break;
		}
	}
	for (i=0; i<number_list; i++) {
		switch (channel.equip[channel_list[i]-1][0]) {
			case 'B': strcat (output_str[i],"B"); break;
			case 'E': strcat (output_str[i],"E"); break;
			case 'J': strcat (output_str[i],"J"); break;
			case 'K': strcat (output_str[i],"K"); break;
			case 'T': strcat (output_str[i],"T"); break;
			case 'R': strcat (output_str[i],"R"); break;
			case 'S': if (channel.equip[channel_list[i]-1][1]==' ')
							strcat (output_str[i],"S");
						else
							output_str[i][0]='S';
						break;
		}
	}
	for (i=0; i<number_list; i++) {
		if (channel.iotype[channel_list[i]-1]==OUTPUT) {
			strcat (output_str[i],out_values[channel_list[i]-1]);
		}
	}
	for (i=0; i<number_list; i++) {
		switch (channel.equip[channel_list[i]-1][0]) {
			case 'B': strcpy (units[i],"\xF8\x43"); break;
			case 'E': strcpy (units[i],"\xF8\x43"); break;
			case 'J': strcpy (units[i],"\xF8\x43"); break;
			case 'K': strcpy (units[i],"\xF8\x43"); break;
			case 'R': strcpy (units[i],"\xF8\x43"); break;
			case 'T': strcpy (units[i],"\xF8\x43"); break;
			case 'S': if (channel.equip[channel_list[i]-1][1]==' ')
							strcpy (units[i],"\xF8\x43");
						else
							strcpy (units[i],"\xE6\x73\x74");
						break;
			case 'V': if (channel_list[i] > 12)
							strcpy (units[i],"V");
						else
							strcpy (units[i],"mV");
						break;
		}
	}

	window (1,7,80,24);
	textbackground (RED);
	clrscr();
	window (1,1,80,25);
	box (1,7,80,24,DOUBLE,SINGLE,WHITE,NOSHADOW);

	window (3,8,79,24);
	textcolor (WHITE);
	cprintf ("Time      ");
	fprintf (output,"\"Time\",");
	for (i=0; ((i<number_list) && (wherex() < 70)); i++) {
		if (channel.iotype[channel_list[i]-1]==INPUT) {
			cprintf ("%-d %s ",channel_list[i],units[i]);
			gotoxy (((i+1)*10)+11,wherey());
		}
	}
	for (i=0; i<number_list; i++) {
    	if (channel.iotype[channel_list[i]-1]==INPUT) {
			fprintf (output,"\"%-2d %-3s\",",channel_list[i],units[i]);
			numchan++;
		}
	}

	cprintf ("\n\r--------  ");
	fprintf (output,"\"\"\r\n\"--------\",");
	for (i=0; ((i<number_list) && (wherex() < 70)); i++) {
		if (channel.iotype[channel_list[i]-1]==INPUT) {
			cprintf ("--------  ",channel_list[i]);
		}
	}
	for (i=0; i<number_list; i++) {
		if (channel.iotype[channel_list[i]-1]==INPUT) {
			fprintf (output,"\"--------\",",channel_list[i]);
		}
	}

	fprintf (output,"\"\"");

	warning();

	putstatus ("Hit ESC to stop logging...");
	textbackground (BLACK);
	window (1,1,80,6);
	clrscr();
	window (1,1,80,25);
	box (1,1,80,6,DOUBLE,DOUBLE,YELLOW,NOSHADOW);
	textcolor (WHITE);
	window (3,2,79,6);
	gettime (&thetime);
	getdate (&thedate);
	cprintf ("  Start time: %02d:%02d:%02d",thetime.ti_hour,thetime.ti_min,thetime.ti_sec);
	cprintf ("          Start date: %d %s %d\n\r",thedate.da_day,months[thedate.da_mon-1],thedate.da_year);
	cprintf ("\n\r  # channels: %-2d",numchan);
	cprintf ("          Sample frequency: %lu secs (=%.2f mins)",freq,(float)freq/60.0);
	cprintf ("\n\r   # samples: %-lu  ",numsamp);

	textcolor (WHITE);
	textbackground (RED);
	window (3,10,79,23);

	seconds=0;
	gettime(&thetime);
	oldtime=thetime.ti_sec;
	gotoxy (1,13);

	for (i=0; i<number_list; i++) {
		if (channel.iotype[channel_list[i]-1]==OUTPUT) {
#ifndef TEST
			putstring (output_str[i]);
#endif
		}
	}

#ifndef TEST
	sprintf (scratch,"SS=%.2f",channel.strain.voltage);
	putstring (scratch);
	sprintf (scratch,"SG=%.2f",channel.strain.factor);
	putstring (scratch);
	sprintf (scratch,"SN=%d",channel.strain.type);
	putstring (scratch);
#endif

	do {
		do {
			gettime (&thetime);
			getdate (&thedate);
			if (oldtime != thetime.ti_sec) {
				seconds++;
				updatetime(&thetime,&thedate,numsamp);
			}
			oldtime=thetime.ti_sec;
			if (seconds>=freq) {
				numsamp++;
				updatetime(&thetime,&thedate,numsamp);
				seconds=0;
				cprintf ("\r\n%02d:%02d:%02d  ",thetime.ti_hour,thetime.ti_min,thetime.ti_sec);
				fprintf (output,"\r\n\"%02d:%02d:%02d\"",thetime.ti_hour,thetime.ti_min,thetime.ti_sec);
				for (i=0; i < number_list; i++) {
					delay (10);
					if (channel.iotype[channel_list[i]-1]==INPUT) {
#ifdef TEST
						strcpy (buffer,"0");
#else
						putstring (output_str[i]);
						strcpy (buffer,getstring ());
#endif
						sscanf (buffer,"%f",&value);
						fprintf (output,",");
						if (wherex() < 70)
							switch (channel.equip[channel_list[i]-1][0]) {
								case 'B':
								case 'E':
								case 'J':
								case 'K':
								case 'T':
								case 'R': cprintf ("%8.1f  ",value); break;
								case 'S': if (channel.equip[channel_list[i]-1][1]==' ')
												cprintf ("%8.1f  ",value);
											else
												cprintf ("%7.0f   ",value);
										  break;
								case 'V': cprintf ("%7.3f   ",value); break;
							}
						switch (channel.equip[channel_list[i]-1][0]) {
								case 'B':
								case 'E':
								case 'J':
								case 'K':
								case 'T':
								case 'R': fprintf (output,"%8.1f",value); break;
								case 'S': if (channel.equip[channel_list[i]-1][1]==' ')
												fprintf (output,"%8.1f",value);
											else
												fprintf (output,"%7.0f",value);
										  break;
								case 'V': fprintf (output,"%7.3f",value); break;
						}
					}
				}
			}
		} while (!kbhit());
	} while (getch()!=27);

	return 0;
}

/******** Initialise the output log ********/
startlogfile (output,freq)
FILE *output;
unsigned long int freq;
{
	struct time thetime;
	struct date thedate;
	int i=0, j=0;

	gettime (&thetime);
	getdate (&thedate);

	fprintf (output,"\"DATALOG Log file.\"\r\n\"Log started %02d:%02d on %d %s %d\"\r\n",thetime.ti_hour,thetime.ti_min,thedate.da_day,months[thedate.da_mon-1],thedate.da_year);
	fputs ("\"--------------------------------------\"\r\n\r\n",output);
	fputs ("\"Channel data:\"\r\n",output);
	fputs ("\"Num\",\"Type\",\"Notes\"\r\n",output);
	fputs ("\"----\",\"--------\",\"-----------------------------------------------------\"\r\n",output);
	for (i=0; i<14; i++) {
		if (strcmp(channel.equip[i],"None")) {
			fprintf (output,"\"%-3d\",\"%-7s\",\"%-49s\"\r\n",i+1,channel.equip[i],channel.notes[i]);
			channel_list[j++]=i+1;
		}
	}
	number_list=j;
	fputs ("\r\n\"Output channels:\"\r\n",output);
	for (i=0; i<number_list; i++) {
		if (channel.iotype[channel_list[i]-1]==OUTPUT) {
			fprintf (output,"\"  Channel %-2d (%-7s): %s\"\r\n",channel_list[i],channel.equip[channel_list[i]-1],out_values[channel_list[i]-1]);
		}
	}
	fputs ("\r\n\"Strain gauge setup:\"\r\n",output);
	fprintf (output,"\"     Bridge voltage: %2.2f V\"\r\n\"       Gauge factor: %1.2f\r\n\"        Bridge type: %-7s\r\n",channel.strain.voltage,channel.strain.factor,straintype[channel.strain.type]);
	fprintf (output,"\r\n\"Logged data at sample rate of %d secs:\"\r\n",freq);

	return 0;
}

/******** Update the status box at the top of the screen ********/
updatetime(ctime,cdate,num)
struct time *ctime;
struct date *cdate;
unsigned long num;
{
	struct text_info curr_info;

	gettextinfo (&curr_info);
	window (3,3,80,6);
	textcolor (WHITE);
	textbackground (BLACK);
	cprintf ("Current time: %02d:%02d:%02d",ctime->ti_hour,ctime->ti_min,ctime->ti_sec);
	cprintf ("        Current date: %d %s %d      ",cdate->da_day,months[cdate->da_mon-1],cdate->da_year);
	cprintf ("\n\n\r   # samples: %-lu  ",num);
	window (curr_info.winleft,curr_info.wintop,curr_info.winright,curr_info.winbottom);
	gotoxy (curr_info.curx,curr_info.cury);
	textattr(curr_info.attribute);

	return 0;
}

/***************************************************************************
** PANEL MONITOR ROUTINES
***************************************************************************/

/******** Monitor with a panel ********/
panelmon(panel)
FILE *panel;
{
	char outs[20],buffer[255],num[10],*p;
	char far *panl;
	int pres=2,i,v,x[14],y[14],bg[14],fg[14];
	float outv;

	p=savescreen();

	panl=MK_FP(0xB800,0);
	for (i=0; i<4000; i++) panl[i]=fgetc (panel);
	for (i=0; i<14; i++) {
		x[i]=-1;
		y[i]=-1;
	}
	for (i=0; i<4000; i+=2) {
		if (panl[i]=='%') {
			num[0]=panl[i+2];
			num[1]=panl[i+4];
			num[2]=0;
			v=atoi(num);
			x[v-1]=(i%160)/2+1;
			y[v-1]=(i/160)+1;
			bg[v-1]=panl[i+1] >> 4;
			fg[v-1]=panl[i+1] & 0x0F;
			i+=4;
		}
	}

	warning();

#ifndef TEST
	sprintf (scratch,"SS=%.2f",channel.strain.voltage);
	putstring (scratch);
	sprintf (scratch,"SG=%.2f",channel.strain.factor);
	putstring (scratch);
	sprintf (scratch,"SN=%d",channel.strain.type);
	putstring (scratch);
#endif

	for (i=0; i<14; i++) {
		if ((channel.iotype[i]==OUTPUT) && (channel.equip[i][0]!='N')) {
			sprintf (outs,"O%d",i+1);

			switch (channel.equip[i][0]) {
				case 'J': strcat (outs,"J"); break;
				case 'T': strcat (outs,"T"); break;
				case 'B': strcat (outs,"B"); break;
				case 'K': strcat (outs,"K"); break;
				case 'R': strcat (outs,"R"); break;
				case 'E': strcat (outs,"E"); break;
				case 'S': if (channel.equip[i][1]==' ')
							strcat (outs,"S");
						else
							outs[0]='S';
						break;

			}
			strcat (outs,out_values[i]);
#ifndef TEST
			putstring (outs);
#endif
		}
	}

	putstatus ("Hit ESC to abort...");
	window (1,1,80,25);
	do {
		do {
			delay (500);
			for (i=0; i<14; i++) {
				switch (channel.equip[i][1]) {
					case 'o': pres=2; break;
					case ' ': pres=1; break;
					case 't': pres=0; break;
				}
				if ((x[i]!=-1) && (channel.iotype[i]==OUTPUT)) {
					gotoxy (x[i],y[i]);
					textcolor (fg[i]);
					textbackground (bg[i]);
					sscanf(out_values[i],"%f",&outv);
					cprintf ("%.*f ",pres,outv);
				}
				if ((x[i]!=-1) && (channel.iotype[i]==INPUT)) {
					sprintf (outs,"I%d",i+1);
					switch (channel.equip[i][0]) {
						case 'J': strcat (outs,"J"); break;
						case 'T': strcat (outs,"T"); break;
						case 'B': strcat (outs,"B"); break;
						case 'K': strcat (outs,"K"); break;
						case 'R': strcat (outs,"R"); break;
						case 'E': strcat (outs,"E"); break;
						case 'S': if (channel.equip[i][1]==' ')
									strcat (outs,"S");
								else
									outs[0]='S';
								break;
					}

#ifdef TEST
					sprintf (buffer,"%d",random(100));
#else

					putstring (outs);
					strcpy (buffer,getstring());
#endif
					gotoxy (x[i],y[i]);
					textcolor (fg[i]);
					textbackground (bg[i]);
					sscanf(buffer,"%f",&outv);
					cprintf ("%.*f ",pres,outv);
				}
			}
		} while (!kbhit());
	} while (getch()!=27);

	putscreen(p);

	return 0;
}

/***************************************************************************
** UNIVERSAL MONITOR ROUTINES
***************************************************************************/

/******** Show a warning message ********/
warning ()
{
	char *p;

	p=savescreen();
	window (10,10,70,16);
	textbackground (BLUE);
	textcolor (YELLOW);
	clrscr();
	window (1,1,80,25);
	box (10,10,70,16,DOUBLE,DOUBLE,YELLOW,SHADOW);
	window (12,11,69,16);
	cputs ("Make sure multi-monitor is switched on and is ready to\n\r");
	cputs ("receive and send data remotely. Hit [Z] [PROGRAM] [11]\n\r");
	cputs ("to set this if you are not sure.If the monitor is not\n\r");
	cputs ("setup, then the program WILL CRASH!\n\n\rHit any key to continue...");
	putstatus ("Hit any key...");
	while (!kbhit());
	while (kbhit()) getch();
	putscreen (p);

	return 0;
}

