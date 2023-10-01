
/* handle_message( int mx, int my )
 * Tar hand om AES-meddelanden mellan andra applikationer -> Newton
 * och Screenmanager -> Newton.
 ********************************************************/

#include <stdio.h>
#include <aes.h>
#include <vdi.h>
#include <stdlib.h>
#include <tos.h>
#include <portab.h>
#include <string.h>
#include <vaproto.h>
#include <gemjing.h>
#include <nkcc.h>
#include <xacc.h>
#include "newton.h"
#include "xtras.h"

extern OBJECT *tree;

extern int quit, _app, menu_id, appl_id, syntax_error;
extern int x,y,w,h,iconified,n,max_x,max_y;
extern int tmp_x,tmp_y,tmp_w,tmp_h,fulled_x,fulled_y,fulled_w,fulled_h;
extern int handle, whandle, font, fontsize, cursor_colour, fulled, resized, text_colour, bg_colour, synliga_rader, teckenbredd, teckenhojd, linewidth, cursor, note_id, boxes;
extern int msg[8];
extern char s[RADER][TECKEN];
extern char undo[RADER][TECKEN];
extern int scrollVector[RADER], scrollVectorBak[RADER];
extern char *bubbletext_pointer;
extern GS_INFO gs_info;
extern int gs_partnerid, gs_id;
extern GS_PARTNER *gs_partner[8];
extern int MagX, MiNT;
extern int posi, undoPosi;
extern int bubble_open, box_done;
extern int n, undoN, graph_mode;

extern void redraw_window(int pos/*, GRECT *redraw */);
extern void redrawWindow(int pos, GRECT *redraw ); /* test */
extern void scroll(void);
extern void handle_button( int mx, int my, int state, int kstate );
extern void open_window( void );
extern void handle_keybd( int key, int kstate, int mx, int my );
extern int Get_cookie( long target, long *p_value );
extern void  scrap_write( int cut );
extern void  scrap_read( void );
extern void position_objects(void);
extern void XAccSend(int sendto);
extern char* pre_calculate( void );
extern void receive_dragndrop(void);
extern void addtomenu(void);
extern void change_settings(void);
extern int  choose_font(void);
extern void colour_select( void );
extern void show_info(void);
extern void stguide_help(void);
extern void choose_samples(void);
extern void insert(char* string, char* toBinserted);
extern void (*vqt_ex)(int handle, char* string, int* extent); /* symbol f”r antingen vqt_f_extent eller vqt_extent */

int notclicked = 0;	/* F”r att s„ga till bubble.c om den ska visa menyn  */

void handle_message( void )
{
	long val;
	int key=0,kstate=0;
	int id=0, kunde_anvanda_gs_kommando=0, fontpartner=0;
	int maxx=0,maxy=0,maxw=0,maxh=0,currx=0,curry=0,currw=0,currh=0;
	int extent[8], i=0, j=0, k=0, dummy=0;
	char* command=NULL, *tmp=NULL, *tmp2=NULL;
	char *param=NULL;
	char *commandline=NULL;
	int mx_l=0, my_l=0, note_ok=0; /*Lokala musvaribler.. orkar inte.. */
	int scrollStart=n,scrollEnd=0;
	int exitobj=0;
	char *BGColour=NULL;
	char *GEMJing_No_Wav=NULL;
	char *env=NULL;

	switch ( msg[0] ) {
		case WM_REDRAW:
			if ( msg[3] == whandle ) {
				box_done=1;
				redraw_window( 1/*, (GRECT *)&msg[4] */ );
				wind_get ( 0, WF_TOP, &i,&j,&j,&j); /* Fr†gar vilket som „r ”versta f”nstret */
				if(graph_mode && (i==whandle))
					redraw_window( 5/*, (GRECT *)&msg[4] */ ); /* Rita ut plotten */
			/*	redrawWindow( msg[3], (GRECT *)&msg[4] ); */
				box_done=0;
				}
				if(!cursor)
					redraw_window(3);
		break;

		case WM_TOPPED:
		case WM_NEWTOP:
		case WM_ONTOP:
		case VA_START:
			wind_update(BEG_UPDATE);
			wind_set( whandle, WF_TOP );
			if(iconified) {
				tree[MAIN].ob_x=tmp_x; tree[MAIN].ob_y=tmp_y; tree[MAIN].ob_width=tmp_w; tree[MAIN].ob_height=tmp_h;
				position_objects();
				wind_calc( WC_BORDER, NAME|CLOSER|MOVER|FULLER|ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h );
				wind_set( whandle, WF_UNICONIFY, x-2, y-2, w+4, h+4 );
				iconified=0;
			}
			if(whandle==0) {
				wind_calc( WC_BORDER, NAME|CLOSER|MOVER|FULLER|ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h );
				position_objects();
				open_window();
			}
			wind_update(END_UPDATE);
			if(!cursor)
				redraw_window(3);
		break;

		case AP_TERM:
			if(!_app && MagX)
				menu_unregister(appl_id);
			quit=1;
		break;

		case WM_CLOSED:
			wind_update(BEG_UPDATE);
			if ( msg[3] == whandle ) {
				wind_close( whandle );
				wind_delete( whandle );
				whandle = 0;
			}
			if ( _app )
				quit=1;
			wind_update(END_UPDATE);
		break;

		case WM_MOVED:
			if ( msg[3] == whandle ) {
				wind_update(BEG_UPDATE);
				wind_set( whandle, WF_CURRXYWH,	msg[4], msg[5], msg[6], msg[7] );
				tree[MAIN].ob_x = x = msg[4]+4;
				tree[MAIN].ob_y = y = msg[5]+22;
				wind_update(END_UPDATE);
			}

		break;

		case WM_FULLED:
			if ( msg[3] == whandle ) {
				if(fulled==0 && resized==1) {
					fulled_x=tree[MAIN].ob_x; fulled_y=tree[MAIN].ob_y;
					fulled_w=tree[MAIN].ob_width; fulled_h=tree[MAIN].ob_height;

					wind_get( 0 , WF_WORKXYWH, &maxx,&maxy,&maxw,&maxh );

					env=getenv("SCREENBORDER");
					if(env && (strchr(env,',')==NULL)){
					   form_alert(1,"[0][$SCREENBORDER not properly set.][   OK   ]");
						dummy=5;
					}

					if( env && strlen(env)>0 && dummy!=45) { /* om $SCREENBORDER „r satt */
						tmp=(char*)Malloc(strlen(env)+1);
						memset(tmp,0,sizeof(tmp));
						strcpy(tmp,env);
						tree[MAIN].ob_x = ( maxx += atoi(strtok (tmp,",")) )+10;
						tree[MAIN].ob_y = ( maxy += atoi(strtok (NULL,",")) )+28;
						tree[MAIN].ob_width = ( maxw -= atoi(strtok (NULL,",")) )-tree[MAIN].ob_x-6;
						tree[MAIN].ob_height = ( maxh -= atoi(strtok (NULL,",")) )-tree[MAIN].ob_y+10;
						Mfree(tmp);
					}
					else {
						tree[MAIN].ob_x = maxx+10;
						tree[MAIN].ob_y = maxy+28;
						tree[MAIN].ob_width = maxw-20;
						tree[MAIN].ob_height = maxh-38;
					} 

					/* Kollar s† den inte g†r utanf”r sk„rmen n†nstans. Funkar la s†d„r. */	
				/*	wind_update(BEG_UPDATE);
					wind_calc( WC_BORDER, NAME|CLOSER|MOVER|FULLER|ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h );
					wind_set(whandle,WF_CURRXYWH,x,y,w,h);
					wind_update(END_UPDATE);
					wind_get( whandle , WF_CURRXYWH, &currx,&curry,&currw,&currh );
					tmp=(char*)calloc(1,40);
					sprintf(tmp,"currx:{%i,%i,%i,%i}", currx,curry,currw,currh);
					insert("",tmp);
					n++;
					wind_get( 0 , WF_WORKXYWH, &maxx,&maxy,&maxw,&maxh );
					if(currx<0)
						tree[MAIN].ob_x += abs(currx);
					if(curry<0)
						tree[MAIN].ob_y += abs(curry);
					wind_update(BEG_UPDATE);
					wind_calc( WC_BORDER, NAME|CLOSER|MOVER|FULLER|ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h );
					wind_set(whandle,WF_CURRXYWH,x+2,y+2,w-4,h-4);
					wind_update(END_UPDATE);
					wind_get( whandle , WF_CURRXYWH, &currx,&curry,&currw,&currh );
					i=currw-maxw;
					if(i>0)
						tree[MAIN].ob_width -= abs(i);
					i=currh-maxh;
					if(i>0)
						tree[MAIN].ob_height -= abs(i);
					sprintf(tmp,"currx:{%i,%i,%i,%i}", currx,curry,currw,currh);
					insert("",tmp);
					n++;
					sprintf(tmp,"maxx:{%i,%i,%i,%i}", maxx,maxy,maxw,maxh);
					insert("",tmp); n++;
					free(tmp); */
					fulled=1;resized=0;
				}
				else {
					tree[MAIN].ob_x=fulled_x; tree[MAIN].ob_y=fulled_y;
					tree[MAIN].ob_width=fulled_w; tree[MAIN].ob_height=fulled_h;
					fulled=0;resized=1;
				}
				position_objects();
				wind_update(BEG_UPDATE);
				wind_calc( WC_BORDER, NAME|CLOSER|MOVER|FULLER|ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h );
				wind_set(whandle,WF_CURRXYWH,x-2,y-2,w+4,h+4);
				wind_update(END_UPDATE);
				redraw_window(2);
				if(!cursor)
					redraw_window(3);
			}
		break;

		case WM_SIZED:
			tree[MAIN].ob_width=msg[6];
			tree[MAIN].ob_height=msg[7];
			position_objects();
			wind_update(BEG_UPDATE);
			wind_calc( WC_BORDER, NAME|CLOSER|MOVER|FULLER|ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h );
			wind_set(whandle,WF_CURRXYWH,x-2,y-2,w+4,h+4);
			wind_update(END_UPDATE);
			redraw_window(2);
			if(!cursor)
				redraw_window(3);
		break;

		case AC_OPEN:
			wind_update(BEG_UPDATE);
			wind_calc( WC_BORDER, NAME|CLOSER|MOVER|FULLER|ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h );
			position_objects();
			open_window( );
			wind_update(END_UPDATE);
		break;

		case AC_CLOSE:
			if ( msg[4] == menu_id ) {
				wind_update(BEG_UPDATE);
				wind_close( whandle );
				wind_delete( whandle );
				wind_update(END_UPDATE);
				whandle = 0;
			}
		break;

		case WM_ICONIFY:
			if ( msg[3] == whandle ) {	
				tmp_x=tree[MAIN].ob_x; tmp_y=tree[MAIN].ob_y; tmp_w=tree[MAIN].ob_width; tmp_h=tree[MAIN].ob_height;
				wind_update(BEG_UPDATE);
				wind_set( whandle, WF_ICONIFY, msg[4], msg[5], msg[6], msg[7] );
				wind_update(END_UPDATE);
				iconified=1;
			}
		break;

		case WM_UNICONIFY:
			if ( msg[3] == whandle ) {
				wind_update(BEG_UPDATE);
				wind_set( whandle, WF_UNICONIFY, msg[4], msg[5], msg[6], msg[7] );
				wind_update(END_UPDATE);
				iconified=0;
			}
		break;

		case BUBBLEGEM_ACK:
			bubbletext_pointer = *(BYTE **) &msg[5];
			if (bubbletext_pointer)
			   Mfree(bubbletext_pointer);
			if (Get_cookie ('BGEM', &val))	/* Att detta kollas har antagligen med Freedom-bubblorna att g”ra. */
				bubble_open=0;
		break;
		case BA_BUBBLE_CLOSED:
			bubble_open=0;
		break;

		case BUBBLEGEM_REQUEST:
		case BA_BUBBLE_REQUEST:
			if(bubble_open)
				break;
			notclicked=1;
			handle_button(msg[4],msg[5],2,0);
			notclicked=0;
		break;

		case FONT_CHANGED:
			font=msg[4];
			fontsize=msg[5];
			text_colour=msg[6];
			fontpartner=msg[1];
			redraw_window(2/*, (GRECT *) &msg[4]*/);
			msg[0]=FONT_ACK;
			msg[1]=appl_id;
			msg[2]=0;
			msg[3]=1;
			msg[4]=0;msg[5]=0;msg[6]=0;msg[7]=0;
			if( appl_write( fontpartner, 16, msg ) == 0 ) /* Om det inte funkade */
				return;                                     /* G”r ingenting.      */
			if(!cursor)
				redraw_window(3);

			break;

		case AV_SENDKEY:
			key=nkc_tconv(msg[4]);
			kstate=msg[3]>>8;
			handle_keybd( key, kstate, 0, 0 );
			break;

		case AV_SENDCLICK:
			kstate=msg[6]>>8;
			handle_button( msg[3],msg[4],msg[5],kstate );
			break;

		case VA_DRAGACCWIND: /* Om n†gonting dragits till Newton, „nnu bara clipbordet */
			mx_l=msg[4]; my_l=msg[5];
			exitobj=objc_find( tree, MAIN, MAX_DEPTH, mx_l, my_l );
			tmp=AVMSG2STR(msg,6);

			if(exitobj==RUTAN) {
				scrp_read(tmp2); /* Klippbordets s”kv„g */
				if( (strcmp( strlwr(tmp), strlwr(tmp2) ) == 0) || (strstr(strlwr(tmp),"scrap.txt") !=NULL ) ) {
					for ( n=-1 ; n<synliga_rader && !i ; n++ ) /* Vilken rad „r man p†? */
						if( my_l < tree[MAIN].ob_y + (n*teckenhojd)+tree[RUTAN].ob_y+teckenhojd +6) /* Tv†an kommer fr†n den f”rbaskade outliningen */
							i=1;
					n--; /* n b”rjar p† 0, inte 1 som synliga_rader */

					if(cursor)
						redraw_window(3); /* Ta bort cursorn */
					scrap_read();
					redraw_window(2);redraw_window(3);
				}
			}
			break;

		case GS_REQUEST:        /* H„r b”rjar allt jox med GEMScript */
			if(msg[7]==ADDTOMENU || msg[7]==NOTEOPEN||	/* Om f”reslaget GEMScript-Id redan anv„nds. */
			(msg[7]==gs_partner[0]->gs_id && gs_partner[0]->gs_partnerid!=msg[1]) || /* M†ste „ven kolla av med appl_id p† dem, s† de kan k”ra mer „n en session */
			(msg[7]==gs_partner[1]->gs_id && gs_partner[1]->gs_partnerid!=msg[1]) || 
			(msg[7]==gs_partner[2]->gs_id && gs_partner[2]->gs_partnerid!=msg[1]) || 
			(msg[7]==gs_partner[3]->gs_id && gs_partner[3]->gs_partnerid!=msg[1]) || 
			(msg[7]==gs_partner[4]->gs_id && gs_partner[4]->gs_partnerid!=msg[1]) || 
			(msg[7]==gs_partner[5]->gs_id && gs_partner[5]->gs_partnerid!=msg[1]) || 
			(msg[7]==gs_partner[6]->gs_id && gs_partner[6]->gs_partnerid!=msg[1]) || 
			(msg[7]==gs_partner[7]->gs_id && gs_partner[7]->gs_partnerid!=msg[1])) {

				i=msg[1];
				msg[0]=GS_REPLY;
				msg[1]=appl_id;
				msg[2]=0;
				msg[3]=(int)(((long) &gs_info >> 16) & 0x0000ffff); /* S† h„r kan man alltid g”ra, d† */
				msg[4]=(int)((long) &gs_info & 0x0000ffff);         /* det handlar om en pekare.     */
				msg[5]=0;
				msg[6]=2;	/* S„ger att man „r upptagen GEMScriptm„ssigt */
				if( appl_write( i, 16, msg ) == 0 ) /* Om det inte funkade */
					break;                           /* G”r ingenting.      */
				return;
			}
			for (j = 0; gs_partner[j]->gs_partnerid!=-1 && j<8 ; j++ );
			if(j!=7){
				gs_partner[0]->gs_partnerid=msg[1]; /* S† man vet vart det skall skickas */
				gs_partner[0]->gs_id=msg[7];        /* Beh”vs vid GS_QUIT                */
				msg[0]=GS_REPLY;
				msg[1]=appl_id;
				msg[2]=0;
				msg[3]=(int)(((long) &gs_info >> 16) & 0x0000ffff); /* S† h„r kan man alltd g”ra, d† */
				msg[4]=(int)((long) &gs_info & 0x0000ffff);         /* det handlar om en pekare.     */
				msg[5]=0;
				msg[6]=0;
				if( appl_write( gs_partner[0]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
					break;                                     /* G”r ingenting.      */
			}
			else {
				i=msg[1]; /* Om g†tt igenom hela strukten s† klarar Newton inte av fler f”rbindelser. */
				msg[0]=GS_REPLY;
				msg[1]=appl_id;
				msg[2]=0;
				msg[3]=(int)(((long) &gs_info >> 16) & 0x0000ffff); /* S† h„r kan man alltd g”ra, d† */
				msg[4]=(int)((long) &gs_info & 0x0000ffff);         /* det handlar om en pekare.     */
				msg[5]=0;
				msg[6]=1;	/* S„ger att man „r upptagen GEMScriptm„ssigt */
				if( appl_write( i, 16, msg ) == 0 ) /* Om det inte funkade */
					break;                           /* G”r ingenting.      */
			}
			break;

		case GS_QUIT:        /* Testing testing */
			for (j = 0; gs_partner[j]->gs_partnerid!=msg[7] && j<8 ; j++ );
			gs_partner[j]->gs_partnerid=-1; /* S† att nya kommandon kan tas emot igen */
			gs_partner[j]->gs_id=-1;
			break;

		case GS_COMMAND:
			command = (BYTE *) Malloc (256);
			command = AVMSG2STR(msg,3);
			command = strlwr(command);
			if(strcmp(command,"appgetlongname")==0) {
				for (j = 0; gs_partner[j]->gs_id!=msg[7] && j<8 ; j++ );
				msg[0]=GS_ACK;
				msg[1]=appl_id;
				msg[2]=0;
				AVSTR2MSG(msg,5,"Newton");
				msg[7]=GSACK_OK;
				if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
					break;                                     /* G”r ingenting.      */
				return;
			}
			if( (strcmp(command,"close")==0) || (strcmp(command,"quit")==0 )) {
				quit=1;
				kunde_anvanda_gs_kommando=1; }

			if(strcmp(command,"copy")==0) {
				scrap_write(0);
				kunde_anvanda_gs_kommando=1;}
			if(strcmp(command,"cut")==0) {
				scrap_write(1);
				kunde_anvanda_gs_kommando=1;}
			if(strcmp(command,"saveundobuffer")==0) {
				for ( k=0 ; k<synliga_rader ; k++) {	/* Fixa undobuffern */
					strcpy(undo[k],s[k]);
					scrollVectorBak[i]=scrollVector[i];
				}
				undoPosi=posi;
				undoN=n;
				redraw_window( 2 );
				kunde_anvanda_gs_kommando=1;}
			if(strcmp(command,"restoreundobuffer")==0)	/* Ta tillbaka undobuffern */
			{
				for ( i=0 ; i<synliga_rader ; i++){
					strcpy(s[i],undo[i]);
					scrollVector[i]=scrollVectorBak[i];
				}
				posi=undoPosi;
				n=undoN;
				redraw_window(2);redraw_window(3);
				kunde_anvanda_gs_kommando=1;}
			if(strcmp(command,"delete")==0)
			{
				if(s[n][0]==0){  /* Om raden „r tom, t”m hela f”nstret */
					memset(s, 0, RADER*TECKEN);
					n=0;
				}
				else
					memset(s[n],0,TECKEN);
				posi=0;
				if(cursor)  /* Sl„cker cursorn om den „r p† */
					redraw_window(3);
				redraw_window( 2 );
				kunde_anvanda_gs_kommando=1;
			}
			if(strcmp(command,"paste")==0)
			{
				if(cursor)  /* Sl„cker cursorn om den „r p† */
					redraw_window(3);
				scrap_read();
				redraw_window(0);
				kunde_anvanda_gs_kommando=1;
			}
			if(strcmp(command,"calculate")==0)
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
				if(!cursor)
					redraw_window(3);
				kunde_anvanda_gs_kommando=1;
			}
			if(strcmp(command,"insertexpression")==0)
			{
				param=command+strlen(command)+1;
				if(cursor)  /* Sl„cker cursorn om den „r p† */
					redraw_window(3);

				insert(s[n],param);
				redraw_window( 2 );

				kunde_anvanda_gs_kommando=1;
			}
			if(strcmp(command,"setcursorposition")==0)
			{
				param=command+strlen(command)+1;
				if(cursor)  /* Sl„cker cursorn om den „r p† */
					redraw_window(3);

				if(param!=NULL)
				{
					k=atoi(param); /* Tempv„rde */
					if( k > strlen(s[n]))
						k=(int)strlen(s[n]);
					posi=k;
					kunde_anvanda_gs_kommando=1;
				}
			}
			if(strcmp(command,"cursorup")==0)
			{
				param=command+strlen(command)+1;
				if(cursor)  /* Sl„cker cursorn om den „r p† */
					redraw_window(3);

				if(param!=NULL)
				{
					k=atoi(param); /* Tempv„rde */
					while(n!=0 && k>0){
						n--;
						k--;
					}
				}
				else if(n>0)
					n--;
				kunde_anvanda_gs_kommando=1;
			}
			if(strcmp(command,"cursordown")==0)
			{
				param=command+strlen(command)+1;
				if(cursor)  /* Sl„cker cursorn om den „r p† */
					redraw_window(3);

				if(param!=NULL)
				{
					k=atoi(param); /* Tempv„rde */
					while(k>0){
						scroll(); /* Om p† sista raden, scrolla */
						n++;
						k--;
					}
				}
				else { /* Varf”r funkar inte den h„r? */
					scroll(); /* Om p† sista raden, scrolla */
					n++;
				}
				kunde_anvanda_gs_kommando=1;
			}
			if(strcmp(command,"getexpression")==0)
			{
				if(s[n][0]!=0)
				{
					for (j = 0; gs_partner[j]->gs_id!=msg[7] && j<8 ; j++ );
					msg[0]=GS_ACK;
					msg[1]=appl_id;
					msg[2]=0;
					AVSTR2MSG(msg,5,s[n]);
					msg[7]=GSACK_OK;
					if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
						return;                                     /* G”r ingenting.      */
					return;
				}
			}

			if(strcmp(command,"menuselected")==0)
			{
				param=command+strlen(command)+1;
				if(strcmp(param,"2")==0)
					choose_font();
				if(strcmp(param,"3")==0) /* Det st†r 3 vid settings, typ */
					change_settings();
				if(strcmp(param,"1")==0)
					choose_samples();
				if(strcmp(param,"4")==0)
					colour_select();
				if(strcmp(param,"6")==0)
					show_info();
				if(strcmp(param,"7")==0)
					stguide_help();
				kunde_anvanda_gs_kommando=1;
			}
			if(strcmp(command,"checkcommand")==0)
			{
				tmp=(char*)Mxalloc(1,0|MGLOBAL);
				strcpy(tmp,"0");	/* Om man inte k„nner till kommandot. */
				param=command+strlen(command)+1;
				param = strlwr(param);
				i=0;
				if(strcmp(param,"appgetlongname")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"close")==0) /* Det st†r 3 vid settings, typ */
					strcpy(tmp,"1");
				if(strcmp(param,"quit")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"copy")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"cut")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"paste")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"delete")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"calculate")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"insertexpression")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"getexpression")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"cursorup")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"cursordown")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"setcursorposition")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"saveundobuffer")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"restoreundobuffer")==0)
					strcpy(tmp,"1");
				if(strcmp(param,"checkcommand")==0)
					strcpy(tmp,"1");
				kunde_anvanda_gs_kommando=1;

				for (j = 0; gs_partner[j]->gs_id!=msg[7] && j<8 ; j++ );
				msg[0]=GS_ACK;
				msg[1]=appl_id;
				msg[2]=0;
				AVSTR2MSG(msg,5,tmp);
				msg[7]=GSACK_OK;
				if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
					return;                                     /* G”r ingenting.      */
				evnt_timer(100,0); /* Ge den tid att fatta */
				Mfree(tmp);
				return;
			}
			if(strcmp(command,"getallcommands")==0) {
				kunde_anvanda_gs_kommando=1;
				tmp=(char*)Mxalloc(512,0|MGLOBAL);
				sprintf(tmp,"AppGetLongName\1Close\1Quit\1Copy\1Cut\1Paste\1Delete\1Calculate\1InsertExpression <expression>\1GetExpression\1CursorUp <nbrOfRows>\1CursorDown <nbrOfRows>\1SetCursorPosition <characters>\1SaveUndoBuffer\1");
				strcat(tmp,"RestoreUndoBuffer\1CheckCommand <command>\1GetAllCommands\1\1");

				i=(int)strlen(tmp);
				for ( ; i>0 ; i-- ) {
					if(*(tmp+i)=='\1')
						*(tmp+i)='\0';
				}

				for (j = 0; gs_partner[j]->gs_id!=msg[7] && j<8 ; j++ );
				msg[0]=GS_ACK;
				msg[1]=appl_id;
				msg[2]=0;
				AVSTR2MSG(msg,5,tmp);
				msg[7]=GSACK_OK;
				if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
					return;                                     /* G”r ingenting.      */
				evnt_timer(100,0); /* Ge den tid att fatta */
				Mfree(tmp);
				return;
			}
			
			for (j = 0; gs_partner[j]->gs_id!=msg[7] && j<8 ; j++ );
			msg[0]=GS_ACK;
			msg[1]=appl_id;
			msg[2]=0;
			AVSTR2MSG(msg,5,NULL);

			if(kunde_anvanda_gs_kommando)
				msg[7]=GSACK_OK;
			else
				msg[7]=GSACK_UNKNOWN;

			if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
				break;                                     /* G”r ingenting.      */

			if(!cursor)
				redraw_window(3);
			break;
		case GS_ACK:
			Mfree(command);Mfree(commandline);
			break;

		case GS_REPLY:
			if(msg[7]==134) /* NOTEOPEN */
			{
				for (j = 0; gs_partner[j]->gs_id!=NOTEOPEN && j<8 ; j++ );
				commandline = (BYTE *) Mxalloc (TECKEN*20,0|MGLOBAL);
				memset(commandline, 0, sizeof(commandline));

				graf_mkstate ( &mx_l, &my_l, &dummy, &dummy );

				while(note_ok==0) {
					sprintf(commandline,"NoteExist\1-i %i", note_id); /* Kollar om en lapp med samma ID redan finns, detta f”r att lappen skall kunna raderas n„r man drar tillbaka den till Newton. Detta hade ju funkat om jinnee hade skickat tillbaka lappen med GEMScript, men den g”r det ju via D&D ist„llet... */
					i=(int)strlen(commandline);

					for ( ; i>0 ; i-- ) {
						if(*(commandline+i)=='\1')
							*(commandline+i)='\0';
					}

					msg[0]=GS_COMMAND;
					msg[1]=appl_id;
					msg[2]=0;
					AVSTR2MSG(msg,3,commandline);
					msg[5]=0;
					msg[6]=0;
					msg[7]=134; /* Har jag satt NOTEOPEN till */

					if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
						return;                                     /* G”r ingenting.      */

					evnt_mesag(msg); /* V„ntar p† att jinnee ska svara */

					tmp=AVMSG2STR(msg,5);
					if(strcmp(tmp,"1")!=0) /* Om lappen inte redan finns */
						note_ok=1;
					else
						note_id++;
				} /* Nu skall ID:t p† notislappen kunna godtas */

				tmp = Malloc(TECKEN*20);	/* Anv„nder h„r f”r att bygga upp flerradigt uttryck */
				memset(tmp,0,sizeof(tmp));

				i=n;
				if(i>0) {
					while(scrollVector[i-1] && i>0){	/* Tar reda p† var uttrycket b”rjar */
						i--;
					}
				}
				scrollStart=i;
				sprintf(commandline,"NoteOpen\1-i %i\1-b %i\1-l %i\1-x %i\1-y %i\1-f %i\1-p %i\1-t %i\1%s\1",note_id, bg_colour, linewidth, mx_l, my_l, font, fontsize, text_colour, s[i]);	/* F”rsta raden med parametrar f”r sj„lva lappen f”rst. */
				while(scrollVector[i]){ /* Kopiera in alla resterande rader som h”r till uttrycket i bufferten. */
					sprintf(commandline+strlen(commandline),"-f %i\1-p %i\1-t %i\1%s\1", font, fontsize, text_colour, s[i+1]);
					i++;
				}
				strcat(commandline,"\1"); /* Avslutande dubbelnolla */
				scrollEnd=i;
				
				Mfree(tmp);
				if(boxes)
				{
					vqt_ex ( handle, s[n], extent );
					graf_growbox(tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +2,tree[MAIN].ob_y + (n*teckenhojd)+tree[RUTAN].ob_y + teckenhojd/4, extent[2]-extent[0], teckenhojd, mx_l-(extent[2]-extent[0])/2, my_l-(teckenhojd)/2, extent[2]-extent[0],teckenhojd+teckenhojd/4+linewidth);
				}

				kstate=nkc_kstate();
				if(kstate&NKF_CTRL) /* Om control, flytta raden */
				{
					while(scrollStart<=scrollEnd){
						memset(s[scrollStart],0,TECKEN);
						scrollVector[scrollStart]=0;
						scrollStart++;
					}
					memset(s[n],0,TECKEN); /* Raderar raden */
					scrollVector[scrollStart]=0;
					redraw_window( 2 );
				}
				i=(int)strlen(commandline);

				for ( ; i>0 ; i-- )
				{
					if(*(commandline+i)=='\1')
						*(commandline+i)='\0';
				}

				msg[0]=GS_COMMAND;
				msg[1]=appl_id;
				msg[2]=0;
				AVSTR2MSG(msg,3,commandline);
				msg[5]=0;
				msg[6]=0;
				msg[7]=134; /* Har jag satt NOTEOPEN till */

				if( appl_write( gs_partner[j]->gs_partnerid, 16, msg ) == 0 ) /* Om det inte funkade */
					break;                                     /* G”r ingenting.      */
			}

			if(msg[7]==121) /* ADDTOMENU */
				addtomenu();

			if(!cursor)
				redraw_window(3);
			break;

		case COLOR_ID:
			for ( j=-1 ; j<synliga_rader && !i ; j++ ) /* Vilken rad „r man p†? */
				if( msg[4] < tree[MAIN].ob_y + (j*teckenhojd)+tree[RUTAN].ob_y+teckenhojd +4) /* Tv†an kommer fr†n den f”rbaskade outliningen */
					i=1;
			j--; /* j b”rjar p† 0, inte 1 som synliga_rader */

			vqt_ex ( handle, s[j], extent );

			if( (msg[3] > (tree[MAIN].ob_x + tree[RUTAN].ob_x + extent[2] +3 +teckenbredd/7) && (objc_find(tree, MAIN, MAX_DEPTH, msg[3], msg[4]) == RUTAN) )) /* Bakgrunden */
			{
				if( msg[5] > 15 )
				{
					rsrc_gaddr( 5, BGCOLOUR, &BGColour );
					wind_update(BEG_UPDATE);
					form_alert( 1, BGColour);
					wind_update(END_UPDATE);
				}

				else
				{
					bg_colour=msg[5];
					redraw_window(2);
				}
			}
			else if( (msg[3] < (tree[MAIN].ob_x + tree[RUTAN].ob_x + extent[2]) && (objc_find(tree, MAIN, MAX_DEPTH, msg[3], msg[4]) == RUTAN) )) /* Texten */
			{
				text_colour=msg[5];
				redraw_window(2);
			}
			else /* Cursorn */
			{
				cursor_colour=msg[5];
				redraw_window(3);
			}
			if(!cursor)
				redraw_window(3);
			break;
		case ACC_KEY: /* Tar emot tangenttryckningar via XACC2 */
			handle_keybd( msg[3], msg[4]>>8, 0, 0 );
			id=msg[1];
			msg[0]=ACC_ACK;
			msg[1]=appl_id;
			msg[2]=0;
			msg[3]=1; /* 1 om man hade nytta av det, 0 annars */
			msg[4]=0;
			msg[5]=0;
			msg[6]=0;
			msg[7]=0;
			if( appl_write( id, 16, msg ) == 0 ) /* Om det inte funkade */
				break;                           /* G”r ingenting.      */

			break;
		case ACC_TEXT: /* Tar emot text via XACC2 */
			id=msg[1];
			tmp=AVMSG2STR(msg,4);
			redraw_window(2);
			insert(s[n],tmp);
			msg[0]=ACC_ACK;
			msg[1]=appl_id;
			msg[2]=0;
			msg[3]=1; /* 1 om man hade nytta av det, 0 annars */
			msg[4]=0;
			msg[5]=0;
			msg[6]=0;
			msg[7]=0;
			if( appl_write( id, 16, msg ) == 0 ) /* Om det inte funkade */
				break;                           /* G”r ingenting.      */
			if(!cursor)
				redraw_window(3);
			break;
		case AP_DRAGDROP:
			receive_dragndrop();
			redraw_window(2);
			redraw_window(3);
			break;
		case GEMJING_RETURN:
			if(msg[3]!=RT_OKAY)
			{
				rsrc_gaddr( R_STRING, WAV, &GEMJing_No_Wav );
				wind_update(BEG_UPDATE);
				form_alert( 1, GEMJing_No_Wav );
				wind_update(END_UPDATE);
			}
			break;

		default:
		break;
	}
}