
/* Newton. Graphical calculator, (c) 1997-2005 Mikael Folkesson.
 * Since 2023 GPL 3. All functions to do with redraw should
 * remain in this file, otherwise the redraw gets a lot slower
 ***********************************************/

#include <aes.h>
#include <vdi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <tos.h>
#include <portab.h>
#include <xfsl.h>
#include <vaproto.h>
#include <nkcc.h>
/*#include <mt_font.h> */
#include "xacc.h"
#include "newton.h"
#include "xtras.h"
#include "graf.h"
#include "dev\w_xgem.h"

/*extern int _app; */						 /* Started as an app or an acc?*/
/*extern GEMPARBLK _GemParBlk;*/	/* These are available even if not imported */

char	 *Version="v 0.994ž of ";
int whandle = 0;                      /* Handle for the opened window. */
char* title = NULL;                   /* Window title bar. */
int gl_wchar = 0,                     /* Character size and width. */
    gl_hchar = 0,                     /* (important when working in different resolutions or fonts) */
    gl_wbox = 0,                      /* Width of a box. */
    gl_hbox = 0;                      /* Height of a box. */
int posi = 0,                          /* String position. */
    undoPosi = 0;

int x = 0, y = 0, w = 0, h = 0;
int hor = 0, ver = 0, dummy = 0, font = 5031, fontsize = 10, text_colour = BLACK, bg_colour = LWHITE, cursor_colour = 12, icon_size = 0, note_id = 30;
double answer = 0;
int msg[8], number_of_fonts = 0;
char* bubbletext_pointer = NULL;
int monochrome = 0;
int bubble_open = 0, syntax_error = 0;
int iconified = 0;
int quit = 0;
int clip[4];
int fulled = 0, resized = 1;             /* Resized. */
int cursor = 0,                         /* Whether it's currently visible. */
    cursor_blink = 1,                  /* Whether it should blink or be steady. */
    moral_check = 1,
    save_work = 1,
    show_intro = 1,
    rad = 1,                            /* Always start counting in radians. */
    sendkey = 1,
    hex = 0,
    bin = 0,
    dec = 1,
    play_sample = 1,
    linewidth = 1,
    fontpi = 1,
    boxes = 1;                          /* Whether grow boxes should be drawn during D&D. */

int first = 1;                          /* Whether to show the intro. */
int tmp_x = 0, tmp_y = 0, tmp_w = 0, tmp_h = 0, fulled_x = 0, fulled_y = 0, fulled_w = 0, fulled_h = 0;
char s[RADER][TECKEN],                  /* The string to be calculated. */
    undo[RADER][TECKEN];                /* Undo buffer. */
int scrollVector[RADER];                /* Indicates which rows are linked. */
int scrollVectorBak[RADER];             /* For undo buffer. */
int n = 0,                              /* Current row. */
    undoN = 0;
int teckenbredd = 0, teckenhojd = 0;   /* Character height/width of the font in the box. */
int synliga_rader = 0;
int antal_konstanter = 0;               /* Constants loaded from newton.cnf. */

int phys_handle = 0,                    /* GEM and VDI handles. */
    handle = 0;
int max_x = 0, max_y = 0, max_w = 0, max_h = 0; /* Maximum workspace size. */

int appl_id = 0,                        /* Program identification number. */
    menu_id = 0;                        /* ID number in the Desk menu. */
int box_done = 0;                       /* If WM_REDRAW, another window is drawn. */
UWORD nvdi = 0;                         /* If VDI has v_ftext (nvdi >= 3). */
int gs_partnerid = -1, gs_id = -1;      /* GEMScript partner ID & ID msg[7] */
GS_PARTNER* gs_partner[8];
GS_INFO gs_info;                        /* GEMScript info structure. */
FNT_DIALOG* fnt_dialog = NULL;          /* For MagiC font selector, should be invoked before. */

double xmin = 4038, xmax = 0, ymin = 0, ymax = 0; /* For the graph. */
char* ekv = NULL;
int graph_mode = 0, draw_grid = 0, linethgraph = 0;
long grid_distx = 0, grid_disty = 0;
int grafarray[2560];                   /* Can handle drawing with width 1280. */
double coordarray[2560];

char* errortmpstring = NULL;            /* String for calculating the position of the help bubble. */
char* uttryck = NULL;                   /* For cuttryck.c */
char konstant_lista[ANTAL_ENHETER][MAX_TECKEN];
double konstant_storhet[ANTAL_ENHETER];
int MiNT = FALSE, MagX = FALSE;
char* insert_tmp = NULL, *insert_tmp2 = NULL; /* Memory must be allocated here, otherwise, it allocates every time insert() is called. */

extern char* file_startup, *path_startup;   /* These are for samples. */
extern char* strptr_startup, *tmp_startup;   /* In samples.c */
extern char* file_trashcan, *path_trashcan;
extern char* strptr_trashcan, *tmp_trashcan;
extern char* file_language, *path_language;
extern char* strptr_language, *tmp_language;
extern char* file_quit, *path_quit;
extern char* strptr_quit, *tmp_quit;

int extraRedrawWorkaround = 0;          /* Unnecessary drawing under magic but not under TOS, it seems. */

OBJECT* tree = NULL;                    /* Objects in the resource file. */

OBJECT *icon=NULL;
OBJECT *icon_small=NULL;
OBJECT *titlebarIcon=NULL;
OBJECT *info=NULL;
OBJECT *settings=NULL;
OBJECT *standard=NULL;
OBJECT *div_by_zero=NULL;
OBJECT *variable=NULL;
OBJECT *error=NULL;
OBJECT *popup_tree=NULL;
OBJECT *popup_tree_formulas=NULL;
OBJECT *samples_tree=NULL;
OBJECT *andragrad=NULL;
OBJECT *economy=NULL;
OBJECT *graph_dial=NULL;

TEDINFO *x_obspec=NULL, *y_obspec=NULL;	
char* xcoordinate=NULL, *ycoordinate=NULL;

/* Function declarations. Here, functions declared in other source code files are also declared. */

void open_window(void);
int rc_intersect(GRECT *r1, GRECT *r2);
void mouse_on(void);
void mouse_off(void);
void redraw_window(int all);
void handle_message(void);
void handle_button(int mx, int my, int state, int kstate);
void handle_keybd(int key, int kstate, int mx, int my);
void event_loop(void);
char* calculate(char* string);
int choose_font(void);
int Get_cookie(long target, long* p_value); /* Not specific */
int get_NVDI_cookie(long* p_value); /* For NVDI */
int get_cookie(long cookie, xFSL* cval); /* For xFSL */
int bubble_help(int mx, int my, int exitobj);
void stguide_help(void);
void save_settings(int fontid);
void load_settings(void);
void scrap_read(void);
void scrap_write(int cut);
void av_sendmesg(int key, int kstate, int message);
int bad_language(char* string);
void show_info(void);
void change_settings(void);
void position_objects(void);
int do_dialog(OBJECT* dialog);
extern void hex_it(long value, char* result);
extern void bin_it(long value, char* result);
void colour_select(void);
TEDINFO* whichcolour(TEDINFO* obspec); /* Sets the bits on the background */
void send_dragndrop(int mx, int my, int kstate); /* Starts D&D with pipes */
void receive_dragndrop(void);
void error_sound(int sound);
void insert_dragged_text(int my, int kstate, char* text);
void read_constants(void);
void formulas(int mx, int my);
void addtomenu_request(void);
void addtomenu(void);
void initSampleDialogue(void);
void choose_samples(void);
void andragradare(void);
int economy_calc(void);
extern char* preprocess(char* string);

void (*print)(int vwk, int x, int y, char* text); /* Symbol for either v_ftext or v_gtext */
void (*vqt_ex)(int handle, char* string, int* extent); /* Symbol for either vqt_f_extent or vqt_extent */

/* The program starts here. */

int main(void)
{
    int i = 0, code = 0, dummy = 0;
    long val = 0L;
    int work_in[11];
    int work_out[57];
    const char name[] = "Newton\0XDSC\01Calculator\0\0";
    char* ExampleString = NULL;
    static char RSCnamn[] = "newton.rsc";
    static char RSCnamn_stor[] = "NEWTON.RSC"; /* If started from a case-sensitive partition */
    static char* No_RSC = "[3][Cannot find newton.rsc.|If run as an accessory under|TOS, should be in root of|boot drive!][   OK	 ]";
    char* XBRA_Destroyed = NULL;

    appl_id = appl_init();
    if (appl_id != -1)
    {
        if (!rsrc_load(RSCnamn))
        {
            if (!rsrc_load(RSCnamn_stor))
            {
                form_alert(1, No_RSC);
                appl_exit();
                return 1;
            }
        }

        MagX = Get_cookie(MagX_COOKIE, &val); /* Check cookies once for all */
        MiNT = Get_cookie(MiNT_COOKIE, &val);

        if (MagX || MiNT)
            Pdomain(1); /* Can use long filenames, but it's not just that. This might be dangerous? No, check longfnam.txt. */
        strcat(Version, __DATE__);
        wind_get(0, WF_CURRXYWH, &max_x, &max_y, &max_w, &max_h);
        rsrc_gaddr(R_TREE, MAIN, &tree);
        rsrc_gaddr(R_TREE, ICONIFIED, &icon);
        rsrc_gaddr(R_TREE, SMALL_ICONIFIED, &icon_small);
        rsrc_gaddr(R_TREE, POPUP, &popup_tree);
        rsrc_gaddr(R_TREE, SAMPLES, &samples_tree);
        rsrc_gaddr(R_TREE, FORMULAS, &popup_tree_formulas);
        rsrc_gaddr(R_STRING, XBRA_DESTROYED, &XBRA_Destroyed);
        rsrc_gaddr(0, SMALLICON, &titlebarIcon); /* Request address of the icon for xgem-titlebar */

        x_obspec = tree[XCOORDINATE].ob_spec.tedinfo;
        y_obspec = tree[YCOORDINATE].ob_spec.tedinfo;
        xcoordinate = x_obspec->te_ptext;
        ycoordinate = y_obspec->te_ptext;

        strcpy(xcoordinate, "x: ");
        strcpy(ycoordinate, "y: ");
        tree[COORDINATEBOX].ob_flags |= HIDETREE; /* Hide the coordinate box. */

        s[0][0] = (char)(char*)Malloc(sizeof(s));
        undo[0][0] = (char)(char*)Malloc(sizeof(undo));
        konstant_lista[0][0] = (char)(char*)Malloc(sizeof(konstant_lista));
        konstant_storhet[0] = (long)Malloc(ANTAL_ENHETER); /* Should ideally be (double) */
        errortmpstring = (char*)Malloc(20 * TECKEN); /* Should be the number of lines in the current expression */
        ekv = (BYTE*)Malloc(TECKEN);
        grafarray[0] = (int)Malloc(sizeof(grafarray));
        coordarray[0] = (double)(int)Malloc(sizeof(coordarray));
        scrollVector[0] = (int)Malloc(sizeof(scrollVector));
        scrollVectorBak[0] = (int)Malloc(sizeof(scrollVector));
        insert_tmp = (char*)Malloc(TECKEN);
        insert_tmp2 = (char*)Malloc(TECKEN);
        if (MagX || MiNT)
            gs_info.len = (long)Mxalloc(sizeof(gs_info), 0 | MGLOBAL);
        else
            gs_info.len = (long)Malloc(sizeof(gs_info));
        i = 0;
        while (i < 8)
        {
            gs_partner[i] = (GS_PARTNER*)Malloc(sizeof(gs_partner));
            i++;
        }

        uttryck = (char*)Malloc(20 * TECKEN); /* An expression can have 20 lines */

        if (!uttryck) /* Out of memory */
            return 1;

        memset(s, 0, RADER * TECKEN); /* Reset it so you can read it as it was */
        memset(undo, 0, RADER * TECKEN); /* Reset it so you can read it as it was */
        memset(konstant_lista, 0, sizeof(konstant_lista));
        memset(konstant_storhet, 0, sizeof(konstant_storhet));
        memset(errortmpstring, 0, sizeof(errortmpstring));
        memset(uttryck, 0, sizeof(uttryck));
        memset(scrollVector, 0, sizeof(scrollVector));
        memset(scrollVectorBak, 0, sizeof(scrollVector));
        memset(&(gs_info.len), 0, sizeof(gs_info)); /* This might cause some issues */
        
        if (is_xtitle()) /* Recent Windframe Xgem? */
        {
            title = (char*)Mxalloc(256, 0 | MGLOBAL);
            sprintf(title, "\xFF\xFF\xFF\xFF%08lX%s", titlebarIcon, "Newton"); /* Insert the title with an icon */
        }
        else
        {
            if (MagX || MiNT)
                title = (char*)Mxalloc(8, 0 | MGLOBAL);
            else
                title = (char*)Malloc(8);
            strcpy(title, "Newton"); /* Text only */
        }

        nvdi = get_NVDI_cookie(&val);
        /* nvdi = get_NVDI_cookie(); */ /* This cookie check crashed other computers, check mille_testar.txt */

        if (nvdi && (val >= 0x0300))
        {
            print = v_ftext; /* Vector exists after version 3 */
            vqt_ex = vqt_f_extent;
        }
        else
        {
            print = v_gtext;
            vqt_ex = vqt_extent;
        }
        xacc_init(-1, name, (WORD)(ULONG)sizeof(name), 0x0001); /* The last 1 comes from the fact that you can send text or keycode to it */

        gs_info.msgs = 0 | GSM_COMMAND; /* GEMScript stuff */
        gs_info.version = 0x0100;
        gs_info.ext = '.SIC';
        gs_info.len = sizeof(gs_info);

        for (i = 0; i < 8; i++)
        { /* Initialize so that the entire structure is set to -1 */
            gs_partner[i]->gs_partnerid = -1;
            gs_partner[i]->gs_id = -1;
        }

        for (i = 0; i < 10; i++)
            work_in[i] = 1;
        work_in[10] = 2;
        phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
        handle = phys_handle;
        v_opnvwk(work_in, &handle, work_out);
        if (handle != 0)
        {
            max_x = work_out[0];
            max_y = work_out[1];
            if (work_out[13] < 16)
                monochrome = 1; /* Check if running in mono or not */
            if (monochrome)
                bg_colour = WHITE;

            form_center(tree, &tree[MAIN].ob_x, &tree[MAIN].ob_y, &dummy, &dummy);
            tree[MAIN].ob_y = tree[MAIN].ob_y + 12; /* The window ends up in the menu otherwise */
            initSampleDialogue();
            load_settings(); /* Read screen position and settings */
            read_constants();
            wind_calc(WC_BORDER, NAME | CLOSER | MOVER | FULLER | ICONIFIER, tree[MAIN].ob_x, tree[MAIN].ob_y, tree[MAIN].ob_width, tree[MAIN].ob_height, &x, &y, &w, &h);
            position_objects();
            av_sendmesg(0, 0, AV_PROTOKOLL); /* Notify the AV Server that it exists */

            if (!_app || MiNT)
                menu_id = menu_register(appl_id, "  Newton");
            if (vq_gdos())
                number_of_fonts = vst_load_fonts(handle, 0);

            ExampleString = (char*)Mxalloc(16, 0 | MGLOBAL); /* Um... */
            if ((Get_cookie('xFSL', &val) == FALSE) && (Get_cookie('UFSL', &val) == FALSE) && (MagX))
            { /* If no font selector but Magic */
                strcpy(ExampleString, "M/(ï¿½(1-(V/C)ï¿½))");
                fnt_dialog = fnts_create(handle, number_of_fonts, 0 | FNTS_BTMP | FNTS_OUTL | FNTS_MONO | FNTS_PROP, 1, ExampleString, 0L);
            }

if( _app )
{
	graf_mouse( ARROW, NULL );
	vst_point( handle, fontsize, &dummy, &dummy, &teckenbredd, &teckenhojd );
	wind_update(BEG_UPDATE);	/* Call this for every window modification. */
	open_window( );
	wind_update(END_UPDATE);
}

if (MiNT || MagX)
	msg[0] = (int) Mxalloc (sizeof(msg), 0 | MGLOBAL ); /* Global memory so others can read and write. */
else
	msg[0] = (int) Malloc (sizeof(msg));
if (!msg)		 /* If there is not enough memory */
	 return 1;

memset(msg, 0, sizeof(msg));

if( play_sample )
	error_sound( STARTUP_SOUND );
nkc_init(NKI_BHTOS|NKI_NO200HZ, handle, _GemParBlk.global); /* Initialize nkcc */
event_loop( );

if(vq_gdos())
	vst_unload_fonts(handle,0);
code=nkc_kstate();
if(!(code&NKF_ALT)) /* If the alt key is not pressed, save the info file */
	save_settings(font);
v_clsvwk( handle );
}

if((Get_cookie('xFSL',&val)==FALSE) && (Get_cookie('UFSL',&val)==FALSE) && (MagX) ) /* If no font selector but Magic */
	fnts_delete( fnt_dialog, 0 );
rsrc_free();
wind_update(BEG_UPDATE);	/* Must be called even when closing windows. wind_new() takes care of END_UPDATE */
wind_new();
av_sendmesg(0,0, AV_ACCWINDCLOSED );
av_sendmesg(0,0, AV_EXIT );
code=nkc_exit();
if( code == -1 || code == -2 || code == -3 )
	form_alert( 1, XBRA_Destroyed );
xacc_exit();
i=0;
while(gs_partner[i]->gs_partnerid!=-1){
	msg[0]=GS_QUIT;msg[1]=appl_id;msg[2]=0;msg[3]=0;msg[4]=0;msg[5]=0;msg[6]=0;msg[7]=gs_partner[i]->gs_id;
	appl_write(gs_partner[i]->gs_partnerid,16,msg);
	i++;
}
appl_exit();
}
Mfree(msg);Mfree(s);Mfree(undo);Mfree(ExampleString);Mfree(errortmpstring);Mfree(uttryck);Mfree(title);Mfree(ekv);
Mfree(file_startup);Mfree(path_startup);Mfree(strptr_startup);Mfree(tmp_startup);Mfree(file_trashcan);Mfree(path_trashcan);	/* All of these are sample-related. */
Mfree(strptr_trashcan);Mfree(tmp_trashcan);Mfree(file_language);Mfree(path_language);Mfree(strptr_language);Mfree(tmp_language);Mfree(&(gs_info.len));
Mfree(file_quit);Mfree(path_quit);Mfree(strptr_quit);Mfree(tmp_quit);Mfree(insert_tmp);Mfree(insert_tmp2);Mfree(grafarray);Mfree(coordarray);Mfree(scrollVector);Mfree(scrollVectorBak);
i=0;
while(i<7){
	Mfree(gs_partner[i]);
	i++;
}
return 0;
}

void open_window( void )
{
if ( whandle <= 0 )
{
	whandle = wind_create( NAME|CLOSER|MOVER|FULLER|ICONIFIER, 0, 0, max_x + 1, max_y + 20 );
	if ( whandle <= 0 )
		return;

	wind_set( whandle, WF_NAME, title );
	wind_open( whandle, x-2, y-2, w+4, h+4 );
	av_sendmesg(0,0, AV_ACCWINDOPEN );
}
else
	wind_set( whandle, WF_TOP );
}

int rc_intersect( GRECT *r1, GRECT *r2 )
{
int ix=0, iy=0, iw=0, ih=0;

ix = max( r2->g_x, r1->g_x );
iy = max( r2->g_y, r1->g_y );
iw = min( r2->g_x + r2->g_w, r1->g_x + r1->g_w );
ih = min( r2->g_y + r2->g_h, r1->g_y + r1->g_h );

r2->g_x = ix;
r2->g_y = iy;
r2->g_w = iw - ix;
r2->g_h = ih - iy;

return ( ((iw > ix) && (ih > iy) ) );
}

/* -------------------------------------------------------------------- */
/* 		void redraw_window( int all );											*/
/* 			if( all==0)	only draw the string, if(all==2) draw the text window 	*/
/* 			if( all==1 ) draw the entire window.			 	 						  */
/* 			if( all==3) draw or remove the cursor. 							*/
/* 			if( all>3 ) do not draw, just calculate clipping.					*/
/* 			if( all==5 ) draw graphics. 												*/
/* -------------------------------------------------------------------- */

void redraw_window( int all )
{
	int extent[8], pxyarray[4], i=0;
	int dummy=0, tmpbgcolor=0, xaxis=-1, yaxis=-1/*, nollan=0*/;
	TEDINFO *obspec;
	GRECT 	 box,
				work;
	char* tmp=NULL, *tmpekv=NULL, *tmp2=NULL, *chdummy=NULL;
	int j=0, k=0, currpos=0;
	double x=0, x_bak=0, x_bak2=0,temp=0;
	unsigned linestyle=23690;

	linestyle+=20000;	/* To do it like this, otherwise it complains about it being a long */

	if( whandle <= 0 )						/* If no window is open */
		return;

	vswr_mode( handle, MD_TRANS );					 /* set transp mode */
	vst_font( handle, font );
	vst_point( handle, fontsize, &dummy, &dummy, &teckenbredd, &teckenhojd );

	vst_color( handle, text_colour );
	synliga_rader = (tree[RUTAN].ob_height-6)/teckenhojd;
	obspec=tree[RUTAN].ob_spec.tedinfo;
	obspec=whichcolour(obspec);

	if(n>synliga_rader-1) /* Safest, n starts at 0 and not 1. */
		n=synliga_rader-1;
	if(n<0)
		n=0;

	wind_get( whandle, WF_WORKXYWH, &work.g_x, &work.g_y, &work.g_w, &work.g_h );
	wind_get( whandle, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h );

	if(box.g_w==0 && box.g_h==0) /* The window is completely covered */
		return;

	work.g_w = min( work.g_w, max_x - work.g_x );
	work.g_h = min( work.g_h, max_y - work.g_y );

	tree[MAIN].ob_x = work.g_x +2;
	tree[MAIN].ob_y = work.g_y +2;

	wind_update(BEG_UPDATE);
	while ( box.g_w > 0 && box.g_h > 0 )
	{
		clip[0] = box.g_x;	/* Clipping for AES */
		clip[1] = box.g_y;
		clip[2] = box.g_w;
		clip[3] = box.g_h;

		if( rc_intersect( &work, &box ) )
		{
			if( iconified==0 )
			{
				if ( all==0 && !graph_mode)
				{
					clip[2] = box.g_x + box.g_w -22;
					clip[3] = box.g_y + box.g_h - tree[MAIN].ob_height +16 +tree[RUTAN].ob_height; /* Only draw on the disply */
					vs_clip( handle, 1, clip );
					mouse_off( );
					print( handle, tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +2, tree[MAIN].ob_y + (n*teckenhojd)+tree[RUTAN].ob_y+teckenhojd, s[n] );
					mouse_on( );
					vs_clip( handle, 0, clip );
					cursor=0;
				}

				if ( all==1 )
				{	mouse_off( );
					objc_draw( tree, MAIN, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3] );
					mouse_on( );
					cursor=0;
				}

				if ( all==2 || (all==1 && !graph_mode) )
				{
					if(first && show_intro && nvdi)
					{
						tmpbgcolor=bg_colour;
						bg_colour=RED;
						obspec=whichcolour(obspec);
						clip[0]+=22;
						clip[1]+=20;
						clip[2] = box.g_x + box.g_w -22;
						clip[3] = box.g_y + box.g_h - tree[MAIN].ob_height +16 +tree[RUTAN].ob_height;
						objc_draw( tree, RUTAN, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3] );
						if(vst_font( handle, 5005 )==0) /* Unless Swiss 721 exists*/
							if(vst_font( handle, 5142 )==0) /* Try Futura */
								if(vst_font( handle, 5768 )==0) /* Try Handel Gothic */
									break; /* If none of them are installed, don't show the splash */
						vst_color( handle, WHITE );

						tmp = (BYTE *) Malloc (12);
						strcpy(tmp,"Newton 2005");
						vqt_ex( handle, tmp, extent );
						for ( i=18; extent[2]-extent[0] < tree[RUTAN].ob_width-30; i+=1)
						{
							vst_arbpt( handle, i, &teckenbredd, &teckenhojd, &dummy, &dummy );
							vqt_ex( handle, tmp, extent ); /*Symbol for vqt_f_extent or vqt_extent*/
						}

						vs_clip( handle, 1, clip );
						mouse_off( );

						print( handle, XMIN+10, YMIN+HEIGHT/2+extent[3]+teckenhojd/2.5, tmp );
						Mfree(tmp);
						if(appl_find( "MLTISTRP" )<0)	/* This takes some time, shown too short time otherwise */
							evnt_timer(1000,0);
						else
							evnt_timer(500,0);

						mouse_on( );
						vs_clip( handle, 0, clip );
						vst_font( handle, font );
						vst_point( handle, fontsize, &dummy, &dummy, &teckenbredd, &teckenhojd );
						vst_color( handle, text_colour );
						bg_colour=tmpbgcolor;
						obspec=whichcolour(obspec);

						first=0;
					}

					objc_draw( tree, RUTAN, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3] );

					clip[2] = box.g_x + box.g_w -22;
					clip[3] = box.g_y + box.g_h - tree[MAIN].ob_height +16 +tree[RUTAN].ob_height;
					vs_clip( handle, 1, clip );
					mouse_off( );
					for ( i=0 ; i<synliga_rader ; i++ )
						print( handle, tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +2, tree[MAIN].ob_y + (i*teckenhojd)+tree[RUTAN].ob_y+teckenhojd, s[i] );
					mouse_on( );
					vs_clip( handle, 0, clip );
					cursor=0;
				}

				if(all==3 && !graph_mode) {
					if(strlen(s[n])==0)	/* If empty row */
						vqt_ex( handle, s[n], extent ); /* Symbol for vqt_f_extent or vqt_extent*/
					else {
						tmp = (BYTE *) Malloc (TECKEN);
						if (!tmp)		 /* If not enough RAM */
							 return;
						memset(tmp, 0, TECKEN);
						strcpy(tmp,s[n]);
						*(tmp+posi)=0;
						vqt_ex ( handle, tmp, extent );
						Mfree(tmp);
					}

					memset(pxyarray, 0, 4);

					pxyarray[0] = tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +1 +extent[2];
					pxyarray[1] = tree[MAIN].ob_y+extent[1] +tree[RUTAN].ob_y+1/*20*/ +teckenhojd*n +(teckenhojd/5);
					pxyarray[2] = pxyarray[0]+teckenbredd/14;
					pxyarray[3] = pxyarray[1]+teckenhojd-(teckenhojd/4);

					clip[2] = box.g_x + box.g_w-22;
					clip[3] = box.g_y + box.g_h - tree[MAIN].ob_height +16 +tree[RUTAN].ob_height;
					vs_clip( handle, 1, clip );

					if(!cursor)
					{	vsf_color( handle, cursor_colour );
						cursor=1; }
					else
					{	vsf_color( handle, bg_colour );
						cursor=0; }

					mouse_off( );
					v_bar( handle, pxyarray );
					if(cursor==0)	/* If the cursor was just turned off, draw the text again */
						redraw_window(0);
					mouse_on( );
					vs_clip( handle, 0, clip );
				}

				if(all==5) { /* Function plotting */
					if(extraRedrawWorkaround){
						extraRedrawWorkaround=0;
						wind_update(END_UPDATE);
						return;
					}
					if(draw_grid) { /* Draw grid */
						vsl_udsty(handle, linestyle); /* weak dotted linestyle */
						vsl_type(handle, 7);
						if(monochrome)
							vsl_color( handle, BLACK );
						else
							vsl_color( handle, LBLACK );
						vsl_ends(handle,0,0);
	
						clip[0]+=24;
						clip[2] = box.g_x + box.g_w -20;
						clip[3] = box.g_y + box.g_h - tree[MAIN].ob_height +16 +tree[RUTAN].ob_height;

						grafarray[0]=XMIN;	/* Draws the first grid */
						grafarray[1]=YMIN;
						grafarray[2]=XMIN;
						grafarray[3]=YMAX;
						vs_clip( handle, 1, clip );
						mouse_off( );
						v_pline( handle, 2, grafarray );						
						mouse_on( );
						vs_clip( handle, 0, clip );
						memset(grafarray, 0, sizeof(grafarray));

						for ( k=0, x=xmin, x_bak=xmin, j=abs(WIDTH); k<j; k++, x+= ((xmax-xmin)/(double)abs(WIDTH)) ) /* Draw vertical grid */
						{
						/*	if(x_bak<=0 && x>=0){ /* Find the zero, problem syncing with grid as zero not always visible */
								nollan=k;
								break;
							} */

							if( fabs(x)-fabs(x_bak)>=grid_distx || fabs(x_bak)-fabs(x)>=grid_distx || (x_bak2<=0 && x>=0) ){
								grafarray[0]=XMIN+k;
								grafarray[1]=YMIN;
								grafarray[2]=XMIN+k;
								grafarray[3]=YMAX;
								vs_clip( handle, 1, clip );
								mouse_off( );
								v_pline( handle, 2, grafarray );						
								mouse_on( );
								vs_clip( handle, 0, clip );
								x_bak=x;
							}
							x_bak2=x; /* To see when it shifts between minus and plus */
						}

						memset(grafarray, 0, sizeof(grafarray));
						grafarray[0]=XMIN;	/* Draws first grid */
						grafarray[1]=YMIN;
						grafarray[2]=XMAX;
						grafarray[3]=YMIN;
						vs_clip( handle, 1, clip );
						mouse_off( );
						v_pline( handle, 2, grafarray );						
						mouse_on( );
						vs_clip( handle, 0, clip );
						memset(grafarray, 0, sizeof(grafarray));

						for ( k=0, x=ymin, x_bak=ymin, j=abs(HEIGHT); k<j; k++, x+= ((ymax-ymin)/(double)abs(HEIGHT)) ) /* Draw horizontal grid */
						{
							if( fabs(x)-fabs(x_bak)>=grid_disty || fabs(x_bak)-fabs(x)>=grid_disty || (x_bak2<=0 && x>=0) ){
								grafarray[0]=XMIN;
								grafarray[1]=YMIN-k+1;
								grafarray[2]=XMAX;
								grafarray[3]=YMIN-k+1;
								vs_clip( handle, 1, clip );
								mouse_off( );
								v_pline( handle, 2, grafarray );						
								mouse_on( );
								vs_clip( handle, 0, clip );
								x_bak=x;
							}
							x_bak2=x; /* To see when it shifts between minus and plus */
						}

						vsl_color( handle, BLACK ); /* reset color */
						vsl_ends(handle, 0, 0);
					}
					
					/* Plot the graph */
												  
					tmp2 = (BYTE *) Malloc (TECKEN); /* Strange, if Mxalloc is used, it crashes completely when drawing the graph twice, but if only Malloc is used, Newton says "something is wrong, I suggest a reboot." This is said when it's not possible to send tooltips. */
					tmpekv = (BYTE *) Malloc (TECKEN); /* Additionally, it keeps saying "there's something wrong in this position all the time," while the tooltips keep moving upward. */
					chdummy = (BYTE *) Malloc (TECKEN);
					if (!chdummy) /* If there is not enough memory */
						return;



					memset(chdummy, 0, TECKEN);
					memset(grafarray, 0, sizeof(grafarray));
					memset(coordarray, 0, sizeof(coordarray));
					mouse_off( );
					graf_mouse( BUSYBEE, NULL );
					mouse_on( );

					for ( j=0, k=0, x=xmin; k<WIDTH+1; j+=2, k++, x+= ((xmax-xmin)/(double)WIDTH) )/* Fill all pixels horizontally, in the right scale */
					{
						memset(tmpekv, 0, TECKEN);
						strcpy(tmpekv,ekv);
						strcpy(tmpekv,preprocess(tmpekv));	/* So that it also handles e.g. 2x */
						
						for ( currpos=0 ; currpos<strlen(tmpekv) ; currpos++ ) /* Replace all 'x' with the current x-value */
						{
							if( *(tmpekv+currpos)=='x' )
							{
								memset(tmp2, 0, TECKEN);
								strcpy(tmp2,(tmpekv+currpos+1));
								memset(tmpekv+currpos,0,TECKEN-currpos-1);
								strcat(tmpekv,gcvt(x,15,chdummy));
								strcat(tmpekv,tmp2);
								if(x_bak<=0 && x>=0)	/* Where the y-axis should be drawn */
									yaxis=XMIN+k;
								x_bak=x;
							}
						}

						temp = (strtod(calculate(preprocess(tmpekv)),NULL)) * ((double)HEIGHT/(ymax-ymin)); /* Y-value on screen */
						grafarray[j] = XMIN+k;
						coordarray[j]=x;	/* X-value at the coordinate*/
						coordarray[j+1]=temp / ((double)HEIGHT/(ymax-ymin));	/* Y-value */

						if(temp>32000) /* To prevent it from going haywire when converted to int */
							temp=32000;
						if(temp<-32000)
							temp=-32000;
						grafarray[j+1]= YMIN /* Here is the beginning of the window */
							- (int)( ymin * ((double)HEIGHT/(ymax-ymin)) ) /* the lower limit of what should be on the screen */
							+ (int)( temp ); /* This is where the memory loss occurs in calculate(preprocess(tmpekv)). Most of it is in preprocess(), but 16K is also in calculate(). */

						if(grafarray[j+1]<YMAX) /* Check that the number is not too large; it seems to get messed up */
							grafarray[j+1]=YMAX-10;
						if(grafarray[j+1]>YMIN)
							grafarray[j+1]=YMIN+10; /* However, this didn't completely solve it; check if the curve is connected somehow */

					}

					vsf_color( handle, text_colour );
					vsl_type(handle, 0);
					vsl_width(handle, 1);
					vsl_color( handle, text_colour );

					clip[1] = box.g_y + box.g_h - tree[MAIN].ob_height +18;
					clip[2] = box.g_x + box.g_w -22;
					clip[3] = box.g_y + box.g_h - tree[MAIN].ob_height +16 +tree[RUTAN].ob_height -10;
					vs_clip( handle, 1, clip );
					vsl_width(handle,linethgraph);
					mouse_off( );
					v_pline( handle, j/2, grafarray ); /* Draw the recently calculated */
					vs_clip( handle, 0, clip );
					vsl_width(handle,1);
					graf_mouse( ARROW, NULL );
					mouse_on( );
					/* Draw x and y-axes */
					
					for ( k=0, x=ymin, j=abs(HEIGHT); k<j; k++, x+= ((ymax-ymin)/(double)abs(HEIGHT)) ) /* Find x-axis */
					{
						if(x_bak<=0 && x>=0){	/* Where the x-axis should be drawn */
							xaxis=YMIN-k+1; 	/* Here it might be suitable to draw the horizontal grid. The zero etc. */
							break;
						}
						x_bak=x;
					}

					memset(grafarray, 0, sizeof(grafarray));
					grafarray[0]=XMIN;				/* x-axis */
					grafarray[1]=xaxis;
					grafarray[2]=XMAX;
					grafarray[3]=grafarray[1];

					clip[2] = box.g_x + box.g_w -22;
					clip[3] = box.g_y + box.g_h - tree[MAIN].ob_height +16 +tree[RUTAN].ob_height;
					vs_clip( handle, 1, clip );
					vsl_color( handle, cursor_colour );
					vsl_ends(handle, 0, 1); /* Arrow av the end */
					vsl_width(handle,1);
					mouse_off( );
					if(xaxis!=-1) /* if -1, then 0 is not visible */
						v_pline( handle, 2, grafarray );
					mouse_on( );
					vs_clip( handle, 0, clip );

					memset(grafarray, 0, sizeof(grafarray));
					grafarray[0]=yaxis;		/* y-axis */
					grafarray[1]=YMIN;
					grafarray[2]=grafarray[0];
					grafarray[3]=YMAX;

					strcpy(tmpekv, "ï¿½(x)=");
					strcat(tmpekv, ekv);
					vs_clip( handle, 1, clip );
					mouse_off( );
					if(yaxis!=-1)
						v_pline( handle, 2, grafarray );
					mouse_on( );
					vsl_ends(handle, 0, 0); /* No more arrow */

					/* Skriv ut axelnamnen etc */

					if(vst_font( handle, 5031 )==0) /* If Baskerville is not available */
						if(vst_font( handle, 5954 )==0) /* Try with Life */
							if(vst_font( handle, 5003 )==0) /* Try with Swiss 721 */
								if(vst_font( handle, 5142 )==0) /* Try with Futura */
									if(vst_font( handle, 5768 )==0) /* Try with Handel Gothic */
										vst_font( handle, font ); /* Otherwise, use the default font */
					vst_point( handle, 10, &dummy, &dummy, &teckenbredd, &teckenhojd );
					vst_color( handle, text_colour );
					mouse_off( );
					print( handle, XMIN+12, YMAX+24 , tmpekv );
					mouse_on( );
					vst_color( handle, text_colour );

					vqt_ex( handle, gcvt(xmax,15,chdummy), extent );
					mouse_off( );
					print( handle, XMIN, xaxis - 10, gcvt(xmin,15,chdummy) ); /* Print xmin */
					print( handle, XMIN+WIDTH - (extent[2]-extent[0]), xaxis -10, gcvt(xmax,15,chdummy) );	/* Print xmax */
					print( handle, yaxis + 8, YMAX+10 , gcvt(ymax,15,chdummy) ); /* Print ymax */
					print( handle, yaxis + 8, YMIN, gcvt(ymin,15,chdummy) );	/* Print ymin */

					mouse_on( );
					vs_clip( handle, 0, clip );

					vst_font( handle, font);
					vst_point( handle, fontsize, &dummy, &dummy, &teckenbredd, &teckenhojd );
					vst_color( handle, text_colour );

					cursor=0;
					Mfree(tmp2);Mfree(tmpekv);Mfree(chdummy);
				}
			}
			else {
				wind_get ( whandle, WF_WORKXYWH, &dummy, &dummy, &dummy, &icon_size );
				if( icon_size < 72 ) {
					icon_small[0].ob_x=work.g_x; icon_small[0].ob_y=work.g_y; icon_small[0].ob_width=work.g_w; icon_small[0].ob_height=work.g_h;
					icon_small[1].ob_x = icon_small[0].ob_width/2 - icon_small[1].ob_width/2;
					icon_small[1].ob_y = icon_small[0].ob_height/2 - icon_small[1].ob_height/2;
					mouse_off( );
					objc_draw( icon_small, 0, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3] );
					mouse_on( );
				}
				else {
					icon[0].ob_x=work.g_x; icon[0].ob_y=work.g_y; icon[0].ob_width=work.g_w; icon[0].ob_height=work.g_h;
					icon[1].ob_x = icon[0].ob_width/2 - icon[1].ob_width/2;
					icon[1].ob_y = icon[0].ob_height/2 - icon[1].ob_height/2;
					mouse_off( );
					objc_draw( icon, 0, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3] );
					mouse_on( );
				}
			}
			wind_get( whandle, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h );
		}
	}
	wind_update(END_UPDATE);
}

void redrawWindow( int winhandle, GRECT *dirty ) /* Test */
{
	GRECT rect;
	TEDINFO *obspec;
	int i;

	if( winhandle <= 0 )						/* If no windows is open */
		return;

	vswr_mode( winhandle, MD_TRANS );					 /* set transp mode */
	vst_font( winhandle, font );
	vst_point( winhandle, fontsize, &dummy, &dummy, &teckenbredd, &teckenhojd );

	vst_color( winhandle, text_colour );
	synliga_rader = (tree[RUTAN].ob_height-6)/teckenhojd;
	obspec=tree[RUTAN].ob_spec.tedinfo;
	obspec=whichcolour(obspec);

	if(n>synliga_rader-1) /* Safest, n begins at 0 and not 1. */
		n=synliga_rader-1;
	if(n<0)
		n=0;

	 wind_update( BEG_UPDATE );

	 wind_get( winhandle, WF_FIRSTXYWH, &rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
	 while( rect.g_w && rect.g_h ) {
		if( rc_intersect( dirty, &rect ) ) {
			mouse_off( );
			clip[0]=rect.g_x;
			clip[1]=rect.g_y;
			clip[2]=rect.g_w;
			clip[3]=rect.g_h;
			objc_draw( tree, MAIN, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3] );
			vs_clip( winhandle, 1, clip );
			for ( i=0 ; i<synliga_rader ; i++ )
				print( winhandle, tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +2, tree[MAIN].ob_y + (i*teckenhojd)+tree[RUTAN].ob_y+teckenhojd, s[i] );
			vs_clip( winhandle, 0, clip );
			mouse_on( );
			 /*
			  * Do your drawing here...constrained to the rectangle in g.
			  */
		}
		wind_get( winhandle, WF_NEXTXYWH, &rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
	 }
	 wind_update( END_UPDATE );
}

void mouse_on( void )
{
	graf_mouse( M_ON, (void *)0 );
}

void mouse_off( void )
{
	graf_mouse( M_OFF, (void *)0 );
}

void position_objects(void)
{
	tree[1].ob_width=tree[MAIN].ob_width;
	tree[1].ob_height=tree[MAIN].ob_height;
	tree[RUTAN].ob_width = tree[MAIN].ob_width-36;
	tree[RUTAN].ob_height = tree[MAIN].ob_height - tree[KNAPP_BAKGRUND].ob_height - 60;
	tree[KNAPP_BAKGRUND].ob_x = tree[MAIN].ob_width/2 - tree[KNAPP_BAKGRUND].ob_width/2;
	tree[KNAPP_BAKGRUND].ob_y = tree[MAIN].ob_height/2 - (tree[RUTAN].ob_height+16 + tree[KNAPP_BAKGRUND].ob_height)/2 + tree[RUTAN].ob_height+26;
	tree[NEWTON_SIZER].ob_x = tree[MAIN].ob_width-tree[NEWTON_SIZER].ob_width;
	tree[NEWTON_SIZER].ob_y = tree[MAIN].ob_height-tree[NEWTON_SIZER].ob_height;
	tree[NEWTON_LOGO].ob_x = tree[KNAPP_BAKGRUND].ob_x+tree[KNAPP_BAKGRUND].ob_width-tree[NEWTON_LOGO].ob_width-8;
	tree[NEWTON_LOGO].ob_y = tree[RUTAN].ob_height + 20;
	tree[COORDINATEBOX].ob_x = tree[KNAPP_BAKGRUND].ob_x;
	tree[COORDINATEBOX].ob_y = tree[RUTAN].ob_height + 24;
}

TEDINFO* whichcolour( TEDINFO* obspec)
{
	obspec->te_color &= -16; /* Zeroes the first four bits in the byte */
	obspec->te_color=obspec->te_color |= bg_colour;

	return obspec;
}