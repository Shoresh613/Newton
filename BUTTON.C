

/* handle_button(mx,my,state, kstate)
 * Interprets button presses and forwards bubble help.
 ****************************************************/


#include <string.h>
#include <aes.h>
#include <vdi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <math.h>
#include <vaproto.h>
#include <scancode.h>
#include <nkcc.h>
#include "newton.h"
#include "xtras.h"
#include "graf.h"

extern OBJECT *tree;

extern int quit, syntax_error;
extern int n,x,y,w,h,max_x, max_y;
extern double xmin, xmax, ymin, ymax; /* For the graph */
extern int teckenbredd, teckenhojd, synliga_rader, resized, fontpi;
extern int handle, state, hex;
extern int clip[4];
extern char s[RADER][TECKEN];
extern char undo[RADER][TECKEN];
extern int scrollVector[RADER], scrollVectorBak[RADER];
extern int bubble_open;
extern int cursor, iconified;
extern int whandle;
extern int MagX, MiNT;
extern int posi,undoPosi, undoN, graph_mode;
extern char* xcoordinate, *ycoordinate;

extern void redraw_window(int pos);
extern int bubble_help(int mx, int my, int exitobj);
extern char* pre_calculate( void );
extern void open_window( void );
extern void av_sendmesg(int key, int kstate, int message);
extern void position_objects(void);
extern void show_info(void);
extern void send_dragndrop(int mx, int my, int kstate);
extern void andragradare(void);
extern void (*vqt_ex)(int handle, char* string, int* extent); /* symbol for either vqt_f_extent or vqt_extent */
extern void insert(char* string, char* toBinserted);
extern int economy_calc(void);
extern int graph_dialog(void);
extern void  mouse_on( void );
extern void  mouse_off( void );
extern void eraseExpression(void);	/* Deletes the expression where the cursor is located. */

extern int extraRedrawWorkaround;	/* A drawing unnecessarily under magic, but not under TOS it seems. */

void handle_button( int mx, int my, int state, int kstate )
{
	int exitobj=0;
	int extent[8];
	int tmp_width=0, tmp_height=0, tmp_mx=0, tmp_my=0;
	int test_width=0, test_height=0;     /* Check if the window got bigger or smaller */
	int rubbox_width=0, rubbox_height=0;
	int dummy=0, b=0, i=0;
	char* tmp=NULL;
	int j=0, k=0;
	int scrollStart=0,scrollEnd=0, maxw=0;
	double xmin_new=0, xmax_new=0, ymin_new=0, ymax_new=0, val_tmp=0; /* For the graph */
	
	for ( j=0 ; j<synliga_rader ; j++){	/* Fix undo buffer */
		strcpy(undo[j],s[j]);
		scrollVectorBak[i]=scrollVector[i];
	}
	undoPosi=posi;
	undoN=n;

	vqt_ex ( handle, s[n], extent );
	exitobj=objc_find( tree, MAIN, MAX_DEPTH, mx, my );

	if(state==2 && (exitobj!=-1) && (exitobj!=XCOORDINATE) && (exitobj!=YCOORDINATE)) /* Help text, when right clicking */
	{
		if(exitobj==RUTAN && graph_mode) /* These are very disturbing */
			return;
		if( bubble_help(mx,my,exitobj) == 0 )
			bubble_open=1;
		return;
	}

	if(cursor)  /* Turns cursor off if on */
		redraw_window(3);

	if(exitobj==BACKGROUND && graph_mode)	/* Switches to normal mode */
	{
		graph_mode=0;
		strcpy(xcoordinate,"x: ");
		strcpy(ycoordinate,"y: ");
		tree[COORDINATEBOX].ob_flags |= HIDETREE;	
		redraw_window(1);
		return;
	}
	if(exitobj==NEWTON_SIZER)
	{
		mouse_off();
		graf_mouse( POINT_HAND, NULL );
		mouse_on();
		wind_update(BEG_UPDATE);
		graf_rubbox ( tree[MAIN].ob_x-3, tree[MAIN].ob_y-3,
							286, 356,
							&tmp_width, &tmp_height );
		wind_update(END_UPDATE);
		test_width = tree[MAIN].ob_width;
		test_height= tree[MAIN].ob_height;

		tree[MAIN].ob_width=tmp_width-6;
		tree[MAIN].ob_height=tmp_height-6;
		position_objects();

		wind_calc( WC_BORDER, NAME|CLOSER|MOVER|ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h );
		wind_update(BEG_UPDATE);
		wind_set(whandle,WF_CURRXYWH,x-2,y-2,w+4,h+4);
		wind_update(END_UPDATE);

		if( (test_width > w-6) || (test_height > h-6) ){ /* If the window was made smaller */
			redraw_window(1);
			if(graph_mode)
				redraw_window(5);
		}
		else if (MagX)
			extraRedrawWorkaround=1;
		mouse_off();
		graf_mouse( ARROW, NULL );
		mouse_on();

		synliga_rader = (tree[RUTAN].ob_height-6)/teckenhojd; /* -2 for border inward by 1 pixel */
		resized=1;
		
		return;
	}
	if(exitobj==NEWTON_LOGO)
	{
		show_info();
		return;
	}

	if(exitobj==RUTAN && !iconified)
	{		
		if(graph_mode){ /* Zoom if graph */
			graf_mkstate(&mx,&my,&dummy,&dummy); /* Find out where the mouse is now. */
			mouse_off();
			graf_mouse( POINT_HAND, NULL );
			mouse_on();
			wind_update(BEG_UPDATE);
			graf_rubbox ( mx, my, 1, 1, &rubbox_width, &rubbox_height);
			wind_update(END_UPDATE);
			mouse_off();
			graf_mouse( ARROW, NULL );
			mouse_on();
		/*	if(mx+rubbox_width>XMIN+WIDTH) /* This is not working very well, but the idea is that if it is dragged outside of the display it should fix it so that it isn't, as it then crashes */
				rubbox_width=XMIN+WIDTH-mx;
			if(my+rubbox_height>YMIN+HEIGHT)
				rubbox_height=YMIN+WIDTH-my; */

			if(rubbox_width>10 && rubbox_height>10){ /* If the area is big enough */
				for ( k=0, val_tmp=xmin, j=abs(WIDTH); k<j; k++, val_tmp+= ((xmax-xmin)/(double)abs(WIDTH)) ) /* Find xmin */
				{
					if(k==mx-XMIN){	
						xmin_new=val_tmp; 			
						break;
					}
				}
				for ( k=0, val_tmp=ymin, j=abs(HEIGHT); k<j; k++, val_tmp+= ((ymax-ymin)/(double)abs(HEIGHT)) ) /* Find ymax */
				{
					if(k==my-YMAX){	
						ymin_new=val_tmp; 			
						break;
					}
				}
				for ( k=0, val_tmp=xmin, j=abs(WIDTH); k<j; k++, val_tmp+= ((xmax-xmin)/(double)abs(WIDTH)) ) /* Find xmax */
				{
					if(k==mx-XMIN+rubbox_width){	
						xmax_new=val_tmp; 			
						break;
					}
				}
				for ( k=0, val_tmp=ymin, j=abs(HEIGHT); k<j; k++, val_tmp+= ((ymax-ymin)/(double)abs(HEIGHT)) ) /* Find ymin */
				{
					if(k==my-YMAX+rubbox_height){	
						ymax_new=val_tmp; 			
						break;
					}
				}
				xmin=round(xmin_new); xmax=round(xmax_new); ymin=round(-ymax_new); ymax=round(-ymin_new); /* Strange allocations yes, but they work decently? Rounding just until I didn't come up with a clever way to get the grid to match the zero. */
				redraw_window(1); /* Clear the screen before drawing the new curve */
				redraw_window(5);
			}
			else{	/* If not big enough rubbox, switch to normal mode */
				graph_mode=0;
				strcpy(xcoordinate,"x: ");
				strcpy(ycoordinate,"y: ");
				tree[COORDINATEBOX].ob_flags |= HIDETREE;	/* Hides the coordinate box. */
				redraw_window(1);
			}
		}
		else{ /* If in normal mode */
			for ( n=-1 ; n<synliga_rader && !i ; n++ ) /* Which row are you on? */
				if( my < tree[MAIN].ob_y + (n*teckenhojd)+tree[RUTAN].ob_y+teckenhojd +6) /* The 2 comes from cursed outlining */
					i=1;
			n--; /* n starts at 0, not 1 as visible_rows */
	
			vqt_ex ( handle, s[n], extent );
			if( mx > tree[MAIN].ob_x + tree[RUTAN].ob_x + extent[2])	/* Here it should calculate at which position on the row you clicked */
				posi=(int)strlen(s[n]);
			else
			{
				tmp = (BYTE *) Malloc (TECKEN);
			
				memset(tmp, 0, TECKEN);
				if (!tmp)       /* If there is not enough memory*/
				    return;
		
				strcpy(tmp,s[n]);
				for ( j = (int)strlen(s[n]) ; j>0 ; j--)
				{
					*(s[n]+strlen(s[n])-1)=0;
					
					vqt_ex ( handle, s[n], extent );
					if( mx > tree[MAIN].ob_x + tree[RUTAN].ob_x + extent[2])
						break;
				}
				strcpy(s[n],tmp);
				posi=j-1;
		
				Mfree(tmp);
			}
	
			evnt_timer(70,0); /* So it doesn't start dragndrop instantly  */
			graf_mkstate(&dummy,&dummy,&b,&dummy);

			vqt_ex ( handle, s[n], extent );
			maxw=extent[2]-extent[0];

			scrollStart=n; /* Checks the number of rows in the expression */
			if(scrollStart>0) {
				while(scrollVector[scrollStart-1] && scrollStart>0){	/* Finds where the expression starts */
					scrollStart--;
				}
			}
			i=scrollStart;
			while(scrollVector[i]){
				vqt_ex ( handle, s[i], extent );	/* Also checks which line is the longest. */
				if(extent[2]-extent[0]>maxw)
					maxw=extent[2]-extent[0];
				i++;
			}
			scrollEnd=i;

			if(s[n][0]!=0 && b&0x001 && !iconified && (mx < tree[MAIN].ob_x + tree[RUTAN].ob_x + extent[2]) ) /* If row not empty, button still pressed, the mouse is not to the right of the expression and not iconified: Drag'n Drop */
			{
				mouse_off();
				graf_mouse( FLAT_HAND, NULL );
				mouse_on();

				wind_update(BEG_UPDATE);		
				tmp_mx=mx;tmp_my=my;
				graf_dragbox(maxw,
					teckenhojd*((scrollEnd-scrollStart)+1),
					tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +2,
					tree[MAIN].ob_y + (scrollStart*teckenhojd)+tree[RUTAN].ob_y + teckenhojd/4,
					0,0,max_x,max_y,&mx,&my);
				wind_update(END_UPDATE);

				mouse_off();
				graf_mouse( ARROW, NULL );
				mouse_on();

				if( (abs(mx-tmp_mx)>teckenhojd/2) || (abs(my-tmp_my)>teckenhojd/2) ) {	/* This test doesn't work brilliantly. */
					kstate= (kstate >> 8);     /* Convert from normalized to regular format. */
					graf_mkstate(&mx,&my,&dummy,&dummy); /* This doesn't feel like much fun, but otherwise the left point of the frame counts as the place where it was released and not the mouse */
					send_dragndrop(mx, my, kstate);
					posi=(int)strlen(s[n]);
				}
			}
			else
			{
				do{ /* Do nothing until the left mouse button is released */
					graf_mkstate(&dummy,&dummy,&b,&dummy);
				}while(b&1);
			}
		if(!cursor)
			redraw_window(3); /* The cursor must follow */
		}
		return;
	}

	if( tree[exitobj].ob_flags & SELECTABLE )
	{
		redraw_window(4);
		if(exitobj==ZOOMIN || exitobj==ZOOMOUT){	/* Hide the icon to show the selected one behind it */
			if((exitobj==ZOOMIN && (fabs(xmin)>1&&fabs(xmax)>1&&fabs(ymin)>1&&fabs(ymax)>1)) || 
				(exitobj==ZOOMOUT && (fabs(xmin)<HUGE_VAL/2 &&fabs(xmax)<HUGE_VAL/2&&fabs(ymin)<HUGE_VAL/2&&fabs(ymax)<HUGE_VAL/2)))
			{
				tree[exitobj].ob_flags |= HIDETREE;
				objc_draw(tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3]);
			}
		}
		else
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], SELECTED, 1);

		if(graph_mode && exitobj!=ZOOMIN && exitobj!=ZOOMOUT){
			graph_mode=0;
			strcpy(xcoordinate,"x: ");
			strcpy(ycoordinate,"y: ");
			tree[COORDINATEBOX].ob_flags |= HIDETREE;	/* Hides the coordinate box. */
			redraw_window(1);
		}
		
		if(exitobj==BAK)
			if( strlen(s[n])>0 && posi>0)
			{
				tmp = (BYTE *) Malloc (TECKEN);
				memset(tmp, 0, TECKEN);
				if (!tmp)       /* If there is not enough memory*/
				    return;

				if(posi==(int)strlen(s[n]))	/* If at the right edge of the row */
					*(s[n]+strlen(s[n])-1)=0;
				else {
					strcpy(tmp,s[n]);
					*(tmp+posi-1)=0;
					strcat(tmp,s[n]+posi);
					strcpy(s[n],tmp);
				}

				if(posi>0)
					posi--;
				if(n>0){
					if( strlen(s[n])==0 && scrollVector[n-1] ) {	/* If the rows were connected, break the connection. */
						scrollVector[n-1]=0;
						n--;
						posi=(int)strlen(s[n]);
					}
				}

				Mfree(tmp);
				redraw_window(4);
				objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
				redraw_window( 2 ); redraw_window( 3 );
				return;
			}

		if(exitobj==DEL){
			tmp = (BYTE *) Malloc (TECKEN);
			memset(tmp, 0, TECKEN);
			if (!tmp)       /* If there is not enough memory*/
			    return;

			if(strlen(s[n])==0){	/* If the line is empty */
				strcpy(s[n],s[n+1]);
				memset(s[n+1],0,TECKEN);
			}
			else {					/* If not empty */
				strcpy(tmp,s[n]);
				*(tmp+posi)=0;
				strcat(tmp,s[n]+posi+1);
				strcpy(s[n],tmp);
			}

			if( (posi==(int)strlen(s[n])) && scrollVector[n] ) { /* If at the end of the line and scrolled */
				strcat(s[n],s[n+1]);			/* Move up the line below */
				memset(s[n+1],0,TECKEN);
				scrollVector[n]=0;
				
				i=n+1;
				while(scrollVector[i-1]){	/* Move up remaining lines, if they belong to the same expression */
					strcpy(s[i],s[i+1]);
					memset(s[i+1],0,TECKEN);								
					scrollVector[i]=0;
					i++;
				}
			}

			Mfree(tmp);
			redraw_window(4);
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			redraw_window( 2 ); redraw_window( 3 );
			return;
		}
		
		if(exitobj==RADERA)
		{	
			if(s[n][0]==0)  /* If the line is empty, clear the entire window */
			{
				memset(s, 0, RADER*TECKEN);
				memset(scrollVector, 0, RADER);
				n=0;
			}
			else
				eraseExpression();
			posi=0;
			redraw_window(4);
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			redraw_window( 2 );redraw_window( 3 );
			return;
		}

		if(exitobj==LIKAMED)
		{
			if(n>0) {
				while(scrollVector[n-1] && n>0){	/* Finds where the expression starts */
					n--;
				}
			}
			while(scrollVector[n]){	/* Finds where the expression ends */
				n++;
			}
			strcpy(s[n],pre_calculate() );
			if(syntax_error==1)
				syntax_error=0;
			else
				posi=(int)strlen(s[n]);
			redraw_window(0);
			redraw_window(4);
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2]+100, clip[3]+100, NORMAL, 1);
			redraw_window(3);
			return;
		}
		
		if(exitobj==VAEND)
		{
			strcpy( s[n], strrev(s[n] ) );          /* Reverse the number, backwards */
			redraw_window(4);
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			redraw_window( 2 );
			return;		
		}
		if(exitobj==ABSO)
		{
			gcvt( fabs( atof(  s[n] )), 15, s[n] ); 	/* Absolute number */
			redraw_window(4);									/* Calculates the clipping */
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			posi=(int)strlen(s[n]);
			redraw_window( 2 );redraw_window( 3 );
			return;		
		}
		if(exitobj==AVRUNDA && !hex)
		{
			gcvt( round(atof( s[n] )), 15, s[n] ); /* Rounds to the nearest integer */
			redraw_window(4);
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			posi=(int)strlen(s[n]);
			redraw_window( 2 );redraw_window( 3 );
			return;		
		}
		if(exitobj==ANDRAEKV)
		{
			andragradare();
			redraw_window(4);
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			redraw_window( 2 );
			posi=(int)strlen(s[n]);
			redraw_window(3);
			return;
		}
		if(exitobj==EKONOMI)
		{
			i=0;
			i=economy_calc();

			if( i!=-1 ) /* Then CANCEL has not been pressed */
			{
				posi=(int)strlen(s[n]);
				redraw_window(2);
			}

			redraw_window(4);
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			redraw_window(3);
			return;
		}
		if(exitobj==GRAF)
		{
			if(graph_dialog()!=-1){
				graph_mode=1;
				tree[COORDINATEBOX].ob_flags &= ~HIDETREE;	/* Hides the coordinate box. */
				redraw_window(1);	/* Clear the screen first */
				redraw_window(5);	/* Display the "graph" */
			} 
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			return;
		}
		if(exitobj==ZOOMIN && (fabs(xmin)>1&&fabs(xmax)>1&&fabs(ymin)>1&&fabs(ymax)>1))
		{
			xmin=round(xmin/2); xmax=round(xmax/2); ymin=round(ymin/2); ymax=round(ymax/2);
			redraw_window(1);	/* Clear the screen first */
			redraw_window(5);	/* Display the "graph" */
			 
			tree[exitobj].ob_flags &= ~HIDETREE;
			objc_draw(tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3]);
			return;
		}
		if(exitobj==ZOOMOUT && (fabs(xmin)<HUGE_VAL/2 &&fabs(xmax)<HUGE_VAL/2&&fabs(ymin)<HUGE_VAL/2&&fabs(ymax)<HUGE_VAL/2))
		{
			xmin=round(xmin*2); xmax=round(xmax*2); ymin=round(ymin*2); ymax=round(ymax*2);
			redraw_window(1);	/* Clear the screen first */
			redraw_window(5);	/* Display the "graph" */
			 
			tree[exitobj].ob_flags &= ~HIDETREE;
			objc_draw(tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3]);
			return;
		}
		if(n==synliga_rader-1  && ( extent[2] >= tree[RUTAN].ob_width - 2*teckenbredd ) )
		{
			n=0;
			while(n<synliga_rader)
			{
				strcpy(s[n],s[n+1]);
				n++;
			}
			memset(s[synliga_rader-1],0,TECKEN);
			scrollVector[n]=1;	/* Sets to one if connected to row below */
			n=synliga_rader-1;
			posi=0;
			redraw_window(2);
		}
		else if( ( extent[2] >= tree[RUTAN].ob_width - 2*teckenbredd ) )
		{
			s[n][ strlen(s[n]) ]=0;   /* To put the last zero so it becomes a string */
			scrollVector[n]=1;	/* Sets to one if connected to row below */
			n++;
			memset(s[n],0,TECKEN);
			posi=0;
			redraw_window(2);
		}

		switch (exitobj)
		{
			case ETT:					/* This is where the numeric keypad comes in */
				insert( s[n], "1" );
				break;
			case TVA:
				insert( s[n], "2" );
				break;
			case TRE:
				insert( s[n], "3" );
				break;
			case FYRA:
				insert( s[n], "4" );
				break;
			case FEM:
				insert( s[n], "5" );
				break;
			case SEX:
				insert( s[n], "6" );
				break;
			case SJU:
				insert( s[n], "7" );
				break;
			case ATTA:
				insert( s[n], "8" );
				break;
			case NIO:
				insert( s[n], "9" );
				break;
			case NOLL:
				insert( s[n], "0" );
				break;
			case PLUS:
				insert( s[n], "+" );
				break;
			case MINUS:
				insert( s[n], "-" );
				break;
			case GANGER:
				if(fontpi)
					insert( s[n], "�" );
				else
					insert( s[n], "*" );
				break;
			case DIVISION:
				if(kstate&NKF_ALT)
					insert( s[n], "%" );
				else 
					insert( s[n], "/" );
				break;
			case PUNKT:
				insert( s[n], "." );
				break;
			case B_PARENTES:
				if(kstate&NKF_ALT)
					insert( s[n], "(" );
				else
				{
					insert( s[n], "()" );
					posi--;
				}
				break;
			case S_PARENTES:
				insert( s[n], ")" );
				break;
			case UPPHOJT:
					insert( s[n], "^" );
				break;
			case INVERTERA:
					insert( s[n], "^-1" );
				break;

			case KVADRAT:
					if(fontpi)
						insert( s[n], "�" );
					else
						insert( s[n], "^2" );
				break;

			case ROT:
					if(fontpi)
						insert( s[n], "�()" );
					else
						insert( s[n], "sqrt()" );
				posi--;
				break;
			case SINUS:
				if(kstate&NKF_CTRL)
					insert( s[n], "sinh()" );
				else if(kstate&NKF_ALT)
					insert( s[n], "asin()" );
				else
					insert( s[n], "sin()" );
				posi--;
				break;
			case COSINUS:
				if(kstate&NKF_CTRL)
					insert( s[n], "cosh()" );
				else if(kstate&NKF_ALT)
					insert( s[n], "acos()" );
				else
					insert( s[n], "cos()" );
				posi--;
				break;
			case TANGENS:
				if(kstate&NKF_CTRL)
					insert( s[n], "tanh()" );
				else if(kstate&NKF_ALT)
					insert( s[n], "atan()" );
				else
					insert( s[n], "tan()" );
				posi--;
				break;
			case PIKNAPPEN:
				if(fontpi)
					insert( s[n], "�" );
				else
					insert( s[n], "Pi" );
				break;
			case EXPONENT:
				insert( s[n], "exp()" );
				posi--;
				break;
			case LOGARITM:
				if(kstate&NKF_ALT)
					insert( s[n], "log()" );
				else 
					insert( s[n], "ln()" );
				posi--;
				break;
			case ABSOLUT:
				if(kstate&NKF_CTRL)
					insert( s[n], "floor()" );
				else if(kstate&NKF_ALT)
					insert( s[n], "ceil()" );
				else
					insert( s[n], "abs()" );
				posi--;
				break;
			case HEAV:
				insert( s[n], "heav()" );
				posi--;
				break;
			case RANDOM:
				insert( s[n], "rand()" );
				posi--;
				break;
			case FAKULTET:
				insert( s[n], "!()" );
				posi--;
				break;
			case NDIST:
				insert( s[n], "ndist()" );
				posi--;
				break;

			default:
				break;
		}
		redraw_window(4);
		objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
	}
	if(!cursor)
		redraw_window(3); /* The cursor must follow */
	evnt_timer(50,0);		/* So it doesn't crash if you hold down the button */
}