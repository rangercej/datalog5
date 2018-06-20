#include <mem.h>
#include "datalog.h"

/***************************************************************************
** GLOBAL VARIABLES
***************************************************************************/

extern unsigned _stklen=10000;

#ifdef DEBUG
	FILE *debugfile;
#endif

struct CURSORDATA curs;
struct CHANNELDATA channel;

char *devices[]={"None","Voltage","B Therm","E Therm",
				"J Therm","K Therm","R Therm",
				"S Therm","T Therm","Strain",
				NULL };
int numdevices=10;

char *straintype[]={"","Full","Half","","Single",NULL };
int straintypes[]={1,2,4};

int baud[]={110,150,300,600,1200,2400,4800,9600,0};
int port[]={1,2,3,4,0};

int channel_list[14];			/* List of channels to monitor */
int number_list;
char output_str[14][10];		/* The control strings to TA-880 */
char units[14][4];				/* Channel units */
char out_values[14][10];		/* Output values of channels 11,12 and 14 */

char *months[]={"January","Febuary","March","April",
				"May","June","July","August",
				"September","October","November","December"};


char scratch[500];				/* Scratch string for temp values */
