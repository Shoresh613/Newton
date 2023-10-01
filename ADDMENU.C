#include <aes.h>
#include <vaproto.h>
#include <string.h>
#include <stdio.h>
#include "xtras.h"


extern GS_INFO gs_info;
extern GS_PARTNER *gs_partner[8];
extern int gs_partnerid, gs_id;
extern int msg[8];
extern int appl_id;
extern int MiNT,MagX;

void addtomenusend(char* commandline);
void addtomenu_request(void);

void addtomenu_request(void)
{
	int j;
	for (j = 0; gs_partner[j]->gs_partnerid!=-1 && j<8 ; j++ );
	gs_partner[j]->gs_partnerid=appl_find("MLTISTRP");
	gs_partner[j]->gs_id=ADDTOMENU;

	if(gs_partner[j]->gs_partnerid!=0)
	{
		msg[0]=GS_REQUEST;
		msg[1]=appl_id;
		msg[2]=0;
		msg[3]=(int)(((long) &gs_info >> 16) & 0x0000ffff);
		msg[4]=(int)((long) &gs_info & 0x0000ffff);
		msg[5]=0;
		msg[6]=0;
		msg[7]=ADDTOMENU;

		if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
			return;                                   /* G”r ingenting.      */
	}
}

void addtomenu()
{
	int j;
	char *commandline=NULL;		

	for (j = 0; gs_partner[j]->gs_id!=ADDTOMENU && j<8 ; j++ );

	if (MiNT || MagX)
		commandline = (BYTE *) Mxalloc (256, 0 | MGLOBAL);
	else
		commandline = (BYTE *) Malloc (256);
	memset(commandline, 0, sizeof(commandline));

	sprintf(commandline,"ADDTOMENU!Font!2!");
	addtomenusend(commandline);

	sprintf(commandline,"ADDTOMENU!Settings!3!");
	addtomenusend(commandline);

	sprintf(commandline,"ADDTOMENU!Samples!1!");
	addtomenusend(commandline);

	sprintf(commandline,"ADDTOMENU!Colours!4!");
	addtomenusend(commandline);

	sprintf(commandline,"ADDTOMENU!-!-1!");
	addtomenusend(commandline);

	sprintf(commandline,"ADDTOMENU!Info!6!");
	addtomenusend(commandline);

	sprintf(commandline,"ADDTOMENU!Help!7!");
	addtomenusend(commandline);
}

void addtomenusend(char* commandline)
{
	int i=0, j=0;

	i=(int)strlen(commandline);
	
	for ( ; i>0 ; i-- )
	{
		if(*(commandline+i)=='!')
			*(commandline+i)='\0';
	}

	for (j = 0; (gs_partner[j]->gs_id!=ADDTOMENU) && (j<8) ; j++ );
	msg[0]=GS_COMMAND;
	msg[1]=appl_id;
	msg[2]=0;
	AVSTR2MSG(msg,3,commandline);
	msg[5]=0;
	msg[6]=0;
	msg[7]=ADDTOMENU;

	if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
		return;                                     /* Avbryt.             */	

	evnt_timer(100,0);
}
