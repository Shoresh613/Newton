
/* stguide_help() Anropar ST-Guide.
 *****************************/


#include <aes.h>
#include <vaproto.h>
#include <portab.h>
#include "newton.h"

extern int appl_id;
extern int msg[8];

void stguide_help(void)
{
	int stguide_id;
	static char helppage[]="*:\\NEWTON.HYP Contents";
	char *No_STGuide=NULL;

	rsrc_gaddr( R_STRING, NO_STGUIDE, &No_STGuide );

	stguide_id = appl_find( "ST-GUIDE" );
	if( stguide_id < 0 )
		form_alert(1,No_STGuide);
	else
	{
		msg[0] = VA_START;
		msg[1] = appl_id;
		msg[2] = 0;
		msg[3] = (WORD)(((LONG) helppage >> 16) & 0x0000ffff);
		msg[4] = (WORD)((LONG) helppage & 0x0000ffff);
		msg[5] = 0;
		msg[6] = 0;
		msg[7] = 0;

		if( appl_write( stguide_id, 16, msg ) == 0 )
			form_alert(1,"[1][Could not call ST Guide!][   OK   ]");
	}
}