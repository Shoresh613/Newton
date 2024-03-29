

/* do_dialog(OBJECT *dialog)
 * Run a dialogue with the user.
 *******************************/


#include <aes.h>
#include <stdio.h>
#include <portab.h>
#include <string.h>
#include "xtras.h"

int do_dialog(OBJECT *dialog)
{
	GRECT lg, bg;    
	int exitbutton=0, tmp=0,tmp2=0,tmp3=0;
	void *flyinf=NULL;
	void *scantab = NULL;   /* if necessary Keycode Table */
	int lastcrsr=0;       /* Cursor position at dialog end */

	tmp3=appl_getinfo(14,&tmp, &tmp2, &tmp2, &tmp2);	/* checks if can form_xdial */
	if(!tmp3)	/* if there was any error, say can't xdial */
		tmp=0;
	form_center( dialog, &bg.g_x, &bg.g_y, &bg.g_w, &bg.g_h );
	lg.g_x=bg.g_x+(bg.g_w/2); lg.g_y=bg.g_y+(bg.g_h/2); lg.g_w=bg.g_x+(bg.g_w/2)+1; lg.g_h=bg.g_y+(bg.g_h/2)+1; /* center the small one too */
	wind_update( BEG_UPDATE );
	if(tmp)
		form_xdial( FMD_START, lg.g_x,lg.g_y,lg.g_w,lg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h, &flyinf);
	else
		form_dial( FMD_START, lg.g_x,lg.g_y,lg.g_w,lg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h);
	objc_draw( dialog, 0, MAX_DEPTH, bg.g_x, bg.g_y, bg.g_w, bg.g_h );
	
	if(tmp){
		exitbutton = 0x7F & form_xdo( dialog, 0, &lastcrsr, scantab, flyinf );
		form_xdial( FMD_FINISH, lg.g_x,lg.g_y,lg.g_w,lg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h,&flyinf);
	}
	else {
		exitbutton = 0x7F & form_do( dialog, 0 );
		form_dial( FMD_FINISH, lg.g_x,lg.g_y,lg.g_w,lg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h);
	}
	wind_update( END_UPDATE );
	dialog[exitbutton].ob_state ^= SELECTED;
	return(exitbutton);
}