

/* colour_select(void)
 * Calls Colours to choose the color of the marker
 **************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <aes.h>
#include <tos.h>
#include "newton.h"
#include "xtras.h"

#define SHW_PARALLEL    100     /* MagiC */

extern int MagX, MiNT;
extern int msg[8], appl_id;

void colour_select( void )
{
	char *env=NULL;
	char *No_Colour_Select=NULL;
	int colorsid;

	rsrc_gaddr( R_STRING, NO_COLOUR_SELECT, &No_Colour_Select );

	if( (colorsid=appl_find("COLORS  ")) > 0) /* If it's already running */
	{
		msg[0]=WM_TOPPED;
		msg[1]=0;
		msg[2]=0;
		msg[3]=0;
		msg[4]=0;
		msg[5]=0;
		msg[6]=0;
		msg[7]=0;

		if( appl_write( colorsid, 16, msg ) == 0 ) /* In case it didnt work */
			return;
		return;
	}

	env = (char *) Malloc (256);
	if (!env)       /* If there's not enough RAM */
	    return;

	env=getenv("COLOR_SELECT");
	if( env==NULL ) /* If $COLOR_SELECT is not set */
		form_alert( 1,No_Colour_Select);
	else
	{
		if (MagX)
			shel_write(1,1,100, env, NULL);
		else
			shel_write(0,1,1, env, NULL);
	}

	Mfree(env);
	return;
}
