

/* economy_calc(void)
 * R„knar ut annuitet, nuv„rde, nusumma eller slutv„rde
 *********************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <aes.h>
#include <portab.h>
#include <tos.h>
#include <math.h>
#include "newton.h"
#include "xtras.h"

extern OBJECT *economy;
extern int MagX, MiNT, n;

extern char s[RADER][TECKEN];

extern int do_dialog( OBJECT *dialog);
extern char* calculate ( char *string );

TEDINFO *obspec11, *obspec22, *obspec33;	
char *strptr11, *strptr22, *strptr33;

int economy_calc(void)
{
	int exitbutn;
	char *buf=NULL;
	double tmp, r=0, years=0, value=0;

	rsrc_gaddr( R_TREE, ECONOMY, &economy );

	obspec11 = economy[CAPITAL].ob_spec.tedinfo;
	obspec22 = economy[YEARS].ob_spec.tedinfo;
	obspec33 = economy[INTERESTRATE].ob_spec.tedinfo;
	strptr11 = obspec11->te_ptext;
	strptr22 = obspec22->te_ptext;
	strptr33 = obspec33->te_ptext;

	memset(strptr11,0,20);
	memset(strptr22,0,20);
	memset(strptr33,0,20);

	if(strlen(s[n]))
		strcpy(strptr11,s[n]);

	exitbutn=do_dialog(economy);

	if(exitbutn==CANCEL_ECO)
		return -1;

	else
	{
		r=atof(strptr33)/100;
		years=atof(strptr22);
		value=atof(strptr11);

		buf = (char*)Malloc(40);
		memset(buf,0, sizeof(buf));

		if(economy[ENDVALUE].ob_state & SELECTED)	/* M†ste kolla s† alla „r ifyllda ocks† */
		{
			tmp=1+r;							/* (1+r)^n */
			tmp=pow(tmp,years);
			
			strcpy(s[n],gcvt(tmp*value,15,buf));
		}
		else if(economy[NUVALUE].ob_state & SELECTED)
		{
			tmp=1+r;							/* (1+r)^-n */
			tmp=pow(tmp,-years);
			
			strcpy(s[n], gcvt(tmp*value,15,buf));			
		}
		else if(economy[NUSUMME].ob_state & SELECTED)
		{
			tmp=1+r;							/* (1-(1+r)^-n)/r */
			tmp=pow(tmp,-years);
			tmp=1-tmp;
			tmp=tmp/r;
			
			strcpy(s[n], gcvt(tmp*value,15,buf));
		}
		else if(economy[ANNUITET].ob_state & SELECTED)
		{
			tmp=1+r;							/* r/(1-(1+r)^-n) */
			tmp=pow(tmp,-years);
			tmp=1-tmp;
			tmp=r/tmp;
			
			strcpy(s[n], gcvt(tmp*value,15,buf));
		}
		else if(economy[SLUTSFAKTOR].ob_state & SELECTED)
		{
			tmp=1+r;							/* ((1+r)^n-1)/r */
			tmp=pow(tmp,years);
			tmp=tmp-1;
			tmp=tmp/r;
			
			strcpy(s[n], gcvt(tmp*value,15,buf));
		}
	}
	Mfree(buf);
	return 0;
}