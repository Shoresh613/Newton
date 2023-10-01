

/* av_sendmesg(key,kstate,message)
 * Skickar vidare tangenttryckningar som Newton inte f”rst†r,
 * skickar „ven de AV meddelanden som Newton f”rst†r.
 ******************************************************/


#include <aes.h>
#include <vaproto.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "xtras.h"

extern int appl_id;
extern int msg[8], mx, my;
extern int whandle;
extern int MagX, MiNT;


void av_sendmesg(int key, int kstate, int message)
{
	int i=0;
	char *AVSERVER=NULL;
	char avserver[]="        ";
	int av_server=0;
	char *scrp_ptr=NULL;
	char *Newton=NULL;

	AVSERVER=getenv("AVSERVER");

	if(AVSERVER==NULL)
		return; /* Om det inte finns n†gon, s† skit i det. */

	while (AVSERVER[i]!=0)
	{	avserver[i]=AVSERVER[i];
		i++;
	}

	while(strlen(avserver)!=8) /* G”r den till 8 tecken, f”r appl_find. */
		strcat(avserver," ");

	av_server = appl_find( avserver );

	if(av_server < 0)
		return; /* Om den inte „r ig†ng, s† skit i det. */

	switch(message)
	{
		case AV_SENDKEY:
			msg[0] = AV_SENDKEY;
			msg[1] = appl_id;
			msg[2] = 0;
			msg[3] = kstate;
			msg[4] = key;
			msg[5] = 0;
			msg[6] = 0;
			msg[7] = 0;
			break;
		case AV_ACCWINDOPEN:
			msg[0] = AV_ACCWINDOPEN;
			msg[1] = appl_id;
			msg[2] = 0;
			msg[3] = whandle;
			msg[4] = 0;
			msg[5] = 0;
			msg[6] = 0;
			msg[7] = 0;
			break;
		case AV_ACCWINDCLOSED:
			msg[0] = AV_ACCWINDCLOSED;
			msg[1] = appl_id;
			msg[2] = 0;
			msg[3] = whandle;
			msg[4] = 0;
			msg[5] = 0;
			msg[6] = 0;
			msg[7] = 0;
			break;
		case AV_PROTOKOLL:
			if (MiNT || MagX)
				Newton = (BYTE *) Mxalloc (9, 0 | MGLOBAL);
			else
				Newton = (BYTE *) Malloc (9);
			strcpy(Newton, "Newton  ");
			msg[0] = AV_PROTOKOLL;
			msg[1] = appl_id;
			msg[2] = 0;
			msg[3] = VA_START;
			msg[4] = 0;
			msg[5] = 0;
			AVSTR2MSG(msg,6,Newton);
			break;
		case AV_PATH_UPDATE:
			if (MiNT || MagX)
				scrp_ptr = (BYTE *) Mxalloc (256, 0 | MGLOBAL);
			else
				scrp_ptr = (BYTE *) Malloc (256);

			scrp_read( scrp_ptr ); /* Var ligger klippbordet? */
			msg[0] = AV_PATH_UPDATE;
			msg[1] = appl_id;
			msg[2] = 0;
			AVSTR2MSG(msg,3,scrp_ptr); /* Uppdaterar endast vid kopiering */
			msg[5] = 0;
			msg[6] = 0;
			msg[7] = 0;
			break;
		case AV_WHAT_IZIT:
			msg[0] = AV_WHAT_IZIT;
			msg[1] = appl_id;
			msg[2] = 0;
			msg[3] = mx;
			msg[4] = my;
			msg[5] = 0;
			msg[6] = 0;
			msg[7] = 0;
			break;
		case AV_EXIT:
			msg[0] = AV_EXIT;
			msg[1] = appl_id;
			msg[2] = 0;
			msg[3] = appl_id;
			msg[4] = 0;
			msg[5] = 0;
			msg[6] = 0;
			msg[7] = 0;
			break;
		default:
			break;
	}

	if( appl_write( av_server, 16, msg ) == 0 ){ /* Om det inte funkade */
		evnt_timer(100,0);	/* V„nta 100 ms med Mfree s† de hinner l„sa */
		if(Newton!=NULL)
			Mfree(Newton);
		if(scrp_ptr!=NULL)
			Mfree(scrp_ptr);
		return;                                  /* G”r ingenting.      */
	}
	else{
		evnt_timer(100,0);	/* V„nta 100 ms med Mfree s† de hinner l„sa */
		if(Newton!=NULL)
			Mfree(Newton);
		if(scrp_ptr!=NULL)
			Mfree(scrp_ptr);
		return;
	}
}