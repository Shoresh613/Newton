

/* bubble_help(mx,my,exitobj)
 * If a bubble help server is installed, bubble help is displayed.
 * Returns non-zero if it's the background.
 *****************************************************/


#include <aes.h>
#include <vdi.h>
#include <portab.h>
#include <tos.h>
#include <stdlib.h>
#include <vaproto.h>
#include <string.h>
#include <nkcc.h>
#include "newton.h"
#include "xtras.h"

extern int appl_id;
extern int msg[8];
extern int iconified, cursor;
extern int bubble_open, notclicked;

extern int quit, font, whandle, fontpi;
extern int n, posi;
extern int MagX, MiNT;
extern char s[RADER][TECKEN];

extern OBJECT *popup_tree, *popup_tree_formulas;
extern OBJECT *settings, *tree;

extern int scrap_write(int cut);
extern void scrap_read(void);
extern void redraw_window( int all );
extern int choose_font(void);
extern void stguide_help(void);
extern void change_settings(void);
extern void show_info(void);
extern void av_sendmesg(int key, int kstate, int message);
extern void formulas(int mx, int my);
extern void colour_select(void);
extern void choose_samples(void);
extern void insert(char* string, char* toBinserted);
extern char* economy_calc(void);

int bubble_help(int mx, int my, int exitobj)
{
	WORD bubble_id=0;
	BYTE *bubble_text=NULL;
	int popupval=0, kstate=0;
	void *flyinf=NULL;
	GRECT bg;

	if(exitobj==-1)
		return 1;

	/* If Mxalloc() is available, set memory protection to "global". */
    /* Never allocate memory for pointers used in rsrc_gaddr! */
    if (MiNT || MagX) /* This memory allocation resulted in 256 bytes lost each time. It may cause issues on other computers, perhaps? */
        bubble_text = (BYTE *)Mxalloc(256, 0 | MGLOBAL);
    else
        bubble_text = (BYTE *)Malloc(256);

    if (!bubble_text) /* If there isn't enough memory */
        return 1;

	memset(bubble_text,0,sizeof(bubble_text));

	switch (exitobj)
	{
		case LIKAMED:
			rsrc_gaddr( 5, BUBBLE_ENTER, &bubble_text );
			break;
		case RADERA:
			rsrc_gaddr( 5, BUBBLE_CLEAR, &bubble_text );
			break;
		case BAK:
			rsrc_gaddr( 5, BUBBLE_BACK, &bubble_text );
			break;
		case DEL:
			rsrc_gaddr( 5, BUBBLE_DEL, &bubble_text );
			break;
		case KVADRAT:
			rsrc_gaddr( 5, BUBBLE_SQUARE, &bubble_text );
			break;
		case ROT:
			rsrc_gaddr( 5, BUBBLE_SQRT, &bubble_text );
			break;
		case UPPHOJT:
			rsrc_gaddr( 5, BUBBLE_POW, &bubble_text );
			break;
		case DIVISION:
			rsrc_gaddr( 5, BUBBLE_DIV, &bubble_text );
			break;
		case RUTAN:
			rsrc_gaddr( 5, BUBBLE_RUTAN, &bubble_text );
			break;		
		case RANDOM:
			rsrc_gaddr( 5, BUBBLE_RAND, &bubble_text );
			break;		
		case FAKULTET:
			rsrc_gaddr( 5, BUBBLE_FAKULTET, &bubble_text );
			break;		
		case ANDRAEKV:
			rsrc_gaddr( 5, BUBBLE_ANDRA, &bubble_text );
			break;		
		case EKONOMI:
			rsrc_gaddr( 5, BUBBLE_ECONOMY, &bubble_text );
			break;		
		case GRAF:
			rsrc_gaddr( 5, BUBBLE_GRAPH, &bubble_text );
			break;		
		case PIKNAPPEN:
			rsrc_gaddr( 5, BUBBLE_PIKNAPP, &bubble_text );
			break;		
		case SINUS:
			rsrc_gaddr( 5, BUBBLE_SIN, &bubble_text );
			break;		
		case COSINUS:
			rsrc_gaddr( 5, BUBBLE_COS, &bubble_text );
			break;		
		case TANGENS:
			rsrc_gaddr( 5, BUBBLE_TAN, &bubble_text );
			break;		
		case EXPONENT:
			rsrc_gaddr( 5, BUBBLE_EXP, &bubble_text );
			break;		
		case LOGARITM:
			rsrc_gaddr( 5, BUBBLE_LOG, &bubble_text );
			break;		
		case ABSOLUT:
			rsrc_gaddr( 5, BUBBLE_ABS, &bubble_text );
			break;		
		case HEAV:
			rsrc_gaddr( 5, BUBBLE_HEAV, &bubble_text );
			break;		
		case VAEND:
			rsrc_gaddr( 5, BUBBLE_VAEND, &bubble_text );
			break;		
		case ABSO:
			rsrc_gaddr( 5, BUBBLE_ABSO, &bubble_text );
			break;		
		case NDIST:
			rsrc_gaddr( 5, BUBBLE_NDIST, &bubble_text );
			break;		
		case AVRUNDA:
			rsrc_gaddr( 5, BUBBLE_AVRUNDA, &bubble_text );
			break;		
		case INVERTERA:
			rsrc_gaddr( 5, BUBBLE_INVERTERA, &bubble_text );
			break;		
		case NEWTON_SIZER:
			rsrc_gaddr( 5, BUBBLE_SIZER, &bubble_text );
			break;
		case ZOOMIN:
			rsrc_gaddr( 5, BUBBLE_ZOOMIN, &bubble_text );
			break;
		case ZOOMOUT:
			rsrc_gaddr( 5, BUBBLE_ZOOMOUT, &bubble_text );
			break;
		case -2:	/* In case of calculation error */
			rsrc_gaddr( 5, BUBBLE_ERROR, &bubble_text );
			break;
		case BACKGROUND: /* If it's the background */
		case 0:
		case KNAPP_BAKGRUND: /* Or button background */
		case NEWTON_LOGO:    /* Or logo */
			if(notclicked){		/* If you have not clicked to bring up the menu, it will not appear */
				Mfree(bubble_text);
				return 1;
			}
			form_center( popup_tree, &bg.g_x, &bg.g_y, &bg.g_w, &bg.g_h );
			form_xdial( FMD_START, bg.g_x,bg.g_y,bg.g_w,bg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h, &flyinf);
			wind_update( BEG_UPDATE );
			popupval=form_popup( popup_tree, mx,my);
			wind_update( END_UPDATE );
			form_xdial( FMD_FINISH, bg.g_x,bg.g_y,bg.g_w,bg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h, &flyinf);
			
			if(popupval==POPUP_CUT)
			{
				kstate=nkc_kstate();
				if(kstate&NKF_SHIFT) /* If everything should be copied */
					scrap_write(3);
				else
					scrap_write(1);
				redraw_window(2); redraw_window( 3 );
			}
			if(popupval==POPUP_COPY)
			{
				kstate=nkc_kstate();
				if(kstate&NKF_SHIFT) /* If everything should be copied */
					scrap_write(2);
				else
					scrap_write(0);
			}
			if(popupval==POPUP_PASTE)
			{
				scrap_read();
				redraw_window(2);redraw_window(3);
			}

			if(popupval==POPUP_FONT)
			{
				font=choose_font();
				redraw_window(2);
			}

			if(popupval==POPUP_SETTINGS)
				change_settings();

			if(popupval==POPUP_SAMPLES)
				choose_samples();

			if(popupval==POPUP_COLOURS)
				colour_select();

			if(popupval==POPUP_HELP)
				stguide_help();

			if(popupval==POPUP_INFO)
				show_info();

			if(popupval==POPUP_QUIT)
			{
				if(!_app) /* If run as accessory */
				{
					wind_close( whandle );
					wind_delete(whandle);
					whandle=0;
					av_sendmesg(0,0, AV_ACCWINDCLOSED );
				}
				else
					quit=1;
			}
			if(popupval==POPUP_FORMULAS)
			{
				if(cursor)
					redraw_window(3);
				formulas(mx,my);
				redraw_window(0);
				redraw_window(3);
			}
			Mfree(bubble_text);
			return 1;
		default:
			rsrc_gaddr( 5, BUBBLE_DEFAULT, &bubble_text );
	}

	if (iconified)
		rsrc_gaddr( 5, BUBBLE_ICONIFIED, &bubble_text );

   bubble_id = appl_find("BUBBLE  ");

   if (bubble_id >= 0)
   {
		msg[0] = BUBBLEGEM_SHOW;
		msg[1] = appl_id;
		msg[2] = 0;
		msg[3] = mx;
		msg[4] = my;
		msg[5] = (WORD)(((LONG) bubble_text >> 16) & 0x0000ffff);
		msg[6] = (WORD)((LONG) bubble_text & 0x0000ffff);
		msg[7] = 0;
		if (appl_write(bubble_id, 16, msg) == 0)
		{
			rsrc_gaddr( 5, SOMETHINGWRONG, &bubble_text );
			form_alert(3,bubble_text);
		}

		evnt_timer(100,0);
		
		if(bubble_text)
			Mfree(bubble_text);
		return 0;
   }
		
	return 1; /* If BubbleGEM isn't installed*/
}

void formulas(int mx, int my)
{
	GRECT lg, bg;
	int popupval=0, tmp=0,tmp2=0,tmp3=0;
	void *flyinf;

	if(!tmp3)	/* If there was some error, like no being able to xdial */
		tmp=0;
	form_center( popup_tree_formulas, &bg.g_x, &bg.g_y, &bg.g_w, &bg.g_h );
	lg.g_x=bg.g_x+(bg.g_w/2); lg.g_y=bg.g_y+(bg.g_h/2); lg.g_w=bg.g_x+(bg.g_w/2)+1; lg.g_h=bg.g_y+(bg.g_h/2)+1; /* centrera lilla ocks� */
	tmp3=appl_getinfo(14,&tmp, &tmp2, &tmp2, &tmp2);	/* Checks if possible to form_xdial */
	wind_update( BEG_UPDATE );
	if(tmp)
		form_xdial( FMD_START, lg.g_x,lg.g_y,lg.g_w,lg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h, &flyinf);
	else
		form_dial( FMD_START, lg.g_x,lg.g_y,lg.g_w,lg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h);
	tmp3=appl_getinfo(9,&tmp2, &tmp, &tmp2, &tmp2);	/* Checks if possible to form_xdial */
	if(tmp && tmp3)	/* If the system can do multitos popups */
		popupval=form_popup( popup_tree_formulas, mx,my);
	wind_update( END_UPDATE );
	form_xdial( FMD_FINISH, bg.g_x,bg.g_y,bg.g_w,bg.g_h,bg.g_x,bg.g_y,bg.g_w,bg.g_h, &flyinf);

	if(popupval==AREACIRCLE)
	{
		if(fontpi)
			insert(s[n],"(�D�)/4");
		else
			insert(s[n],"(Pi*D^2)/4");
	}
	if(popupval==CIRCUMCIRCLE)
	{
		if(fontpi)
			insert(s[n],"�D");
		else
			insert(s[n],"Pi*D");
	}
	if(popupval==AREATRIANGLE)
	{
		if(fontpi)
			insert(s[n],"(B�H)/2");
		else
			insert(s[n],"(B*H)/2");
	}
	if(popupval==AREASPHERE)
	{
		if(fontpi)
			insert(s[n],"4�R�");
		else
			insert(s[n],"4*Pi*R^2");
	}
	if(popupval==VOLUMESPHERE)
	{
		if(fontpi)
			insert(s[n],"(4�R�)/3");
		else
			insert(s[n],"(4*Pi*R^3)/3");
	}
	if(popupval==AREACYLINDER)
	{
		if(fontpi)
			insert(s[n],"2�R�H");
		else
			insert(s[n],"2*Pi*R*H");
	}
	if(popupval==VOLUMECYLINDER)
	{
		if(fontpi)
			insert(s[n],"�R��H");
		else
			insert(s[n],"Pi*R^2*H");
	}
	if(popupval==AREACONE)
	{
		if(fontpi)
			insert(s[n],"�R�S");
		else
			insert(s[n],"Pi*R*S");
	}
	if(popupval==VOLUMECONE)
	{
		if(fontpi)
			insert(s[n],"(�R��H)/3");
		else
			insert(s[n],"(Pi*R^2*H)/3");
	}
	if(popupval==DISTANCE)
	{
		if(fontpi)
			insert(s[n],"Vnull�T+(A�T�)/2");
		else
			insert(s[n],"Vnull*T+(A*T^2)/2");
	}
	if(popupval==SPEED){
		if(fontpi)	
			insert(s[n],"Vnull+A�T");
		else
			insert(s[n],"Vnull+A*T");
	}
	if(popupval==KINETIC)
	{
		if(fontpi)
			insert(s[n],"(M�V�)/2");
		else
			insert(s[n],"(M*V^2)/2");
	}
	if(popupval==COULOMB)
	{
		if(fontpi)
			insert(s[n],"(8.988�10^9)�((Q1�Q2)/R�)");
		else
			insert(s[n],"(8.988*10^9)*((Q1*Q2)/R^2)");
	}
	if(popupval==IMPEDANCE)
	{
		if(fontpi)
			insert(s[n],"1/(�((1/R�)+(1/(��L)-��Cap)�))");
		else
			insert(s[n],"1/(sqrt((1/R^2)+(1/(Omega*L)-Omega*Cap)^2))");
	}
	if(popupval==RELENERGY)
	{
		if(fontpi)
			insert(s[n],"M�C�");
		else
			insert(s[n],"M*C^2");
	}
	if(popupval==RELMASS)
	{
		if(fontpi)
			insert(s[n],"Mnull/(�(1-(V/C)�))");
		else
			insert(s[n],"Mnull/(sqrt(1-(V/C)^2))");
	}
	if(popupval==RELTIME)
	{
		if(fontpi)
			insert(s[n],"Tnull/(�(1-(V/C)�))");
		else
			insert(s[n],"Tnull/(sqrt(1-(V/C)^2))");
	}
	if(popupval==RELLENGTH)
	{
		if(fontpi)
			insert(s[n],"Lnull�(�(1-(V/C)�))");
		else
			insert(s[n],"Lnull*(sqrt(1-(V/C)^2))");
	}
}