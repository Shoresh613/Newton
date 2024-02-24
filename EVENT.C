
/* event_loop()
 * Keeps track of what's going on, the event handler.
 ***************************************/

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
	int old_mx=0; /* No need to update coordinates if the mouse has not moved */

	do
	{
		if(graph_mode) { /* Now it should also check the mouse coordinates */
			event = nkc_multi( MU_MESAG | MU_BUTTON | MU_KEYBD | MU_TIMER | MU_M1,
									  258,3,0,
									/* 258: Two clicks or 0x0100
										3: both mouse buttons
										0: note(left_not_printed and right_not_printed)
										= left_pressed or right_pressed
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
		if(i==14 && (appl_find("MLTISTRP")>=0))	/* This is also why it crashes on a lot of computers */
			addtomenu_request(); /* Fix the GEMScript so that it can run several at the same time instead, this is cheating.. */
										/*  may have done that now, but I don't understand why I had to put this call here. */
	}	
	while ( !quit );
	
	if( play_sample )
		error_sound( QUIT_SOUND );
}