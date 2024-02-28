

/* scrap_read(), scrap_write(cut)
 * Cut and Paste.
 ************************/

#include <stdio.h>
#include <string.h>
#include <aes.h>
#include <vaproto.h>
#include <portab.h>
#include "xtras.h"
#include "newton.h"

extern char s[RADER][TECKEN];
extern int scrollVector[RADER];
extern int n, appl_id, msg[8], synliga_rader, posi;
extern int MagX, MiNT;

extern void av_sendmesg(int key, int kstate, int message);
extern void redraw_window( int all );
extern void insert(char* string, char* toBinserted);
extern void scroll(void);
extern void eraseExpression(void);

void scrap_read(void)
{
	FILE *fp;

	char scrp_ptr[128];
	char *clip;
	char *Empty_Clipboard=NULL, *tmp=NULL;
	int eof=0,rowCount=0;

	rsrc_gaddr( R_STRING, EMPTY_CLIPBOARD, &Empty_Clipboard );
	scrp_read( scrp_ptr );
	strcat( scrp_ptr, "scrap.txt" );

	if((fp=fopen(scrp_ptr,"r"))== NULL)
	{	form_alert( 1,Empty_Clipboard);
		return;
	}
	
	tmp = (BYTE *) Malloc (3);
	clip = (BYTE *) Malloc (TECKEN);
	memset(tmp,0,3);
	memset(clip,0,TECKEN);
		
	while(eof!=EOF)
	{
		eof=fscanf( fp, "%[^\r\n]", clip);	/* Reads until end of line */
		
		if(eof==EOF)
			break;
		if(strlen(clip) || (strcmp(clip,"")==0) ){
			if( strlen(clip) && ((strlen(clip)+strlen(s[n]))<TECKEN)) /* If not empty and not too long */
				insert(s[n], clip );
			n++;
			rowCount++;
		}
		if(eof==EOF)
			break;
		eof=fscanf( fp, "%c", tmp);			/* Reads CR*/
		if(eof==EOF)
			break;
		if(strcmp(tmp,"\r")==0)
			eof=fscanf( fp, "%c", tmp);		/* Reads LF*/		
		if(n>=synliga_rader-1)					/* Scrolls to new row if to load new row */
			scroll();
	}
	n--;

	while(rowCount>1) {	/* Tells which rows are connected */
		scrollVector[n-rowCount+1]=1;
		rowCount--;
	}

	posi=(int)strlen(s[n]);
	redraw_window(2);
	fclose(fp);Mfree(tmp);Mfree(clip);
}

void scrap_write( int cut ) /* cut==1, cut instead of copy, 2 copy all visible lines, 3 cut all visible lines */
{
	FILE *fp=NULL;

	char scrp_ptr[128];
	char *Write_Clipboard=NULL;
	char *ptr=NULL;
	int i=0;

	if (MiNT || MagX)
		ptr=(char*)Mxalloc( 5, 0|MGLOBAL );
	else
		ptr=(char*)Malloc( 5 );
	memset(ptr,0,sizeof(ptr));

	rsrc_gaddr( R_STRING, WRITE_CLIPBOARD, &Write_Clipboard );
	scrp_read( scrp_ptr );
	strcat( scrp_ptr, "scrap.txt" );

	if((fp=fopen(scrp_ptr,"w"))== NULL)
		form_alert( 1,Write_Clipboard);

	if(cut==0 || cut==1) /* If just copy */
	{
		if(s[n][0]==0) /* If the row is empty */
			return;
		i=n;
		if(i>0) {
			while(scrollVector[i-1] && i>0){	/* Finds where the expression starts */
				i--;
			}
		}
		fputs( s[i], fp ); /* Writes the first row of the expression to the clipboard. */
		fputs("\r\n",fp);	/* Start with new row. */
		while(scrollVector[i]){ /* Write all remaining rows (if any) to the clipboard. */
			fputs( s[i+1], fp );
			fputs("\r\n",fp);
			i++;
		}
	}

	if(cut==2 || cut==3) /* Copy in all row */
	{
		for ( i=0 ;i<synliga_rader ;i++)
		{
			if(s[i][0]!=0) /* If row not empty */
				fputs( s[i], fp );
			
			fputs("\n",fp);
		}
	}
	
	if( cut == 1 )
		eraseExpression();
	if( cut == 3){
		for ( i=0 ;i<synliga_rader ;i++) /* Clears all rows */
			memset(s[i],0,TECKEN);
		n=0;
		posi=0;
	}

	fclose(fp);

	strcpy(ptr,".TXT");

	msg[0]=SC_CHANGED;
	msg[1]=appl_id;
	msg[2]=0;
	msg[3]=2; /* text */
	msg[4]=(WORD)(((LONG) ptr >> 16) & 0x0000ffff);
	msg[5]=(WORD)((LONG) ptr & 0x0000ffff);
	msg[6]=0;
	msg[7]=0;
	
	if(_GemParBlk.global[0]>=0x400)			/* Broadcast only available after AES v.4 */
		shel_write(7,0,0,NULL,(char*)msg);
	av_sendmesg(0,0,AV_PATH_UPDATE);		/* Send AV message too, jinnee doesn't seem to get SC_CHANGED */
	
	evnt_timer(100,0);						/* Wait a bit before emptying so everyone has time to read */
	Mfree(ptr);
}