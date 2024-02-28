

/* save_settings(fontid), load_settings(), change_settings()
 * Loads and saves font, settings and position on the screen.
 *******************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <aes.h>
#include <vdi.h>
#include <portab.h>
#include <tos.h>
#include "newton.h"
#include "xtras.h"

extern OBJECT *tree;
extern OBJECT *settings;
extern int font, fontsize, cursor_colour, text_colour, bg_colour, synliga_rader, linewidth, fontpi;
extern int cursor_blink, moral_check, rad, save_work, show_intro, sendkey, n, hex, bin, dec, play_sample, boxes;
extern int tmp_x,tmp_y,tmp_w,tmp_h, iconified, handle, monochrome;
extern int MagX, MiNT;
extern int posi;
extern char *tmp_startup, *tmp_trashcan, *tmp_language, *tmp_quit; /* Paths to the audio files */
extern char undo[RADER][TECKEN];
extern UWORD nvdi;

extern char s[RADER][TECKEN];

extern int do_dialog( OBJECT *dialog);

TEDINFO *obspec1;	
char *strptr1;

void save_settings( int fontid )
{
	FILE *fp=NULL;
	char *home=NULL;
	char *tmp=NULL;
	char font_id[]="        ", Cursor_Colour[]="     ", Text_Colour[]="     ", Bg_Colour[]="     ", FontSize[]="     ", Posi[]="     ";
	char x_pos[]="    ", y_pos[]="    ", bredd[]="    ", hojd[]="    ", Show_Intro[]="  ", cursor[]="  ", moral[]="  ", Rad[]="  ", SaveWork[]="  ", N[]="  ", SendKey[]="  ", Hex[]="  ", Bin[]="  ", Dec[]="  ", PlaySample[]="  ", LineWidth[]="  ", FontPi[]="  ", Boxes[]="  ";
	int i=0, dummy=0;
	char *Save_Info=NULL;

	rsrc_gaddr( R_STRING, SAVE_INFO, &Save_Info );

	tmp = (BYTE *) Malloc (256);

	if (!tmp)       /* If there is not enough memory */
	    return;

	home=getenv("HOME");
	if( home==NULL ) { /* if $HOME is not set */
		if(!monochrome) {
			if((fp=fopen("newton.inf","w+"))== NULL)
			{	form_alert( 1,Save_Info);
				Mfree(tmp);
				return;
			}
		}
		else {
			if((fp=fopen("newtonbw.inf","w+"))== NULL) {
				form_alert( 1,Save_Info);
				Mfree(tmp);
				return;
			}
		}
	}
	else {
		strcpy( tmp, home);
		strcat(tmp,"\\");

		if(!monochrome)
			strcat( tmp, "defaults\\newton.inf" );
		else
				strcat( tmp, "defaults\\newtonbw.inf" ); /* If the defaults folder exists in $HOME */

		if((fp=fopen(tmp,"w+"))== NULL) {
			strcpy(tmp,home);
			strcat(tmp,"\\");
			if(!monochrome)
				strcat( tmp, "newton.inf" ); /* If $HOME exists */
			else
				strcat( tmp, "newtonbw.inf" ); /* If $HOME exists */
			if((fp=fopen(tmp,"w+"))== NULL) {
				form_alert( 1,Save_Info);
				Mfree(tmp);
				return;
			}
		}
	}	

	fputs( "0.9935 ", fp ); /* 0.975 because otherwise there will be problems, loads as approximately 0.97000003  */
	itoa( fontid, font_id, 10);
	fputs( font_id, fp );
	fputs( " ", fp );
	itoa( fontsize, FontSize, 10);
	fputs( FontSize, fp );
	fputs( " ", fp );
	itoa( cursor_colour, Cursor_Colour, 10);
	fputs( Cursor_Colour, fp );
	fputs( " ", fp );
	itoa( text_colour, Text_Colour, 10);
	fputs( Text_Colour, fp );
	fputs( " ", fp );
	itoa( bg_colour, Bg_Colour, 10);
	fputs( Bg_Colour, fp );
	fputs( " ", fp );
	if(iconified) {
		itoa( tmp_x, x_pos, 10 );
		fputs( x_pos, fp );
		fputs( " ", fp );
		itoa( tmp_y, y_pos, 10 );
		fputs( y_pos, fp );
		fputs( " ", fp );
		itoa( tmp_w, bredd, 10 );
		fputs( bredd, fp );
		fputs( " ", fp );
		itoa( tmp_h, hojd, 10 );
		fputs( hojd, fp );
		fputs( " ", fp );
	}
	else {
		itoa( tree[MAIN].ob_x, x_pos, 10 );
		fputs( x_pos, fp );
		fputs( " ", fp );
		itoa( tree[MAIN].ob_y, y_pos, 10 );
		fputs( y_pos, fp );
		fputs( " ", fp );
		itoa( tree[MAIN].ob_width, bredd, 10 );
		fputs( bredd, fp );
		fputs( " ", fp );
		itoa( tree[MAIN].ob_height, hojd, 10 );
		fputs( hojd, fp );
		fputs( " ", fp );
	}
	itoa( rad, Rad, 10 );
	fputs( Rad, fp );
	fputs( " ", fp );
	itoa( cursor_blink, cursor, 10 );
	fputs( cursor, fp );
	fputs( " ", fp );
	itoa( moral_check, moral, 10 );
	fputs( moral, fp );
	fputs( " ", fp );
	itoa( sendkey, SendKey, 10 );
	fputs( SendKey, fp );
	fputs( " ", fp );
	itoa( show_intro, Show_Intro, 10 );
	fputs( Show_Intro, fp );
	fputs( " ", fp );
	itoa( play_sample, PlaySample, 10 );
	fputs( PlaySample, fp );
	fputs( " ", fp );
	itoa( hex, Hex, 10 );
	fputs( Hex, fp );
	fputs( " ", fp );
	itoa( bin, Bin, 10 );
	fputs( Bin, fp );
	fputs( " ", fp );
	itoa( dec, Dec, 10 );
	fputs( Dec, fp );
	fputs( " ", fp );
	itoa( linewidth, LineWidth, 10 );
	fputs( LineWidth, fp );
	fputs( " ", fp );
	itoa( fontpi, FontPi, 10 );
	fputs( FontPi, fp );
	fputs( " ", fp );
	itoa( boxes, Boxes, 10 );
	fputs( Boxes, fp );
	fputs( " ", fp );
	itoa( posi, Posi, 10 );
	fputs( Posi, fp );
	fputs( " ", fp );

	itoa( dummy, Posi, 10 );	/* Dummies in the mean time, to save special fonts etc for graph_mode */
	fputs( Posi, fp );
	fputs( " ", fp );
	itoa( dummy, Posi, 10 );
	fputs( Posi, fp );
	fputs( " ", fp );
	itoa( dummy, Posi, 10 );
	fputs( Posi, fp );
	fputs( " ", fp );
	itoa( dummy, Posi, 10 );
	fputs( Posi, fp );
	fputs( " ", fp );
	itoa( dummy, Posi, 10 );
	fputs( Posi, fp );
	fputs( " ", fp );

	itoa( dummy, Posi, 10 );	/* 4 extra entries for future use, so you don't have to redo the info file all the time */
	fputs( Posi, fp );
	fputs( " ", fp );
	itoa( dummy, Posi, 10 );
	fputs( Posi, fp );
	fputs( " ", fp );
	itoa( dummy, Posi, 10 );
	fputs( Posi, fp );
	fputs( " ", fp );
	itoa( dummy, Posi, 10 );
	fputs( Posi, fp );
	fputs( " ", fp );


	if(tmp_startup[0]==0) /* The audio files' paths */
		fputs("#", fp);
	else {
		fputs("\"" , fp);
		fputs(tmp_startup, fp);
		fputs("\"" , fp);
	}
	fputs( " ", fp );
	if(tmp_trashcan[0]==0)
		fputs("#", fp);
	else {
		fputs("\"" , fp);
		fputs(tmp_trashcan, fp);
		fputs("\"" , fp);
	}
	fputs( " ", fp );
	if(tmp_language[0]==0)
		fputs("#", fp);
	else {
		fputs("\"" , fp);
		fputs(tmp_language, fp);
		fputs("\"" , fp);
	}
	fputs( " ", fp );
	if(tmp_quit[0]==0)
		fputs("#", fp);
	else {
		fputs("\"" , fp);
		fputs(tmp_quit, fp);
		fputs("\"" , fp);
	}
	fputs( " ", fp );

	itoa( save_work, SaveWork, 10 );
	fputs( SaveWork, fp );
	fputs( " ", fp );

	if(save_work) {
		itoa( n, N, 10 ); /* Position */
		fputs( N, fp );
		fputs( " ", fp );
		
		while(i<synliga_rader) { /* Expressions */
			if(s[i][0]==0)
				fputs("#", fp);
			else {
				fputs( "\"", fp );
				fputs( s[i], fp );
				fputs( "\"", fp );
			}
			fputs( " ", fp );
			i++;
		}
	}

	fclose(fp);
	Mfree(tmp);
}

void load_settings( void )
{
	FILE *fp=NULL;
	char *home=NULL;
	char *tmp=NULL;
	int x=0,y=0,wi=0,he=0; /* Check if the width has been changed */
	int gw=0,gh=0,dummy=0;
	int i=0,eof=0;
	float ver=0;
	char *Old_Info_File=NULL;

	tmp = (BYTE *) Malloc (TECKEN);
	if (!tmp)       /* If there is not enough memory */
		return;
	memset(tmp, 0, TECKEN);

	rsrc_gaddr( R_STRING, OLD_INFOFILE, &Old_Info_File );

	home=getenv("HOME");
	if( home==NULL ) {
		if(!monochrome) {
			if((fp=fopen("newton.inf","r"))== NULL) {
				Mfree(tmp);
				return;
			}
		}
		else {
			if((fp=fopen("newtonbw.inf","r"))== NULL) {
				Mfree(tmp);
				return;
			}		
		}
	}
	else {
		strcpy(tmp,home);
		strcat(tmp,"\\"); /* Always put extra slash there */
		if(!monochrome)
			strcat( tmp, "defaults\\newton.inf" );
		else
			strcat( tmp, "defaults\\newtonbw.inf" );		
		if((fp=fopen(tmp,"r"))== NULL) {
			strcpy(tmp,home); 
			strcat(tmp,"\\");
			if(!monochrome)
				strcat( tmp, "newton.inf" );
			else
				strcat( tmp, "newtonbw.inf" );
			if((fp=fopen(tmp,"r"))== NULL) {
				Mfree(tmp);
				return;
			}
		}	
	}

	fscanf( fp, "%g", &ver ); /* Checks if the info file is of a compatible version */
	if( ver<0.993 ) { /* This works if you only change the version number that is written in the info file when the format is changed.*/
		form_alert( 1,Old_Info_File);
		fclose(fp);	Mfree(home); Mfree(tmp); return; 
	}
	
	fscanf( fp, "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i",
			&font, &fontsize, &cursor_colour, &text_colour, &bg_colour, &x, &y, &wi, &he, &rad, &cursor_blink, &moral_check, &sendkey, &show_intro, &play_sample, &hex,
			&bin, &dec, &linewidth, &fontpi, &boxes, &posi, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);

	fscanf( fp, "%c", tmp_startup);			/* The space before the audio file paths */
	fscanf( fp, "%c", tmp_startup);
	if(strcmp("#",tmp_startup)==0)
		tmp_startup[0]=0;
	else {
		fscanf( fp, "%[^\"]", tmp_startup);	/* Read until character " */
		fscanf( fp, "%c", tmp);					/* Dummy */
	}
	fscanf( fp, "%c", tmp_trashcan);			/* The space */
	fscanf( fp, "%c", tmp_trashcan);
	if(strcmp("#",tmp_trashcan)==0)
		tmp_trashcan[0]=0;
	else {
		fscanf( fp, "%[^\"]", tmp_trashcan);
		fscanf( fp, "%c", tmp);
	}
	fscanf( fp, "%c", tmp_language);			/* The space */
	fscanf( fp, "%c", tmp_language);
	if(strcmp("#",tmp_language)==0)
		tmp_language[0]=0;
	else {
		fscanf( fp, "%[^\"]", tmp_language);
		fscanf( fp, "%c", tmp);
	}
	fscanf( fp, "%c", tmp_quit);			/* The space */
	fscanf( fp, "%c", tmp_quit);
	if(strcmp("#",tmp_quit)==0)
		tmp_quit[0]=0;
	else {
		fscanf( fp, "%[^\"]", tmp_quit);
		fscanf( fp, "%c", tmp);
	}

	fscanf( fp, "%i", &save_work);
	wind_get( 0, WF_WORKXYWH, &dummy, &dummy, &gw, &gh ); /* How big is the screen? */

	if( x >= 0 ) /* So that it at least doesn't end up outside of the screen */
		tree[MAIN].ob_x=x;
	if( y >= 0 )
		tree[MAIN].ob_y=y;
	if( wi > gw )           /* If larger than screen */
		wi = gw;
	if( he > gh-26 )
		he = gh-26;
	if( wi >= tree[MAIN].ob_width ) /* If greater than minimum size */
		tree[MAIN].ob_width=wi;
	if( he >= tree[MAIN].ob_height )
		tree[MAIN].ob_height=he;
	if(posi<0)	/* The cursor position must not be less than 0  */
		posi=0;

	if(save_work) {
		fscanf(fp, "%i", &n );
		fscanf(fp, "%c", tmp);		/* The space */
		while( i<RADER && eof!=EOF ) {
			memset(tmp,0,TECKEN);
			fscanf(fp, "%c", tmp);
			if(strcmp(tmp,"#")==0){
				s[i][0]=0;
				eof=fscanf(fp, "%c", tmp);	/* Space */
			}
			else {
				fscanf(fp, "%[^\"]", (char*)s[i] ); /* Read to end of expression */
				fscanf(fp, "%c", tmp);	/* Ending '-sign */
				eof=fscanf(fp, "%c", tmp);	/* The space */
			}
			i++;
		}
	}

	if(posi>strlen(s[n]))	/* The cursor position cannot be more than the line length */
		posi=(int)strlen(s[n]);

	for ( i=0 ; i<synliga_rader ; i++)	/* Fix the undo buffer */
		strcpy(undo[i],s[i]);

	fclose(fp);
	Mfree(tmp);
}

void change_settings(void)
{
	int exitbutn;
	
	rsrc_gaddr( R_TREE, SETTINGS, &settings );

	obspec1 = settings[LINEWIDTH].ob_spec.tedinfo;
	strptr1 = obspec1->te_ptext;
	strptr1[0] = 0;

	if(rad==0) { /* Sets the attributes of the buttons */
		settings[RADIANS].ob_state &= ~SELECTED;
		settings[DEGREES].ob_state |= SELECTED;
	}
	else {	
		settings[RADIANS].ob_state |= SELECTED;
		settings[DEGREES].ob_state &= ~SELECTED;
	}
	if(hex==1) {
		settings[HEXADECIMAL].ob_state |= SELECTED;
		settings[DECIMAL].ob_state &= ~SELECTED;
		settings[BINARY].ob_state &= ~SELECTED;
	}
	else if(bin==1) {	
		settings[BINARY].ob_state |= SELECTED;
		settings[HEXADECIMAL].ob_state &= ~SELECTED;
		settings[DECIMAL].ob_state &= ~SELECTED;
	}
	else if(dec==1) {	
		settings[DECIMAL].ob_state |= SELECTED;
		settings[HEXADECIMAL].ob_state &= ~SELECTED;
		settings[BINARY].ob_state &= ~SELECTED;
	}
	if(cursor_blink==1)
		settings[CURSOR].ob_state |= SELECTED;
	else
		settings[CURSOR].ob_state &= ~SELECTED;
	if(moral_check==1)
		settings[MORAL].ob_state |= SELECTED;
	else
		settings[MORAL].ob_state &= ~SELECTED;
	if(sendkey==1)
		settings[SENDKEY].ob_state |= SELECTED;
	else
		settings[SENDKEY].ob_state &= ~SELECTED;
	if(save_work==1)
		settings[SAVE_WORK].ob_state |= SELECTED;
	else
		settings[SAVE_WORK].ob_state &= ~SELECTED;
	if(show_intro==1)
		settings[SHOW_INTRO].ob_state |= SELECTED;
	else
		settings[SHOW_INTRO].ob_state &= ~SELECTED;
	if(play_sample==1)
		settings[PLAYSAMPLE].ob_state |= SELECTED;
	else
		settings[PLAYSAMPLE].ob_state &= ~SELECTED;
	if(fontpi==1)
		settings[FONTPI].ob_state |= SELECTED;
	else
		settings[FONTPI].ob_state &= ~SELECTED;
	if(boxes==1)
		settings[BOXES].ob_state |= SELECTED;
	else
		settings[BOXES].ob_state &= ~SELECTED;

	itoa(linewidth,strptr1,10);

	if(!nvdi) {
		settings[SHOW_INTRO].ob_state &= ~SELECTED;
		settings[SHOW_INTRO].ob_state |= DISABLED;
	}

	exitbutn=do_dialog(settings);

	if(exitbutn==SET) {
		if(settings[RADIANS].ob_state & SELECTED)
			rad=1;
		else
			rad=0;
		if(settings[HEXADECIMAL].ob_state & SELECTED)
			hex=1;
		else
			hex=0;
		if(settings[BINARY].ob_state & SELECTED)
			bin=1;
		else
			bin=0;
		if(settings[DECIMAL].ob_state & SELECTED)
			dec=1;
		else
			dec=0;
		if(settings[CURSOR].ob_state & SELECTED)
			cursor_blink=1;
		else
			cursor_blink=0;
		if(settings[MORAL].ob_state & SELECTED)
			moral_check=1;
		else
			moral_check=0;
		if(settings[SENDKEY].ob_state & SELECTED)
			sendkey=1;
		else
			sendkey=0;
		if(settings[SAVE_WORK].ob_state & SELECTED)
			save_work=1;
		else
			save_work=0;
		if(settings[SHOW_INTRO].ob_state & SELECTED)
			show_intro=1;
		else
			show_intro=0;
		if(settings[PLAYSAMPLE].ob_state & SELECTED)
			play_sample=1;
		else
			play_sample=0;
		if(settings[FONTPI].ob_state & SELECTED)
			fontpi=1;
		else
			fontpi=0;
		if(settings[BOXES].ob_state & SELECTED)
			boxes=1;
		else
			boxes=0;
	
		linewidth=atoi(strptr1);
	}
}