

/* av_sendmesg(key,kstate,message)
 * Forwards keystrokes that Newton does not understand,
 * also sends the AV messages that Newton do understand.
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
		return; /* If there isn't one, screw it */

	while (AVSERVER[i]!=0)
	{	avserver[i]=AVSERVER[i];
		i++;
	}

	while(strlen(avserver)!=8) /* Make it 8 characters, for appl_find. */
		strcat(avserver," ");

	av_server = appl_find( avserver );

	if(av_server < 0)
		return; /* If it's not running, screw it. */

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

			scrp_read( scrp_ptr ); /* Where is the clipboard? */
			msg[0] = AV_PATH_UPDATE;
			msg[1] = appl_id;
			msg[2] = 0;
			AVSTR2MSG(msg,3,scrp_ptr); /* Only updates when copying */
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

	if( appl_write( av_server, 16, msg ) == 0 ){ /* If it didn't work */
		evnt_timer(100,0);	/* Wait 100 ms with Mfree so they have time to read */
		if(Newton!=NULL)
			Mfree(Newton);
		if(scrp_ptr!=NULL)
			Mfree(scrp_ptr);
		return;                                  /* Do nothing.      */
	}
	else{
		evnt_timer(100,0);	/* Wait 100 ms with Mfree so they have time to read */
		if(Newton!=NULL)
			Mfree(Newton);
		if(scrp_ptr!=NULL)
			Mfree(scrp_ptr);
		return;
	}
}