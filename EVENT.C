
/* event_loop()
 * H†ller reda p† vad som h„nder.
 *************************/

#include <aes.h>
#include <nkcc.h>
#include <string.h>
#include "xacc.h"
#include "xtras.h"
#include "newton.h"

extern int quit, graph_mode;
extern int bubble_open, iconified, cursor_blink, cursor, play_sample;
extern int msg[8], clip[4];
extern char *tmp_quit;

extern OBJECT *tree;

extern void handle_message( void );
extern void handle_button( int mx, int my, int state, int kstate );
extern void handle_keybd( int key, int kstate, int mx, int my );
extern void update_coordinates( int mx);
extern void redraw_window( int all );
extern void addtomenu_request(void);
extern void error_sound( int sound );
extern void  mouse_on( void );
extern void  mouse_off( void );

int mx, my;

void event_loop( void )
{
	int kstate,
		 key,
		 clicks,
		 event,
		 state;
	int i=0;
	int old_mx=0; /* Beh”ver inte uppdatera koordinater om musen inte flyttat p† sig */

	do
	{
		if(graph_mode) { /* Nu ska den „ven kolla av muskoordinaterna */
			event = nkc_multi( MU_MESAG | MU_BUTTON | MU_KEYBD | MU_TIMER | MU_M1,
									  258,3,0,
									/* 258: Tv† klick eller 0x0100
										3:   b†da musknapparna
										0:   not(v„nster_inte_tryckt and h”ger_inte_tryckt)
										     = v„nster_tryckt or h”ger_tryckt
									*/	0, tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +6, 
									      tree[MAIN].ob_y + tree[RUTAN].ob_height + 20  - tree[RUTAN].ob_height, tree[RUTAN].ob_width-13, tree[RUTAN].ob_height-11,
									  0, 0, 0, 0, 0,
									  msg,
									  500, 0,
									  &mx, &my, &state, &kstate, &key, &clicks );
		}
		else {
			event = nkc_multi( MU_MESAG | MU_BUTTON | MU_KEYBD | MU_TIMER,
									  258,3,0,
										0, 0, 0, 0, 0,
									  0, 0, 0, 0, 0,
									  msg,
									  500, 0,
									  &mx, &my, &state, &kstate, &key, &clicks );		
		}

		if ( event & MU_MESAG )
			if ( !xacc_message( msg ) )
				handle_message( );

		if ( event & MU_BUTTON )
			handle_button( mx, my, state, kstate );

		if ( event & MU_KEYBD )
				handle_keybd( key, kstate, mx, my );

		if ( event & MU_TIMER )
			if(!bubble_open && !iconified && cursor_blink)
				redraw_window( 3 );

		if ( event & MU_M1 ){
			if(mx!=old_mx){
				update_coordinates(mx);
				redraw_window(4);
				mouse_off();
				objc_draw( tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3] );
				mouse_on();
			}
			old_mx=mx;
		}


		if(i<15)
			i++;
		if(i==14 && (appl_find("MLTISTRP")>=0))	/* Detta „r ocks† anledningen till att den buggar ur p† en massa burkar */
			addtomenu_request(); /* Fixa till GEMScripten s† den pallar k”ra flera samtidigt ist„llet, detta „r fusk.. */
										/* Det kanske jag har gjort nu, men jag fattar inte varf”r jag varit tvungen att l„gga detta upprop s†h„r. */
	}	
	while ( !quit );
	
	if( play_sample )
		error_sound( QUIT_SOUND );
}