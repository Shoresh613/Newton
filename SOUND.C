
/* error_sound(void)
 * Calls GEMjing to play a sample.
 *******************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <aes.h>
#include <tos.h>
#include <string.h>
#include <ext.h>
#include <vaproto.h>
#include "xtras.h"
#define SHW_PARALLEL    100     /* MagiC */

extern int msg[8], appl_id;
extern int MagX, MiNT;

extern char *tmp_startup,*tmp_trashcan,*tmp_language,*tmp_quit;

void error_sound( int sound )
{
	char *env=NULL, *cmdline=NULL, *tmp2=NULL;
	int gemjingid=-1;
	
	if (MiNT || MagX)
		cmdline = (char *) Mxalloc (256, 0 | MGLOBAL);
	else
		cmdline = (char *) Malloc (256);

	tmp2 = (char *) Malloc (256);
	if (!tmp2)       /* If there is not enough memory */
	    return;

	memset(cmdline, 0, sizeof(cmdline));
	memset(tmp2, 0, sizeof(tmp2));

	strcpy(tmp2,"kaka");
	if(sound==STARTUP_SOUND)
		strcpy(tmp2,tmp_startup);
	if(sound==ERASE_DD_SOUND)
		strcpy(tmp2,tmp_trashcan);
	if(sound==BAD_LANGUAGE_SOUND)
		strcpy(tmp2,tmp_language);
	if(sound==QUIT_SOUND)
		strcpy(tmp2,tmp_quit);
	if(strcmp(tmp2,"kaka")==0 || strcmp(tmp2,"")==0) { /* If no file is specified */
		Mfree(cmdline);Mfree(tmp2);
		return;
	}

	if( (gemjingid=appl_find("GEMJING ")) < 0) { /* If not running already */
		strcpy(cmdline," -q ");
		strcat(cmdline,tmp2);
		
		env=getenv("GEMJINGPATH");
		if( env && strlen(env)>0 ) { /* if $GEMJING is set */
			if (MagX)
				shel_write(1,1,100, env, cmdline);
			else
				shel_write(0,1,1, env, cmdline);
		}
	}
	else {
		strcpy(cmdline,tmp2);
		msg[0]=VA_START;
		msg[1]=appl_id;
		msg[2]=0;
		AVSTR2MSG(msg,3,cmdline);
		msg[5]=0;
		msg[6]=0;
		msg[7]=0;

		if( appl_write( gemjingid, 16, msg ) == 0 ) /* If it didn't work */
			return;                                  /* Do nothing.      */
	}

	Mfree(tmp2);Mfree(cmdline);
	return;
}