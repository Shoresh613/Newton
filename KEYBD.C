

/* handle_keybd(key,kstate,mx,my)
 * Tolkar tangentbordstryckningar. Det den inte
 * f”rst†r skickas vidare till AVSERVERn.
 *******************************/


#include <string.h>
#include <aes.h>
#include <vdi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <nkcc.h>
#include <vaproto.h>
#include <portab.h>
#include "newton.h"
#include "xtras.h"

#define EPS 0.0000000000001 /* S„ger hur exakt fraktionsutr„knaren ska vara */

typedef struct{
	long numerator;
	long denominator;
} FRACTION;

extern OBJECT *info;
extern OBJECT *settings;
extern OBJECT *tree;

extern int quit, syntax_error, first;
extern int n, undoN;
extern int font, handle, whandle, iconified, hex;
extern int clip[4];
extern char s[RADER][TECKEN];
extern char undo[RADER][TECKEN];
extern char *Version;
extern int synliga_rader, teckenbredd;
extern int posi, undoPosi;
extern int cursor, sendkey, fontpi;
extern int tmp_x,tmp_y,tmp_w,tmp_h;
extern double xmin, xmax, ymin, ymax; /* F”r grafen d†r† */
extern int MagX, MiNT;
extern const char funk_lista[ANTAL_FUNKTIONER][8];
extern char konstant_lista[ANTAL_ENHETER][MAX_TECKEN];
extern int scrollVector[RADER], scrollVectorBak[RADER];
extern int antal_konstanter;
extern int graph_mode;
extern char* xcoordinate, *ycoordinate;
extern int choose_font(void);
extern void scrap_read(void);
extern void scrap_write(int i);
extern void stguide_help(void);
extern void av_sendmesg(int key, int kstate, int message);
extern void redraw_window(int pos);
extern char* pre_calculate( void );
extern void show_info(void);
extern void change_settings(void);
extern int do_dialog(OBJECT *dialog);
extern void colour_select( void );
extern void formulas(int mx, int my);
extern void save_settings(int font);
extern void start_browser(void);
extern void start_mailer(void);
extern void choose_samples(void);
extern void andragradare(void);
extern int economy_calc(void);
extern void insert(char* string, char* toBinserted);
extern void (*vqt_ex)(int handle, char* string, int* extent);
extern int graph_dialog(void);
extern void  mouse_on( void );
extern void  mouse_off( void );

int isNotPartOfWord(char c);
int getEndOfWordPosition( int direction ); /* direction 0 = left, 1= right */
int fraction(double decimal, FRACTION *fract);	/* Returnerar -1 om det inte gick att hitta l„mplig */
void eraseExpression(void);

void handle_keybd( int key, int kstate, int mx, int my )
{
	int extent[8];  /* F”r vqt_f_extent() */
	char tangent=0;
	char* tmp=NULL, *tmp2=NULL, *tmp3=NULL;
	FRACTION *frac=NULL;	/* F”r utr„kning av fraktion  */
	int i=0, j=0;

	if(cursor)  /* Sl„cker cursorn om den „r p† */
		redraw_window(3);

	vqt_ex ( handle, s[n], extent );

	tangent = (char)(((long) key ) & 0x0000ffff ); /* ASCII */

	if(tangent!=NK_UNDO && tangent!=NK_UP && tangent!=NK_DOWN && tangent!=NK_RIGHT && tangent!=NK_LEFT && tangent!='F'){
		for ( i=0 ; i<synliga_rader ; i++){	/* Fixa undobuffern */
			strcpy(undo[i],s[i]);
			scrollVectorBak[i]=scrollVector[i];
		}
		undoPosi=posi;
		undoN=n;
	}

	if(graph_mode && (tangent!='+') && (tangent!='-') ){
		graph_mode=0;
		strcpy(xcoordinate,"x: ");
		strcpy(ycoordinate,"y: ");
		tree[COORDINATEBOX].ob_flags |= HIDETREE;	/* G”mmer koordinatboxen. */
		redraw_window(1);
	}

	if(key&NKF_FUNC) /* Om n†gon funktionsknapp */
	{
		switch (tangent) /* N„r man trycker dessa beh”vs det aldrig scrollas */
		{
			case NK_UNDO:
				for ( i=0 ; i<synliga_rader ; i++){
					strcpy(s[i],undo[i]);
					scrollVector[i]=scrollVectorBak[i];
				}
				posi=undoPosi;
				n=undoN;
				redraw_window(2);redraw_window(3);
				return;

			case NK_TAB:	/* Autolocator */
				if(strlen(s[n])==0 || posi==0 )
					return;
				tmp = (BYTE *) Malloc (TECKEN);
				tmp2 = (BYTE *) Malloc (TECKEN);
				tmp3 = (BYTE *) Malloc (TECKEN);
				memset(tmp, 0, TECKEN);
				memset(tmp2, 0, TECKEN);
				memset(tmp3, 0, TECKEN);
				if (!tmp3)       /* Om det inte finns tillr„ckligt med minne*/
				    return;

				strcpy(tmp,s[n]);
				*(tmp+posi)=0; /* Den v„nstra halvan */
				strcpy(tmp3,s[n]+posi);	/* Allt till h”ger */

				for ( i=(int)strlen(tmp),j=0 ; i>=0 ; i--, j++)	/* R„knar hur l†ngt ordet „r */
				{
					if( isNotPartOfWord(*(tmp+i)) )
						break;
				}

				strcpy(tmp2,tmp+strlen(tmp)+1-j);	/* Kopiera ”ver ordet */

				if( *(tmp2)<='Z' && *(tmp2)>='A' )	/* Kolla i konstantlistan */
				{
					for(i=0;i<antal_konstanter;i++)
					{
						if( (strstr(konstant_lista[i],tmp2)!=NULL) && (strcspn(konstant_lista[i],tmp2)==0 ) )
						{
							posi-=(int)strlen(tmp2);
							*(tmp+strlen(tmp)+1-j)=0;
							strcpy(tmp2,konstant_lista[i]);
							posi+=(int)strlen(tmp2);
							strcat(tmp,tmp2);
							strcat(tmp,tmp3);
							strcpy(s[n],tmp);
							redraw_window(2);
							break;
						}
					}
				}
				else if(*(tmp2)<='z' && *(tmp2)>='a')	/* Kolla bland funktionerna */
				{
					for(i=0;i<ANTAL_FUNKTIONER;i++)
					{
						if( (strstr(funk_lista[i],tmp2)!=NULL) && (strcspn(funk_lista[i],tmp2)==0 ) )
						{
							posi-=(int)strlen(tmp2);
							*(tmp+strlen(tmp)+1-j)=0;
							strcpy(tmp2,funk_lista[i]);
							posi+=(int)strlen(tmp2)+1;
							strcat(tmp,tmp2);
							strcat(tmp,"()");
							strcat(tmp,tmp3);
							strcpy(s[n],tmp);
							redraw_window(2);
							break;
						}
					}
				}
				if((strstr("true",tmp2)!=NULL) && (strcspn("true",tmp2)==0 ))
				{
					posi-=(int)strlen(tmp2);
					*(tmp+strlen(tmp)+1-j)=0;
					strcpy(tmp2,"true");
					posi+=(int)strlen(tmp2)+1;
					strcat(tmp,tmp2);
					strcat(tmp,tmp3);
					strcpy(s[n],tmp);
					redraw_window(2);
				}
				else if((strstr("false",tmp2)!=NULL) && (strcspn("false",tmp2)==0 ))
				{
					posi-=(int)strlen(tmp2);
					*(tmp+strlen(tmp)+1-j)=0;
					strcpy(tmp2,"false");
					posi+=(int)strlen(tmp2);
					strcat(tmp,tmp2);
					strcat(tmp,tmp3);
					strcpy(s[n],tmp);
					redraw_window(2);
				}
				redraw_window(3);
				Mfree(tmp);Mfree(tmp2);Mfree(tmp3);
				return;
			case NK_BS:
				if( strlen(s[n])>0 && posi>0) {
					tmp = (BYTE *) Malloc (TECKEN);
					memset(tmp, 0, TECKEN);
					if (!tmp || (posi==0))       /* Om det inte finns tillr„ckligt med minne */
					    return;

					if(key&NKF_SHIFT)	{		/* Tar bort allt till v„nster */
						strcpy(tmp,s[n]+posi);
						memset(s[n],0,TECKEN);
						strcpy(s[n],tmp);
						posi=0;
						if(strlen(s[n])==0)	/* Om raden „r nu „r tom, bryt koppling. */
							scrollVector[n-1]=0;	
					}

					else if(key&NKF_CTRL) {	/* Tar bort "ordet" till v„nster */
						strcpy(tmp,s[n]);
						memset(tmp+posi,0,TECKEN-posi-1);
						strcpy(s[n],s[n]+posi);
						memset(s[n]+posi,0,TECKEN-posi-1);

						if(isNotPartOfWord( *(s[n]+posi-1) )){ /* Ta bort „ven +,- etc */
							*(tmp+posi-1)=0;
							posi--;
						}
						else{
							for ( j=posi ; j>=0 ; j--) {
								if( isNotPartOfWord(*(tmp+j)) )
									break;
								*(tmp+j)=0;
							}
							posi=j+1;
						}
						strcat(tmp,s[n]);
						strcpy(s[n],tmp);
					}

					else {
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
					}
					Mfree(tmp);
					redraw_window( 2 );
				}
				else{	/* Om raden „r tomm */
					if(scrollVector[n-1]){	/* Och raden ovanf”r h”r till samma uttryck */
						scrollVector[n-1]=0;
						n--;
						posi=(int)strlen(s[n]);
						*(s[n]+strlen(s[n])-1)=0;	/* B”rja knapra p† raden ovanf”r */
					}					
				}
				redraw_window(3);
				return;
			case NK_DEL:
				tmp = (BYTE *) Malloc (TECKEN);
				memset(tmp, 0, TECKEN);
				if (!tmp)       /* Om det inte finns tillr„ckligt med minne*/
				    return;

				if(key&NKF_SHIFT)			/* Tar bort allt till h”ger */
					memset(s[n]+posi,0,TECKEN-posi-1);

				else if(key&NKF_CTRL) {	/* Tar bort "ordet" till h”ger */
					if(posi==strlen(s[n])){
						Mfree(tmp);
						redraw_window(3);
						return;				/* Avbryter om l„ngst ut p† raden */
					}
					strcpy(tmp,s[n]+posi);
					memset((s[n]+posi),0,TECKEN-posi-1);

					if(isNotPartOfWord( *(s[n]+posi) )){
						j=1;					/* Ska ta bort tecknet mellan ord, men funkar inte */
					}
					else {
						for ( j=0 ; j<=strlen(tmp) ; j++) {
							if( isNotPartOfWord(*(tmp+j)) )
								break;
						}
					}
					strcpy(tmp,tmp+j);
					strcat(s[n],tmp);
				}

				else {
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
				}
				Mfree(tmp);
				redraw_window( 2 );
				redraw_window(3);
				return;

			case NK_ESC:
			case NK_CLRHOME:
				if(s[n][0]==0)  /* Om raden „r tom, t”m hela f”nstret */
				{
					memset(s, 0, RADER*TECKEN);
					memset(scrollVector, 0, RADER);
					n=0;
				}
				else
					eraseExpression(); /* Definierad l„ngre ner */
				posi=0;
				redraw_window( 2 ); redraw_window( 3 );
				return;

			case NK_RET:
			case NK_ENTER:
				if(key&NKF_CTRL)	/* Flytta ned raden, struntar i scrollVector */
				{
					strcpy(s[n+1],s[n]);
					memset(s[n],0,TECKEN);
					n++;
				}
				else {
					if(n>0) {
						while(scrollVector[n-1] && n>0){	/* Tar reda p† var uttrycket b”rjar */
							n--;
						}
					}
					while(scrollVector[n]){	/* Tar reda p† var uttrycket slutar */
						n++;
					}

					strcpy(s[n],pre_calculate() );
					if(syntax_error==1)	/* Om det var fel n†nstans i uttrycket „r cursorn redan satt till den positionen */
						syntax_error=0;
					else
						posi=(int)strlen(s[n]);
				}
				redraw_window(2);
				redraw_window(3);
				return;
			case 'Q':
			case 'U':
				if(key&NKF_CTRL)
				{
					if(!_app) /* Om accessory */
					{
						wind_close( whandle );
						wind_delete(whandle);
						whandle=0;
						av_sendmesg(0,0, AV_ACCWINDCLOSED );
					}
					else
						quit=1;
				}
				return;
			case 'I':
				if(key&NKF_CTRL)
					show_info();
				if(key&NKF_ALT)
					insert( s[n], "Ü" ); /* logisk om...s†-pil */
				redraw_window(3);
				return;
			case 'S':
				if(key&NKF_ALT)
					change_settings();
				else if(key&NKF_CTRL)
					save_settings(font);
				redraw_window(3);
				return;
			case 'P':
				if(key&NKF_ALT)
				{
					if(fontpi)
						insert( s[n], "ã" );
					else
						insert( s[n], "Pi" );
				}
				redraw_window(3);
				return;
			case '2':
				if(key&NKF_ALT && !(key&NKF_CTRL))
				{
					if(fontpi)
						insert( s[n], "ý" );
					else
						insert( s[n], "^2" );
				}
				if(key&NKF_CTRL && !(key&NKF_ALT))
				{
					if(fontpi)
						insert( s[n], "«" );
					else
						insert( s[n], "1/2" );
				}
				redraw_window(3);
				return;
			case '3':
				if(key&NKF_ALT)
					if(fontpi)
						insert( s[n], "þ" );
					else
						insert( s[n], "^3" );
				redraw_window(3);
				return;
			case '4':
				if(key&NKF_CTRL)
				{
					if(fontpi)
						insert( s[n], "¬" );
					else
						insert( s[n], "1/4" );
				}
				redraw_window(3);
				return;
			case 'M':
				if(key&NKF_ALT)
				{
					if(fontpi)
						insert( s[n], "æ" );
					else
						insert( s[n], "My" );
				}
				redraw_window(3);
				return;
			case 'O':
				if(key&NKF_ALT)
				{
					if(fontpi)
						insert( s[n], "ê" );
					else
						insert( s[n], "Omega" );
				}
				redraw_window(3);
				return;
			case '<':
				if(key&NKF_CTRL)
				{
					if(fontpi)
						insert( s[n], "ó" );
					else
						insert( s[n], "<=" );
				}
				redraw_window(3);
				return;
			case '>':
				if(key&NKF_CTRL)
				{
					if(fontpi)
						insert( s[n], "ò" );
					else
						insert( s[n], ">=" );
				}
				redraw_window(3);
				return;
			case 'E':
				if(key&NKF_ALT)
				{
					andragradare();
					redraw_window(2);
					posi=(int)strlen(s[n]);
				}
				if(key&NKF_CTRL)
				{
					i=0;
					i=economy_calc();

					if( i!=-1 ) /* D† har man inte tryckt CANCEL */
					{
						n++;
						posi=(int)strlen(s[n]);
						redraw_window(2);
					}
				}
				redraw_window(3);
				return;

			case 'F':
				if(key&NKF_ALT && !(key&NKF_CTRL))
				{
					formulas(mx,my);
					redraw_window(0);
					redraw_window(3);
				}
				if(key&NKF_CTRL && !(key&NKF_ALT))
					if(graph_dialog()!=-1){
						graph_mode=1;
						tree[COORDINATEBOX].ob_flags &= ~HIDETREE;	/* G”mmer koordinatboxen. */
						redraw_window(1);  /* Rensa sk„rmen f”rst */
						redraw_window(5);} /* Visa "grafen" */
				return;

			case 'G':
				if(key&NKF_ALT && !(key&NKF_CTRL)){
					choose_samples(); /* V„lj sampling */
					redraw_window(3);
				}
				if(key&NKF_CTRL && !(key&NKF_ALT))
				{
					first=1;
					redraw_window(2);
				}
				return;
			case 'Z':
				if(key&NKF_ALT)
				{
					font=choose_font();
					vst_font( handle, font );
					redraw_window(2);redraw_window(0);
				}
				redraw_window(3);
				return;
			case 'C':
				if(key&NKF_CTRL && !(key&NKF_ALT) && !(key&NKF_SHIFT))
					scrap_write(0);
				if(key&NKF_CTRL && !(key&NKF_ALT) && (key&NKF_SHIFT))
					scrap_write(2);
				if(key&NKF_ALT && !(key&NKF_CTRL) && !(key&NKF_SHIFT))
					colour_select();
				redraw_window(3);
				return;
			case 'X':
				if(key&NKF_CTRL && !(key&NKF_SHIFT))
				{
					scrap_write(1);
					redraw_window(2);
				}
				if(key&NKF_CTRL && (key&NKF_SHIFT))
				{
					scrap_write(3);
					redraw_window(2);
				}
				redraw_window(3);
				return;
			case NK_HELP:
				stguide_help();
				redraw_window(3);
				return;
			case 'R':
				if(key&NKF_ALT && !hex && !(key&NKF_CTRL))
				{
					gcvt( round(atof( s[n] )), 15, s[n] ); /* Rundar av till n„rmsta heltal */
					posi=(int)strlen(s[n]);
					redraw_window( 2 );
				}
				if(key&NKF_CTRL && !hex && !(key&NKF_ALT))
				{
					frac->numerator=(long)Malloc(sizeof("    ")); /* Allokera fyra byte = long */
					frac->denominator=(long)Malloc(sizeof("    "));

					tmp = (BYTE *) Malloc (TECKEN);
					tmp2 = (BYTE *) Malloc (TECKEN);
					tmp3 = (BYTE *) Malloc (TECKEN);

					memset(tmp,0,sizeof(tmp));
					memset(tmp2,0,sizeof(tmp2));
					memset(tmp3,0,sizeof(tmp2));

					i=0;
					if(strlen(s[n])!=0){
						strcpy(tmp3,s[n]);

						if(strstr(tmp3,"."))
						{
							strcpy(tmp2,(strstr(tmp3,".")) );
							strcpy(tmp,tmp3);
							*(tmp3 + strlen(tmp3)-strlen(strstr(tmp3,".")))='\0';
							if(strcmp(tmp3,"0")==0 || strcmp(tmp3,"")==0){ /* Om det bara „r t.ex. 0.75 */
								strcpy(tmp3,"");
								i=1; /* Flagga att det bara var en nolla, inget "+" */
							}

							if(fraction( atof(tmp2), frac )!=-1) { /* Returnerar -1 om den inte kunde hitta l„mplig fraktion */
								if(!i)	/* Om bara en nolla eller ingenting*/
									strcat(tmp3, "+");
								strcat(tmp3, ltoa(frac->numerator, tmp, 10));
								strcat(tmp3, "/");
								strcat(tmp3, ltoa(frac->denominator, tmp, 10));
								strcpy(s[n], tmp3);
								posi=(int)strlen(s[n]);
							}
						}
					}

					Mfree(&frac->numerator);Mfree(&frac->denominator);Mfree(tmp);Mfree(tmp2);Mfree(tmp3);
					redraw_window( 2 );
				}
				redraw_window(3);
				return;
			case 'A':
				if(key&NKF_ALT)
				{
					gcvt( fabs( atof(  s[n] )), 15, s[n] ); /* Absoluttalet */
					posi=(int)strlen(s[n]);
					redraw_window( 2 );
				}
				redraw_window(3);
				return;
			case 'B':
				if(key&NKF_ALT)
				{
					strcpy( s[n], strrev(s[n] ) );          /* V„nder p† talet, bakofram */
					redraw_window( 2 );
				}
				redraw_window(3);
				return;

			/* Piltangenterna */
			case NK_UP:
				if(n==0)
				{
					redraw_window( 3 );
					return;
				}
				if(key&NKF_SHIFT)
					n=0;
				else
					n--;
				posi=(int)strlen(s[n]);
				redraw_window( 3 );
				return;
			case NK_DOWN:
				if(n==synliga_rader-1)
				{
					redraw_window( 3 );
					return;
				}
				if(key&NKF_SHIFT)
					n=synliga_rader-1;
				else
					n++;
				posi=(int)strlen(s[n]);
				redraw_window( 3 );
				return;
			case NK_LEFT:
				if(posi==0){
					redraw_window(3);
					return;
				}
				if(key&NKF_SHIFT)
					posi=0;
				else if(key&NKF_CTRL){	/* Stega ett ord i taget */
					if(isNotPartOfWord( *(s[n]+posi-1) )) /* Om +,- etc */
						posi--;
					else
						posi=getEndOfWordPosition(LEFT)+1;
				}
				else
					posi--;
				redraw_window( 0 );
				redraw_window(3);
				return;
			case NK_RIGHT:
				if(posi==(int)strlen(s[n])){
					redraw_window(3);
					return;
				}
				if(key&NKF_SHIFT)
					posi=(int)strlen(s[n]);
				else if(key&NKF_CTRL){	/* Stega ett ord i taget */
					if(isNotPartOfWord( *(s[n]+posi) ))
							posi++;
					else
						posi=getEndOfWordPosition(RIGHT);
				}
				else
					posi++;
				redraw_window( 0 );
				redraw_window(3);
				return;
		}
	}

	if( n==synliga_rader-1 && ( extent[2] >= tree[RUTAN].ob_width - 2*teckenbredd )) /* Om p† sista raden l„ngst ut, scrolla */
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
		redraw_window( 2 );
	}

	else if( extent[2] >= tree[RUTAN].ob_width - 2*teckenbredd )
	{
		s[n][ strlen(s[n]) ] = 0; /* F”r att s„tta sista nollan s† det blir en str„ng */
		scrollVector[n]=1;	/* Ettst„ller om h”r ihop med raden nedanf”r. */
		n++;
		memset(s[n],0,TECKEN);
		posi=0;
		redraw_window( 2 );
	}

	if(key&NKF_FUNC)
	{
		switch (tangent)                 /* H„r skall det d„remot scrollas */
		{
			case NK_F1:                   /* Funktionstangenterna */
				if(key&NKF_ALT && !(key&NKF_CTRL))
					insert( s[n], "asin()" );
				else if(key&NKF_CTRL && !(key&NKF_ALT))
					insert( s[n], "sinh()" );
				else
					insert( s[n], "sin()" );
				posi--;
			break;
			case NK_F2:
				if(key&NKF_ALT && !(key&NKF_CTRL))
					insert( s[n], "acos()" );
				else if(key&NKF_CTRL && !(key&NKF_ALT))
					insert( s[n], "cosh()" );
				else
					insert( s[n], "cos()" );
				posi--;
			break;
			case NK_F3:
				if(key&NKF_ALT && !(key&NKF_CTRL))
					insert( s[n], "atan()" );
				else if(key&NKF_CTRL && !(key&NKF_ALT))
					insert( s[n], "tanh()" );
				else
					insert( s[n], "tan()" );
				posi--;
			break;
			case NK_F4:
				insert( s[n], "exp()" );
				posi--;
			break;
			case NK_F5:
				if(key&NKF_ALT)
					insert( s[n], "log()" );
				else
					insert( s[n], "ln()" );
				posi--;
			break;
			case NK_F6:
				if(key&NKF_ALT && !(key&NKF_CTRL))
					insert( s[n], "floor()" );
				else if(key&NKF_CTRL && !(key&NKF_ALT))
					insert( s[n], "ceil()" );
				else
					insert( s[n], "abs()" );
				posi--;
			break;
			case NK_F7:
				insert( s[n], "heav()" );
				posi--;
			break;
			case NK_F8:
				if(key&NKF_ALT)
				{
					if(fontpi)
						insert( s[n], "û()" );
					else
						insert( s[n], "sqrt()" );
					posi--;
				}
				else
				{
					if(fontpi)
						insert( s[n], "ý" );
					else
						insert( s[n], "^2" );
				}
			break;
			case NK_F9:
				insert( s[n], "rand()" );
				posi--;
			break;
			case NK_F10:
				insert( s[n], "ndist()" );
				posi--;
			break;
			case 'V':
				if(key&NKF_CTRL)
					scrap_read();
			break;
			case '(':
				if(key&NKF_ALT)
					insert( s[n], "(" );
			break;
			case ' ':
				if(key&NKF_CTRL && key&NKF_ALT) /* Ikonifiera */
				{
					if(!iconified)
					{
						wind_get(whandle, WF_CURRXYWH, &tmp_x, &tmp_y, &tmp_w, &tmp_h);
						wind_set(whandle, WF_ICONIFY, -1, -1, -1, -1);
						iconified=1;
					}
					else
					{
						wind_set(whandle, WF_UNICONIFY, tmp_x, tmp_y, tmp_w, tmp_h);
						iconified=0;
					}
				}
			break;
			default:
				if(sendkey)
				{
					key=nkc_n2gem(key);        /* Konverteras fr†n normaliserat */
					kstate= (kstate >> 8);     /* till vanligt format.          */
					av_sendmesg(key,kstate,AV_SENDKEY);
				}
			break;
		}
		redraw_window(3);
		return;
	}

	if(!(key&NKF_FUNC))
	{
		switch (tangent)
		{
			case '1':
				insert( s[n], "1" );
				break;
			case '2':
				insert( s[n], "2" );
				break;
			case '3':
				insert( s[n], "3" );
				break;
			case '4':
				insert( s[n], "4" );
				break;
			case '5':
				insert( s[n], "5" );
				break;
			case '6':
				insert( s[n], "6" );
				break;
			case '7':
				insert( s[n], "7" );
				break;
			case '8':
				insert( s[n], "8" );
				break;
			case '9':
				insert( s[n], "9" );
				break;
			case '0':
				insert( s[n], "0" );
				break;
			case '+':
				if(graph_mode){
					if((fabs(xmin)>1&&fabs(xmax)>1&&fabs(ymin)>1&&fabs(ymax)>1))
					{
						tree[ZOOMIN].ob_flags |= HIDETREE;
						redraw_window(4); /* R„kna ut clipping */
						objc_draw(tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3]);
						xmin=round(xmin/2); xmax=round(xmax/2); ymin=round(ymin/2); ymax=round(ymax/2);
						redraw_window(1);	/* Rensa sk„rmen f”rst */
						redraw_window(5);	/* Visa "grafen" */

						tree[ZOOMIN].ob_flags &= ~HIDETREE;
						objc_draw(tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3]);
					}
				}
				else
					insert( s[n], "+" );
				break;
			case '-':
				if(graph_mode){
					if((fabs(xmin)<HUGE_VAL/2 &&fabs(xmax)<HUGE_VAL/2&&fabs(ymin)<HUGE_VAL/2&&fabs(ymax)<HUGE_VAL/2))
					{
						redraw_window(4);
						tree[ZOOMOUT].ob_flags |= HIDETREE;
						objc_draw(tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3]);
						xmin=round(xmin*2); xmax=round(xmax*2); ymin=round(ymin*2); ymax=round(ymax*2);
						redraw_window(1);	/* Rensa sk„rmen f”rst */
						redraw_window(5);	/* Visa "grafen" */

						tree[ZOOMOUT].ob_flags &= ~HIDETREE;
						objc_draw(tree, COORDINATEBOX, MAX_DEPTH, clip[0], clip[1], clip[2], clip[3]);
					}
				}
				else
					insert( s[n], "-" );
				break;
			case '*':
				if(fontpi)
					insert( s[n], "ú" );
				else
					insert( s[n], "*" );
				break;
			case '/':
				insert( s[n], "/" );
				break;
			case '%':
				insert( s[n], "%" );
				break;
			case '&':
				insert( s[n], "&" );
				break;
			case '~':
				insert( s[n], "~()" );
				posi--;
				break;
			case '|':
				insert( s[n], "|" );
				break;
			case '(':
				insert( s[n], "()" );
				posi--;
				break;
			case ')':
				insert( s[n], ")" );
				break;
			case '.':
				insert( s[n], "." );
				break;
			case ',':
				insert( s[n], "," );
				break;
			case '^':
				insert( s[n], "^" );
				break;
			case '=':
				insert( s[n], "=" );
				break;
			case '<':
				insert( s[n], "<" );
				break;
			case '>':
				insert( s[n], ">" );
				break;
			case '!':
				insert( s[n], "!()" );
				posi--;
				break;
			case ' ':
				insert( s[n], " " );	/* space */
				break;
			/* Bokst„verna */

			case 'a':
				insert( s[n], "a" );
				break;
			case 'b':
				insert( s[n], "b" );
				break;
			case 'c':
				insert( s[n], "c" );
				break;
			case 'd':
				insert( s[n], "d" );
				break;
			case 'e':
				insert( s[n], "e" );
				break;
			case 'f':
				insert( s[n], "f" );
				break;
			case 'g':
				insert( s[n], "g" );
				break;
			case 'h':
				insert( s[n], "h" );
				break;
			case 'i':
				insert( s[n], "i" );
				break;
			case 'j':
				insert( s[n], "j" );
				break;
			case 'k':
				insert( s[n], "k" );
				break;
			case 'l':
				insert( s[n], "l" );
				break;
			case 'm':
				insert( s[n], "m" );
				break;
			case 'n':
				insert( s[n], "n" );
				break;
			case 'o':
				insert( s[n], "o" );
				break;
			case 'p':
				insert( s[n], "p" );
				break;
			case 'q':
				insert( s[n], "q" );
				break;
			case 'r':
				insert( s[n], "r" );
				break;
			case 's':
				insert( s[n], "s" );
				break;
			case 't':
				insert( s[n], "t" );
				break;
			case 'u':
				insert( s[n], "u" );
				break;
			case 'v':
				insert( s[n], "v" );
				break;
			case 'w':
				insert( s[n], "w" );
				break;
			case 'x':
				insert( s[n], "x" );
				break;
			case 'y':
				insert( s[n], "y" );
				break;
			case 'z':
				insert( s[n], "z" );
				break;
			case 'A':
				insert( s[n], "A" );
				break;
			case 'B':
				insert( s[n], "B" );
				break;
			case 'C':
				insert( s[n], "C" );
				break;
			case 'D':
				insert( s[n], "D" );
				break;
			case 'E':
				insert( s[n], "E" );
				break;
			case 'F':
				insert( s[n], "F" );
				break;
			case 'G':
				insert( s[n], "G" );
				break;
			case 'H':
				insert( s[n], "H" );
				break;
			case 'I':
				insert( s[n], "I" );
				break;
			case 'J':
				insert( s[n], "J" );
				break;
			case 'K':
				insert( s[n], "K" );
				break;
			case 'L':
				insert( s[n], "L" );
				break;
			case 'M':
				insert( s[n], "M" );
				break;
			case 'N':
				insert( s[n], "N" );
				break;
			case 'O':
				insert( s[n], "O" );
				break;
			case 'P':
				insert( s[n], "P" );
				break;
			case 'Q':
				insert( s[n], "Q" );
				break;
			case 'R':
				insert( s[n], "R" );
				break;
			case 'S':
				insert( s[n], "S" );
				break;
			case 'T':
				insert( s[n], "T" );
				break;
			case 'U':
				insert( s[n], "U" );
				break;
			case 'V':
				insert( s[n], "V" );
				break;
			case 'W':
				insert( s[n], "W" );
				break;
			case 'X':
				insert( s[n], "X" );
				break;
			case 'Y':
				insert( s[n], "Y" );
				break;
			case 'Z':
				insert( s[n], "Z" );
				break;
			default:
				break;
		}
	}
	if(!cursor)
		redraw_window(3); /* Cursorn skall h„nga med */
}

void show_info(void)
{
	TEDINFO *obspec;
	char *strptr;
	int exitbutn;

	rsrc_gaddr( R_TREE, INFOBOX, &info );
	obspec = info[VERSION].ob_spec.tedinfo;
	strptr = obspec->te_ptext;
	strptr[0] = 0;
	strcpy(strptr, Version);
	exitbutn=do_dialog(info);

	if(exitbutn==EMAIL)
		start_mailer();
	if(exitbutn==URL)
		start_browser();
}

int isNotPartOfWord(char c)
{
	if( c=='+' || c=='-' || c=='*' || c=='/' || c=='%'  || c=='&' || c=='|' || c=='Ü' || c=='~' || c=='(' || c==')' || c=='.' || c=='<' || c=='=' || c=='>' || c=='ò' || c=='ó' || c=='ú' )
		return 1;
	else
		return 0;
}

/* Decimals to Fractions -- Given a floating point number,
   finds the 'best' rational approximation (by using continued fractions).  */

int fraction(double decimal, FRACTION *frac)
{
	double a=0,b=1.0,num=0,den=0,ratio=0;
	long c=1,d=0,e=0,f=1,mult=0;

	a=decimal;

	for(;;)
	{
		mult=a/b;
		a-=mult*b;
		c-=mult*d;
		e-=mult*f;
		if(c>10000 ||c==100 || c==1000 || c==10000)	/* Avbryter, rutinen leverar annars sjuka grejjer, som 0.67 nom=67 den=100 */
			break;
		num=-e;
		den=c;
		ratio=num/den;
		if(decimal-ratio<EPS)
			break;

		mult=b/a;
		b-=mult*a;
		d-=mult*c;
		f-=mult*e;
		if(-d>10000 ||c==100 || c==1000 || c==10000)	/* Avbryter, rutinen leverar annars sjuka grejjer, som 0.67 nom=67 den=100 */
			break;
		num=f;
		den=-d;
		ratio=num/den;
		if(ratio-decimal<EPS)
			break;
	}
	if( ((decimal-ratio)<EPS) || ((ratio-decimal)<EPS) ){ /* Om den kunde hitta med godtagbar noggranhet */
		frac->numerator=num;											/* Denna if-sats har ingen verkan. Varf”r? */
		frac->denominator=den;
		return 0;
	}
	else
		return -1;
}

int getEndOfWordPosition( int direction )
{
	char *tmp=NULL;
	int tmp_posi=0, j=0;

	tmp = (BYTE *) Malloc (TECKEN);
	if (!tmp){       /* Om det inte finns tillr„ckligt med minne*/
		redraw_window(3);
	   return posi;
	}
	memset(tmp, 0, TECKEN);

	if(direction==RIGHT){ /* h”ger, slut av ord */
		strcpy(tmp,s[n]+posi);

		for ( j=0 ; j<=strlen(tmp) ; j++)
		{
			if( isNotPartOfWord(*(tmp+j)) )
				break;
		}

		tmp_posi=posi+j;
		if(tmp_posi>(int)strlen(s[n]))
			tmp_posi=(int)strlen(s[n]);
	}
	else {	/* v„nster, b”rjan av ord */
		strcpy(tmp,s[n]);
		*(tmp+posi)=0;	/* Beh”vs av n†n outgrundlig anledning */

		for ( j=posi ; j>=0 ; j--)
		{
			if( isNotPartOfWord(*(tmp+j)) )
				break;
		}

		tmp_posi=j;
		if(tmp_posi<-1)	/* Det „r n†t meck h„r, m†ste k”ra +1 efter†t */
			tmp_posi=-1;
	}
	Mfree(tmp);
	return tmp_posi;
}

void eraseExpression(void) {
	int i=0;
	
	memset(s[n],0,TECKEN);	/* Radera alltid raden */
	i=n;
	if(i>0) {
		while(scrollVector[i-1] && i>0){	/* Tar reda p† var uttrycket b”rjar */
			i--;
		}
	}
	n=i;	/* S„tter mark”ren p† raden d„r uttrycket b”rjar */
	memset(s[n],0,TECKEN);	/* Radera alltid raden uttrycket b”rjade p† */
	while(scrollVector[i]){
		memset(s[i+1],0,TECKEN);	/* Radera raden under om de h”r ihop */
		scrollVector[i]=0;
		i++;
	}
}