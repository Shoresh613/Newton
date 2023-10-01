

/* handle_button(mx,my,state, kstate)
 * Tolkar mustryck och vidarebefordrar bubbelhj„lp.
 **********************************************/


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
extern double xmin, xmax, ymin, ymax; /* F”r grafen d†r† */
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
extern void (*vqt_ex)(int handle, char* string, int* extent); /* symbol f”r antingen vqt_f_extent eller vqt_extent */
extern void insert(char* string, char* toBinserted);
extern int economy_calc(void);
extern int graph_dialog(void);
extern void  mouse_on( void );
extern void  mouse_off( void );
extern void eraseExpression(void);	/* Raderar uttrycket mark”ren befinner sig p†. */

extern int extraRedrawWorkaround;	/* En ritning i on”dan under magic, men inte under TOS verkar det som. */

void handle_button( int mx, int my, int state, int kstate )
{
	int exitobj=0;
	int extent[8];
	int tmp_width=0, tmp_height=0, tmp_mx=0, tmp_my=0;
	int test_width=0, test_height=0;     /* Kolla om f”nstret blev st”rre eller mindre */
	int rubbox_width=0, rubbox_height=0;
	int dummy=0, b=0, i=0;
	char* tmp=NULL;
	int j=0, k=0;
	int scrollStart=0,scrollEnd=0, maxw=0;
	double xmin_new=0, xmax_new=0, ymin_new=0, ymax_new=0, val_tmp=0; /* F”r grafen d†r† */
	
	for ( j=0 ; j<synliga_rader ; j++){	/* Fixa undobuffern */
		strcpy(undo[j],s[j]);
		scrollVectorBak[i]=scrollVector[i];
	}
	undoPosi=posi;
	undoN=n;

	vqt_ex ( handle, s[n], extent );
	exitobj=objc_find( tree, MAIN, MAX_DEPTH, mx, my );

	if(state==2 && (exitobj!=-1) && (exitobj!=XCOORDINATE) && (exitobj!=YCOORDINATE)) /* Hj„lptexten, om h”ger musknapp */
	{
		if(exitobj==RUTAN && graph_mode) /* F”r j„vla st”rigt med den */
			return;
		if( bubble_help(mx,my,exitobj) == 0 )
			bubble_open=1;
		return;
	}

	if(cursor)  /* Sl„cker cursorn om den „r p† */
		redraw_window(3);

	if(exitobj==BACKGROUND && graph_mode)	/* Sl†r om till normall„ge */
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

		if( (test_width > w-6) || (test_height > h-6) ){ /* Om f”nstret f”rminskats */
			redraw_window(1);
			if(graph_mode)
				redraw_window(5);
		}
		else if (MagX)
			extraRedrawWorkaround=1;
		mouse_off();
		graf_mouse( ARROW, NULL );
		mouse_on();

		synliga_rader = (tree[RUTAN].ob_height-6)/teckenhojd; /* -2 f”r kanten in†t med 1 pixel */
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
		if(graph_mode){ /* Zooma om graf */
			graf_mkstate(&mx,&my,&dummy,&dummy); /* Ta reda p† var musen „r just nu. */
			mouse_off();
			graf_mouse( POINT_HAND, NULL );
			mouse_on();
			wind_update(BEG_UPDATE);
			graf_rubbox ( mx, my, 1, 1, &rubbox_width, &rubbox_height);
			wind_update(END_UPDATE);
			mouse_off();
			graf_mouse( ARROW, NULL );
			mouse_on();
		/*	if(mx+rubbox_width>XMIN+WIDTH) /* Detta funkar ju inte s† bra, men tanken „r att om det „r draget utanf”r rutan s† ska den fixa till det s† att det inte „r det, eftersom den d† ballar ur */
				rubbox_width=XMIN+WIDTH-mx;
			if(my+rubbox_height>YMIN+HEIGHT)
				rubbox_height=YMIN+WIDTH-my; */

			if(rubbox_width>10 && rubbox_height>10){ /* Om omr†det „r tillr„ckligt stort */
				for ( k=0, val_tmp=xmin, j=abs(WIDTH); k<j; k++, val_tmp+= ((xmax-xmin)/(double)abs(WIDTH)) ) /* Hitta xmin */
				{
					if(k==mx-XMIN){	
						xmin_new=val_tmp; 			
						break;
					}
				}
				for ( k=0, val_tmp=ymin, j=abs(HEIGHT); k<j; k++, val_tmp+= ((ymax-ymin)/(double)abs(HEIGHT)) ) /* Hitta ymax */
				{
					if(k==my-YMAX){	
						ymin_new=val_tmp; 			
						break;
					}
				}
				for ( k=0, val_tmp=xmin, j=abs(WIDTH); k<j; k++, val_tmp+= ((xmax-xmin)/(double)abs(WIDTH)) ) /* Hitta xmax */
				{
					if(k==mx-XMIN+rubbox_width){	
						xmax_new=val_tmp; 			
						break;
					}
				}
				for ( k=0, val_tmp=ymin, j=abs(HEIGHT); k<j; k++, val_tmp+= ((ymax-ymin)/(double)abs(HEIGHT)) ) /* Hitta ymin */
				{
					if(k==my-YMAX+rubbox_height){	
						ymax_new=val_tmp; 			
						break;
					}
				}
				xmin=round(xmin_new); xmax=round(xmax_new); ymin=round(-ymax_new); ymax=round(-ymin_new); /* Konstiga tilldelningar jao, men de funkar hyfsat? Rundar bara f”r att jag inte kommit p† n†t smart s„tt att f† grid att passa med nollan. */
				redraw_window(1); /* Rensa sk„rmen innan ritar nya kurvan */
				redraw_window(5);
			}
			else{	/* Om inte tillr„ckligt stor rubbox, sl† om till normall„ge */
				graph_mode=0;
				strcpy(xcoordinate,"x: ");
				strcpy(ycoordinate,"y: ");
				tree[COORDINATEBOX].ob_flags |= HIDETREE;	/* G”mmer koordinatboxen. */
				redraw_window(1);
			}
		}
		else{ /* Om i normall„ge */
			for ( n=-1 ; n<synliga_rader && !i ; n++ ) /* Vilken rad „r man p†? */
				if( my < tree[MAIN].ob_y + (n*teckenhojd)+tree[RUTAN].ob_y+teckenhojd +6) /* Tv†an kommer fr†n den f”rbaskade outliningen */
					i=1;
			n--; /* n b”rjar p† 0, inte 1 som synliga_rader */
	
			vqt_ex ( handle, s[n], extent );
			if( mx > tree[MAIN].ob_x + tree[RUTAN].ob_x + extent[2])	/* H„r ska den r„kna ut p† vilken position p† raden man klickat */
				posi=(int)strlen(s[n]);
			else
			{
				tmp = (BYTE *) Malloc (TECKEN);
			
				memset(tmp, 0, TECKEN);
				if (!tmp)       /* Om det inte finns tillr„ckligt med minne*/
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
	
			evnt_timer(70,0); /* S† den inte b”rjar dragndrop med en g†ng */
			graf_mkstate(&dummy,&dummy,&b,&dummy);

			vqt_ex ( handle, s[n], extent );
			maxw=extent[2]-extent[0];

			scrollStart=n; /* Kollar antalet rader i uttrycket */
			if(scrollStart>0) {
				while(scrollVector[scrollStart-1] && scrollStart>0){	/* Tar reda p† var uttrycket b”rjar */
					scrollStart--;
				}
			}
			i=scrollStart;
			while(scrollVector[i]){
				vqt_ex ( handle, s[i], extent );	/* Kollar „ven vilken rad som „r l„ngst. */
				if(extent[2]-extent[0]>maxw)
					maxw=extent[2]-extent[0];
				i++;
			}
			scrollEnd=i;

			if(s[n][0]!=0 && b&0x001 && !iconified && (mx < tree[MAIN].ob_x + tree[RUTAN].ob_x + extent[2]) ) /* Om raden inte tom, knappen fortfarande intryckt, musen inte „r till h”ger om uttrycket och inte ikonifierad: Drag'n Drop */
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

				if( (abs(mx-tmp_mx)>teckenhojd/2) || (abs(my-tmp_my)>teckenhojd/2) ) {	/* Detta test funkar inte str†lande. */
					kstate= (kstate >> 8);     /* Konverteras fr†n normaliserat till vanligt format.          */
					graf_mkstate(&mx,&my,&dummy,&dummy); /* Detta k„nns inte s† skoj, men annars r„knas ramens v„nstra punkt som st„llet d„r det sl„pptes och inte musen */
					send_dragndrop(mx, my, kstate);
					posi=(int)strlen(s[n]);
				}
			}
			else
			{
				do{ /* G”r ingenting f”rr„n v„nster musknapp „r sl„ppt */
					graf_mkstate(&dummy,&dummy,&b,&dummy);
				}while(b&1);
			}
		if(!cursor)
			redraw_window(3); /* Cursorn skall h„nga med */
		}
		return;
	}

	if( tree[exitobj].ob_flags & SELECTABLE )
	{
		redraw_window(4);
		if(exitobj==ZOOMIN || exitobj==ZOOMOUT){	/* G”m ikonen f”r att visa den valda som ligger bakom */
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
			tree[COORDINATEBOX].ob_flags |= HIDETREE;	/* G”mmer koordinatboxen. */
			redraw_window(1);
		}
		
		if(exitobj==BAK)
			if( strlen(s[n])>0 && posi>0)
			{
				tmp = (BYTE *) Malloc (TECKEN);
				memset(tmp, 0, TECKEN);
				if (!tmp)       /* Om det inte finns tillr„ckligt med minne*/
				    return;

				if(posi==(int)strlen(s[n]))	/* Om l„ngst ut p† raden */
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
					if( strlen(s[n])==0 && scrollVector[n-1] ) {	/* Om raderna h”rde ihop, bryt kopplingen. */
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
			if (!tmp)       /* Om det inte finns tillr„ckligt med minne*/
			    return;

			if(strlen(s[n])==0){	/* Om raden tom */
				strcpy(s[n],s[n+1]);
				memset(s[n+1],0,TECKEN);
			}
			else {					/* Om inte tom */
				strcpy(tmp,s[n]);
				*(tmp+posi)=0;
				strcat(tmp,s[n]+posi+1);
				strcpy(s[n],tmp);
			}

			if( (posi==(int)strlen(s[n])) && scrollVector[n] ) { /* Om l„ngst ut p† raden och scrollad */
				strcat(s[n],s[n+1]);			/* Flytta upp raden under */
				memset(s[n+1],0,TECKEN);
				scrollVector[n]=0;
				
				i=n+1;
				while(scrollVector[i-1]){	/* Flytta upp resterande rader, om tillh”r samma uttryck */
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
			if(s[n][0]==0)  /* Om raden „r tom, t”m hela f”nstret */
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
				while(scrollVector[n-1] && n>0){	/* Tar reda p† var uttrycket b”rjar */
					n--;
				}
			}
			while(scrollVector[n]){	/* Tar reda p† var uttrycket slutar */
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
			strcpy( s[n], strrev(s[n] ) );          /* V„nder p† talet, bakofram */
			redraw_window(4);
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			redraw_window( 2 );
			return;		
		}
		if(exitobj==ABSO)
		{
			gcvt( fabs( atof(  s[n] )), 15, s[n] ); 	/* Absoluttalet */
			redraw_window(4);									/* R„knar ut clippingen */
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			posi=(int)strlen(s[n]);
			redraw_window( 2 );redraw_window( 3 );
			return;		
		}
		if(exitobj==AVRUNDA && !hex)
		{
			gcvt( round(atof( s[n] )), 15, s[n] ); /* Rundar av till n„rmsta heltal */
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

			if( i!=-1 ) /* D† har man inte tryckt CANCEL */
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
				tree[COORDINATEBOX].ob_flags &= ~HIDETREE;	/* G”mmer koordinatboxen. */
				redraw_window(1);	/* Rensa sk„rmen f”rst */
				redraw_window(5);	/* Visa "grafen" */
			} 
			objc_change(tree, exitobj, 0, clip[0], clip[1], clip[2], clip[3], NORMAL, 1);
			return;
		}
		if(exitobj==ZOOMIN && (fabs(xmin)>1&&fabs(xmax)>1&&fabs(ymin)>1&&fabs(ymax)>1))
		{
			xmin=round(xmin/2); xmax=round(xmax/2); ymin=round(ymin/2); ymax=round(ymax/2);
			redraw_window(1);	/* Rensa sk„rmen f”rst */
			redraw_window(5);	/* Visa "grafen" */
			 
			tree[exitobj].ob_flags &= ~HIDETREE;
			objc_draw(tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3]);
			return;
		}
		if(exitobj==ZOOMOUT && (fabs(xmin)<HUGE_VAL/2 &&fabs(xmax)<HUGE_VAL/2&&fabs(ymin)<HUGE_VAL/2&&fabs(ymax)<HUGE_VAL/2))
		{
			xmin=round(xmin*2); xmax=round(xmax*2); ymin=round(ymin*2); ymax=round(ymax*2);
			redraw_window(1);	/* Rensa sk„rmen f”rst */
			redraw_window(5);	/* Visa "grafen" */
			 
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
			scrollVector[n]=1;	/* Ettst„ller om h”r ihop med raden nedanf”r. */
			n=synliga_rader-1;
			posi=0;
			redraw_window(2);
		}
		else if( ( extent[2] >= tree[RUTAN].ob_width - 2*teckenbredd ) )
		{
			s[n][ strlen(s[n]) ]=0;   /* F”r att s„tte sista nollan s† det blir en str„ng */
			scrollVector[n]=1;	/* Ettst„ller om h”r ihop med raden nedanf”r. */
			n++;
			memset(s[n],0,TECKEN);
			posi=0;
			redraw_window(2);
		}

		switch (exitobj)
		{
			case ETT:					/* H„r kommer det numeriska tangentbordet in */
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
					insert( s[n], "ú" );
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
						insert( s[n], "ý" );
					else
						insert( s[n], "^2" );
				break;

			case ROT:
					if(fontpi)
						insert( s[n], "û()" );
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
					insert( s[n], "ã" );
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
		redraw_window(3); /* Cursorn skall h„nga med */
	evnt_timer(50,0);		/* S† den inte brakar iv„g om man h†ller inne knappen */
}