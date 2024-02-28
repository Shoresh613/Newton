#include <string.h>
#include <tos.h>
#include "xtras.h"

extern int posi;
extern int MagX, MiNT;
extern int n;
extern char s[RADER][TECKEN];
extern char *insert_tmp, *insert_tmp2;

extern void redraw_window(int pos);

void insert( char* st , char* toBinserted)
{
	if(strlen(s[n])==0)	/* If the line is empty */
	{
		memset(s[n],0,TECKEN);
		strcpy(s[n],toBinserted);
		posi += (int)strlen(toBinserted);
		redraw_window(0);	/* draw the string */
		return;
	}
	if(posi==(int)strlen(s[n]))	/* If you are at the end of the line */
	{
		memset(s[n]+posi,0,TECKEN-posi);
		strcat(s[n],toBinserted);
		posi = (int)strlen(s[n]);
		redraw_window(0);
		return;
	}

	memset(insert_tmp, 0, TECKEN);
	memset(insert_tmp2, 0, TECKEN);

	if(posi==0)
	{
		strcpy(insert_tmp,toBinserted);
		strcat(insert_tmp,s[n]);
		memset(s[n],0,TECKEN);
		strcpy(s[n],insert_tmp);
	}
	else
	{
		strcpy(insert_tmp,toBinserted);
		strcat(insert_tmp,s[n]+posi);
		insert_tmp2=strdup(s[n]);
		*(insert_tmp2+posi)=0;
		strcat(insert_tmp2,insert_tmp);
		memset(s[n],0,TECKEN);
		strcpy(s[n],insert_tmp2);
	}
	posi += (int)strlen(toBinserted);
	redraw_window(2);	/* draw the character window */
}