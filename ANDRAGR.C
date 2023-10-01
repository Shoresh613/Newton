#include <aes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "newton.h"
#include "xtras.h"

extern OBJECT* andragrad;
extern char s[RADER][TECKEN];
extern int n;

extern int do_dialog(OBJECT *dialog);
extern void scroll(void);

void andragradare(void)
{
	char *strptr, *strptr2;
	TEDINFO *obspec, *obspec2;
	double D,P,Q;
	int exitbutn;
	char *ErrorForm=NULL, *complex=NULL;

	rsrc_gaddr( R_TREE, ANDRAGRAD, &andragrad );
	rsrc_gaddr( R_STRING, ANDRAALERT, &ErrorForm );
	rsrc_gaddr( R_STRING, COMPLEX, &complex );
	
	obspec = andragrad[ANDRA_P].ob_spec.tedinfo;
	strptr = obspec->te_ptext;
	memset(strptr, 0, 20);
	obspec2 = andragrad[ANDRA_Q].ob_spec.tedinfo;
	strptr2 = obspec2->te_ptext;
	memset(strptr2, 0, 20);

	do{
		exitbutn = do_dialog( andragrad );
		if(exitbutn!=ANDRA_CANCEL && (strlen(strptr)==0 || strlen(strptr2)==0) )
			form_alert(1,ErrorForm);
	}while(exitbutn!=ANDRA_CANCEL && (strlen(strptr)==0 || strlen(strptr2)==0) );

	if(exitbutn==ANDRA_CANCEL)
		return;
	else
	{
		P=atof(strptr);
		Q=atof(strptr2);
		D=(P*P)/4-Q;
		
		if(Q<0)
		{
			form_alert(1,complex);
			return;
		} 
		else
		{
			scroll();     /* Scrollar om det beh”vs */

			do{           /* S† flyttar den ner s† l†ngt det beh”vs */
				if(s[n][0]!=0)
					n++;
				scroll();
			}while(s[n][0]!=0);
			
			gcvt( -P/2 + sqrt(D), 15, s[n] );
			n++;
			gcvt( -P/2 - sqrt(D), 15, s[n] );
		}
	}
}