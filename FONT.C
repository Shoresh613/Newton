

/* choose_font()
 * Anropar en xFSL-fontvÑljare.
 **************************/


#include <string.h>
#include <stdlib.h>
#include <aes.h>
#include <tos.h>
#include <xfsl.h>
#include <ufsl.h>
#include <stdio.h>
#include <portab.h>
/*#include <mt_font.h>*/
#include "newton.h"
#include "xtras.h"

extern OBJECT *settings;
extern int font, fontsize, text_colour, bg_colour, handle, number_of_fonts;
extern FNT_DIALOG* fnt_dialog;
extern char s[RADER][TECKEN];
extern int n;
extern int MagX, MiNT;
extern int Get_cookie( long target, long *p_value );


int cdecl fontsel_init(void);
int cdecl fontsel_input(int vdihandle, int dummy, int ftype, char *text, int *id, int *size);
int cdecl fontsel_exinput(int vdihandle, int ftype, char *text, int *id, int *size);

int fix31_to_pixel( long a )
{
   int  b;

   b = (int) (( a + 32768L ) >> 16 ); /* runden !! */
   return( b );                       /* Pixelwert zurÅckgeben */
}

void cdecl helpfunc()
{
   form_alert(1,"[0][Choose font for Newton.][   OK   ]");
}

int choose_font(void)
{
	int xhandle=0, xret=0;
	xFSL *xfsl=NULL;
	UFSL *ufsl=NULL;
	char *env=NULL;
	xFSL_PAR xpar;
	PFONTINFO pfont;
	int done=0, *fonttmp=NULL, *sizetmp=NULL;
	
	long val=0;	
	long *size=NULL, *mtfontid=NULL, *ratio=NULL;
	int *checkboxes=NULL, exitbutn=0;
	
	char *No_xFSL=NULL;
	char *Choose_Font=NULL;
	char *ExampleString=NULL;

/*	static int chars[] = { ' ', '~',  /* ASCII 32..126 */
						'˚','˚', '˝','˝', '„','„', 'Í','Í', 'Û','Û',
						'˙','˙', '˛','˛', '´','´', '¨','¨', 'Ê','Ê',
						1,0       /* Ende der Liste */
	};*//* Dessa tecken ska fonten innehÜlla. Tycker jag gîr rÑtt enl. spec. */

	if (MiNT || MagX) {
		ExampleString = (char*) Mxalloc( sizeof(s[n]), 0|MGLOBAL );
		ufsl = (UFSL*) Mxalloc( sizeof(ufsl), 0|MGLOBAL );
		xpar.par_size = (int)(xFSL_PAR*) Mxalloc( sizeof(xpar), 0|MGLOBAL );
		pfont.fontid = (int)(PFONTINFO*) Mxalloc( sizeof(pfont), 0|MGLOBAL );
	}
	else {
		ExampleString = (char*) Malloc( sizeof(s[n]) );
		ufsl = (UFSL*) Malloc( sizeof(ufsl) );
		xpar.par_size = (int)(xFSL_PAR*) Malloc( sizeof(xpar) );
		pfont.fontid = (int)(PFONTINFO*) Malloc( sizeof(pfont) );
	}

	memset(&xpar,0,sizeof(xFSL_PAR));
	memset(&pfont,0,sizeof(PFONTINFO));

	rsrc_gaddr( R_STRING, CHOOSE_FONT, &Choose_Font );
	rsrc_gaddr( R_STRING, NO_XFSL, &No_xFSL );

	if(text_colour<0) /* Den muckade en gÜng, better safe than sorry */
		text_colour=1;
	if(bg_colour<0)
		bg_colour=1;

	xpar.font=&pfont;
	xpar.font->fontcol=text_colour;
	xpar.font->backcol=bg_colour;
	xpar.font->fontid=font;
	xpar.font->fontsize.size=fontsize;
	xpar.font->validtype=V_CHAR_IND; /* MAP_ASCII stod det innan V_CHAR_IND*/
	xpar.font->validchars=NULL;/*chars;*/
	xpar.font->fonttype=0|BITMAP_FONT|SPEEDO_FONT|TT_FONT|PFB_FONT; /* ér inte sÑker om denna skall vara kvar.. */
	xpar.fontflags=FF_ALL;
	xpar.poptext=NULL;
	xpar.num_entries=0;
	xpar.sel_entry=0;
	xpar.popup=NULL;
	if(strlen(s[n])==0)
		xpar.example="M/(˚(1-(V/C)˝))";
	else {
		strcpy(ExampleString,s[n]);
		xpar.example=ExampleString;
	}
	xpar.helpinfo=NULL;
	xpar.helptext=NULL;
	xpar.headline=Choose_Font;
	xpar.control=0|CC_NOWIDTH|CC_NOKERN|CC_NOSKEW|CC_NOALIGN|CC_NOROTATION;

	xpar.par_size=(WORD)((LONG) sizeof(xFSL_PAR));  /* Testar lite casting, dÜ sizeof ger en */
	xpar.pfi_size=(WORD)((LONG) sizeof(PFONTINFO)); /* long och .size skall vara int.        */

	if(Get_cookie('xFSL',(long*)&xfsl)){
		xhandle=xfsl->xfsl_init(0,&xpar);
		if(xhandle>=0)
		{
			do
				xret=xfsl->xfsl_event(xhandle,0L);
			while(xret>xFS_OK);
			xfsl->xfsl_exit(xhandle);
			if(xret==xFS_STOP){
				Mfree(ExampleString);Mfree(ufsl);Mfree((void*)xpar.par_size);Mfree((void*)pfont.fontid);
				return font; /* Om man valt AVBRYT */
			}
			else if(xret==xFS_OK)
			{
				text_colour=xpar.font->fontcol;
				bg_colour=xpar.font->backcol;
				font=xpar.font->fontid;
				fontsize=xpar.font->fontsize.size;
				Mfree(ExampleString);Mfree(ufsl);Mfree((void*)xpar.par_size);Mfree((void*)pfont.fontid);
				return font; /* HÑr meddelas vilken font som valts */
			}
			else if(xret<0) /*{printf("Error %d\n",xret);return 0;} */return font;
		}
		else /*{printf("Error %d\n",xhandle);return 0;}*/return font;
	} 

	else if(Get_cookie('UFSL',(long*)&ufsl))
	{
		Mfree((void*)xpar.par_size);Mfree((void*)pfont.fontid);
		if (MiNT || MagX)
		{
			fonttmp = (int*) Mxalloc( sizeof(int), 0|MGLOBAL );
			sizetmp = (int*) Mxalloc( sizeof(int), 0|MGLOBAL );
		}
		else
		{
			fonttmp = (int*) Malloc( sizeof(int) );
			sizetmp = (int*) Malloc( sizeof(int) );
		}

		*fonttmp=font;
		*sizetmp=fontsize;
		
		ufsl->helpfunc= NULL;   /* Hilfefunktion oder NULL */
		ufsl->msgfunc = NULL; /* Redrawfunktion oder NULL,
		                                  Dialtyp beachten */

		done=ufsl->font_selinput(handle,0,0,ExampleString,fonttmp,sizetmp);	/* Stîdjs av alla fontvÑljare */
	/*	done=ufsl->fontsel_exinput(handle,0,ExampleString,fonttmp,sizetmp); */ /* Stîdjs kanske inte av alla fontvÑljare */
		if(done==1)
		{
			font= (*fonttmp);
			fontsize= (*sizetmp);
			Mfree(fonttmp);Mfree(sizetmp);Mfree(ufsl);Mfree(ExampleString);
         return font;
		}
		Mfree(fonttmp);Mfree(sizetmp);Mfree(ufsl);Mfree(ExampleString);
		if(done==-1)
		{
         form_alert(1,"[3][Not enough memory to call|font selector! ][  OK  ]");
         return font;
		}
		if(done==-3)
		{
         form_alert(1,"[3][Could not identify font size! ][  OK  ]");
         return font;
		}
		if(done==-4)
		{
         form_alert(1,"[3][The number of fonts has to|be more than one! ][  OK  ]");
         return font;
	   }
	}

	else if(Get_cookie(MagX_COOKIE,&val))
	{
		Mfree((void*)xpar.par_size);Mfree((void*)pfont.fontid);

		size = (long*) Mxalloc( sizeof(long), 0|MGLOBAL );
		mtfontid = (long*) Mxalloc( sizeof(long), 0|MGLOBAL );
		ratio = (long*) Mxalloc( sizeof(long), 0|MGLOBAL );
		checkboxes = (int*) Mxalloc( sizeof(int), 0|MGLOBAL );

		exitbutn=fnts_do( fnt_dialog,0|FNTS_SNAME|FNTS_SSIZE,font,fontsize*65536L,65536L,checkboxes,mtfontid,size,ratio );

		if(exitbutn==FNTS_OK)
		{
			font=(int) *mtfontid;
			fontsize=fix31_to_pixel( *size );
		}

		Mfree(size);Mfree(mtfontid);Mfree(ratio);Mfree(checkboxes);
		Mfree(ExampleString);Mfree(ufsl);
		return font;
	} /*Det Ñr fnts_do som muckar.. */ 

	else
	{
		Mfree((void*)xpar.par_size);Mfree((void*)pfont.fontid);
		env = (char *) Malloc (512);
		if (!env)       /* Om det inte finns tillrÑckligt med minne*/
		    return font;
	
		strcpy(env,getenv("FONTSELECT"));
		if( env==NULL ) /* om $FONTSELECT inte Ñr satt */
			form_alert(1,No_xFSL);
		else
		{
			if(Get_cookie (MagX_COOKIE, &val) == TRUE)
				shel_write(1,1,100, env, NULL); /* Egentligen skall man kunna ange flera fontvÑljare separerade med ; dessa skall dÜ startas hela hîgen*/
			else
				shel_write(0,1,1, env, NULL);
		}
	
		Mfree(env);
		Mfree(ExampleString);Mfree(ufsl);
	}

	return font; /* Om ingen kunde ropas upp, returnera samma font som det var innan */
}