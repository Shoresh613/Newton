

/* void pre_calculate( void )
 * Scrolls and first makes a morals check,
 * then calculates the expression.
 *********************************************/


#include <tos.h>
#include <portab.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "newton.h"
#include "xtras.h"

extern int bad_language( char *string );
extern void redraw_window(int pos);
extern char* calculate ( char *string );
extern void error_sound( int sound );
extern void redraw_window( int all );
void scroll( void );
char* preprocess(char *string);

extern int n;
extern char s[RADER][TECKEN];
extern int scrollVector[RADER];
extern int moral_check, synliga_rader, play_sample;
extern int MagX, MiNT;
extern char konstant_lista[ANTAL_ENHETER][MAX_TECKEN]; /* These are declared in newton.c, loaded at boot from newton.cnf, then copied over to variable* in cuttryck in calculate() */
extern double konstant_storhet[ANTAL_ENHETER];
extern int antal_konstanter;

char* pre_calculate ( void )
{
	char *tmp=NULL, *tmp2=NULL, *string=NULL;
	char *Set_Constant_Error=NULL;
	int i=0, finns=0,scrollStart=0,scrollEnd=0;
	
	if(s[n][0]==0)                         /* If the display is empty */
		return "";

	scrollStart=n;
	if(scrollStart>0) {
		while(scrollVector[scrollStart-1] && scrollStart>0){	/* Finds where the expression starts */
			scrollStart--;
		}
	}
	i=scrollStart;
	while(scrollVector[i]){
		i++;
	}
	scrollEnd=i;
	
	tmp = (BYTE *) Malloc (TECKEN*(scrollEnd-scrollStart+1));
	tmp2 = (BYTE *) Malloc (TECKEN);
	string = (BYTE *) Malloc (TECKEN*(scrollEnd-scrollStart+1));

	if(!string)	/* If out of memory */
		return "";


	memset(tmp,0,sizeof(tmp));
	memset(tmp2,0,sizeof(tmp2));
	memset(string,0,sizeof(string));

	if(scrollEnd!=scrollStart)
	{
		i=scrollStart;
		strcpy(tmp,s[scrollStart]);
		while(scrollVector[i])		/* As long as they belong together, merge */
		{
			strcat(tmp,s[i+1]);
			i++;
		}
		strcpy(string,tmp);
	}
	else
		strcpy(string,s[n]);

	if(moral_check)
	{
		if( bad_language(string) == 1)       /* Check morals of the user ;)      */
		{
			if( play_sample )
				error_sound( BAD_LANGUAGE_SOUND ); /* Funny little sound */
			Mfree(tmp);Mfree(tmp2);Mfree(string);
			return "Bad language error";
		}
	}

	strcpy(string,preprocess(string));	/* The memory leak? Preprocess so it is stored in the same memory location? */
	scroll();                 /* If on last line, scroll */

	if(strstr(string,"=") && (strstr(string,"==")==NULL) && (strstr(string,"<")==NULL) && (strstr(string,">")==NULL) && (antal_konstanter<ANTAL_ENHETER) ) /* Assign constant a value */
	{
		strcpy(tmp2,(strstr(string,"=")+1) );
		memset(string + strlen(string)-strlen(strstr(string,"=")),0,TECKEN*(scrollEnd-scrollStart+1)-strlen(string)-strlen(strstr(string,"=")));	/* Zeroes the string from and including the equal sign */ 
	/*	*(string + strlen(string)-strlen(strstr(string,"=")))='\0'; */
		
		if( !(*(string)<='Z' && *(string)>='A'))	/* If not a capital letter */
		{
			rsrc_gaddr( R_STRING, SETCONSTANT, &Set_Constant_Error );
			form_alert( 1,Set_Constant_Error);
			Mfree(tmp); Mfree(tmp2);Mfree(string);
			redraw_window( 2 );
			return s[n];
		}
		else
		{
			for(i=0;i<ANTAL_ENHETER;i++)	/* Checks if the variable already exists */
			{
				if(! strcmp(konstant_lista[i],string))
				{
					finns=1;
					break;
				}
			}
			if(finns)
			{
				strcpy(konstant_lista[i],string);
				konstant_storhet[i]=atof(calculate(preprocess(tmp2)));
			}
			else
			{
				strcpy(konstant_lista[antal_konstanter],string);
				konstant_storhet[antal_konstanter]=atof(calculate(preprocess(tmp2)));
				antal_konstanter++;
			}
			strcpy(string,tmp2);
		}
	}

	if(strstr(string,"==") || strstr(string,"<") || strstr(string,">")) /* Comparison test */
	{
		if(strstr(string,"==") && strstr(string,"<")==NULL && strstr(string,">")==NULL)
		{
			strcpy(tmp2,(strstr(string,"==")+2) );
			strcpy(tmp,string);
			*(string + strlen(string)-strlen(strstr(string,"=")-1))='\0';

			if( strcmp(calculate(tmp), calculate(tmp2)) == 0 )
				strcpy(string,"true");
			else
				strcpy(string, "false");
		}
		if(strstr(string,"<") && strstr(string,"=")==NULL )
		{
			strcpy(tmp2,(strstr(string,"<")+1) );
			strcpy(tmp,string);
			*(string + strlen(string)-strlen(strstr(string,"<")-1))='\0';

			if( atof(calculate(tmp)) - atof(calculate(tmp2)) < 0 )
				strcpy(string,"true");
			else
				strcpy(string, "false");
		}
		else if(strstr(string,">") && strstr(string,"=")==NULL )
		{
			strcpy(tmp2,(strstr(string,">")+1) );
			strcpy(tmp,string);
			*(string + strlen(string)-strlen(strstr(string,">")-1))='\0';

			if( atof(calculate(tmp)) - atof(calculate(tmp2)) > 0 )
				strcpy(string,"true");
			else
				strcpy(string, "false");
		}
		if( strstr(string,"<=") )
		{
			strcpy(tmp2,(strstr(string,"=")+1) );
			strcpy(tmp,string);
			*(string + strlen(string)-strlen(strstr(string,"=")-1))='\0';

			if( atof(calculate(tmp)) <= atof(calculate(tmp2)) )
				strcpy(string,"true");
			else
				strcpy(string, "false");
		}
		else if( strstr(string,">=") )
		{
			strcpy(tmp2,(strstr(string,"=")+1) );
			strcpy(tmp,string);
			*(string + strlen(string)-strlen(strstr(string,"=")-1))='\0';

			if( atof(calculate(tmp)) >= atof(calculate(tmp2)) )
				strcpy(string,"true");
			else
				strcpy(string, "false");
		}
	}
	else
		strcpy(string,calculate( string ));

/*	if(strcmp(string,"666")==0)
		strcpy(string, "Your computer is posessed!! (666)"); */

	if( n!=synliga_rader-1 )  /* If not on last row, move down one row */
	{
		n++;
		if(s[n][0]!=0)        /* Clears the result row first */
		{
			memset(s[n],0,TECKEN);
			redraw_window(2);
		}
	}

	Mfree(tmp); Mfree(tmp2);Mfree(string);
	redraw_window( 2 );
	return string;
}

void scroll(void)
{
	if( n==synliga_rader-1 ) /* If on the last row, scroll */
	{
		n=0;
		while(n<synliga_rader)
		{
			strcpy(s[n],s[n+1]);
			scrollVector[n]=scrollVector[n+1];	/* Must include the scroll vector too */
			n++;
		}
		s[synliga_rader-1][0]=0;
		n=synliga_rader-2;
	}
}

char* preprocess(char *string)
{
	char *tmp=NULL;
	int i=0;
	int scrollStart=n,scrollEnd=0;
	
	if(scrollStart>0) {
		while(scrollVector[scrollStart-1] && scrollStart>0){	/* Finds where the expression starts */
			scrollStart--;
		}
	}
	i=scrollStart;
	while(scrollVector[i]){
		i++;
	}
	scrollEnd=i;

	tmp = (BYTE *) Malloc (TECKEN*(scrollEnd-scrollStart+1));
	memset(tmp,0,sizeof(tmp));
	
	for ( i=1 ; i<strlen(string)-1 ; i++ ) /* This running loop ensures that expressions */
	{										/* of type 1.28e4 can be calculated (=12800)    */
		if( *(string+i)=='e' )
		{
			if( ( isdigit( (int)(char)*(string+i+1)) || (int)(char)*(string+i+1)=='-' ) 
			&& ( isdigit( (int)(char)*(string+i-1)) ) )
			{
				memset(tmp,0,sizeof(tmp));
				strcpy(tmp,(string+i+1));
				memset(string+i,0,strlen(string)-(i+1));
				strcat(string,"*10^");
				strcat(string,tmp);
			}
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* , -> . */
		if( *(string+i)==',' )
			*(string+i)='.';

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> Pi / *Pi  */
	{
		if( *(string+i)=='�' ) /* Pi */
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			if( (isdigit( (int)(char)*(string+i-1)) || isalpha( (char)*(string+i-1)) || (char)*(string+i-1)==')' ) && i ) /* S� kan man �ven skriva 5Pi ist. f�r 5*Pi */
			{
				*(string+i)='*';
				i++;
			}
			*(string+i)='P';
			*(string+i+1)='i';
			*(string+i+2)='\0';

			if(isdigit( (int)(char)*(tmp)) || isalpha( (char)*(tmp)) || (char)*(tmp)=='(' ) /* S� kan man �ven skriva PiKaka ist. f�r Pi*Kaka */
				strcat(string,"*");

			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> Omega */
	{
		if( *(string+i)=='�' ) /* Omega */
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			if( (isdigit( (int)(char)*(string+i-1)) || isalpha( (char)*(string+i-1)) || (char)*(string+i-1)==')' ) && i ) /* S� kan man �ven skriva 5Pi ist. f�r 5*Pi */
			{
				*(string+i)='*';
				i++;
			}
			*(string+i)='O';
			*(string+i+1)='m';
			*(string+i+2)='e';
			*(string+i+3)='g';
			*(string+i+4)='a';
			*(string+i+5)='\0';

			if(isdigit( (int)(char)*(tmp)) || isalpha( (char)*(tmp)) || (char)*(tmp)=='(' ) /* S� kan man �ven skriva PiKaka ist. f�r Pi*Kaka */
				strcat(string,"*");

			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> ^2 */
	{
		if( *(string+i)=='�' ) /* Square */
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i)='^';
			*(string+i+1)='(';
			*(string+i+2)='2';
			*(string+i+3)=')';
			*(string+i+4)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> ^3 */
	{
		if( *(string+i)=='�' ) /* Cube */
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i)='^';
			*(string+i+1)='(';
			*(string+i+2)='3';
			*(string+i+3)=')';
			*(string+i+4)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � (square root) -> sqrt */
	{
		if( *(string+i)=='�' ) /* Square Root */
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i)='s';
			*(string+i+1)='q';
			*(string+i+2)='r';
			*(string+i+3)='t';
			*(string+i+4)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> My */
	{
		if( *(string+i)=='�' ) /* my */
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i)='(';
			*(string+i+1)='M';
			*(string+i+2)='y';
			*(string+i+3)=')';
			*(string+i+4)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> (1/2) */
	{
		if( *(string+i)=='�' )
		{
			if( ( !isdigit( (int)(char)*(string+i+1)) )
			&& ( ( !isdigit( (int)(char)*(string+i-1)) ) || i==0  ) )
			{
				memset(tmp,0,sizeof(tmp));
				strcpy(tmp,(string+i+1));
				*(string+i)='(';
				*(string+i+1)='1';
				*(string+i+2)='/';
				*(string+i+3)='2';
				*(string+i+4)=')';
				*(string+i+5)='\0';
				strcat(string,tmp);
			}
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> (1/4) */
	{
		if( *(string+i)=='�' )
		{
			if( ( !isdigit( (int)(char)*(string+i+1)) )
			&& ( ( !isdigit( (int)(char)*(string+i-1)) ) || i==0  ) )
			{
				memset(tmp,0,sizeof(tmp));
				strcpy(tmp,(string+i+1));
				*(string+i)='(';
				*(string+i+1)='1';
				*(string+i+2)='/';
				*(string+i+3)='4';
				*(string+i+4)=')';
				*(string+i+5)='\0';
				strcat(string,tmp);
			}
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> >= */
	{
		if( *(string+i)=='�' )
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i)='>';
			*(string+i+1)='=';
			*(string+i+2)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* � -> <= */
	{
		if( *(string+i)=='�' )
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i)='<';
			*(string+i+1)='=';
			*(string+i+2)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* log( -> logd( */
	{
		if( *(string+i)=='l' && *(string+i+1)=='o' && *(string+i+2)=='g' && *(string+i+3)=='(')
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+4));
			*(string+i+3)='d';
			*(string+i+4)='(';
			*(string+i+5)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* ln( -> log( */
	{
		if( *(string+i)=='l' && *(string+i+1)=='n' && *(string+i+2)=='(')
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+3));
			*(string+i+1)='o';
			*(string+i+2)='g';
			*(string+i+3)='(';
			*(string+i+4)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ )  /* !( -> fak( */
	{
		if( *(string+i)=='!' && *(string+i+1)=='(' )
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+2));
			*(string+i)='f';
			*(string+i+1)='a';
			*(string+i+2)='k';
			*(string+i+3)='(';
			*(string+i+4)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ )  /* ~( -> neg( */
	{
		if( *(string+i)=='~' && *(string+i+1)=='(' )
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+2));
			*(string+i)='n';
			*(string+i+1)='e';
			*(string+i+2)='g';
			*(string+i+3)='(';
			*(string+i+4)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* false -> 0*/
	{
		if( *(string+i)=='f' && *(string+i+1)=='a' && *(string+i+2)=='l' && *(string+i+3)=='s' && *(string+i+4)=='e')
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+5));
			*(string+i)='0';
			*(string+i+1)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* true -> 1*/
	{
		if( *(string+i)=='t' && *(string+i+1)=='r' && *(string+i+2)=='u' && *(string+i+3)=='e')
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+4));
			*(string+i)='1';
			*(string+i+1)='\0';
			strcat(string,tmp);
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* 2( -> 2*( */
	{
		if( isdigit((int)(char)*(string+i)) && *(string+i+1)=='(')
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i+1)='*';
			*(string+i+2)='\0';
			strcat(string,tmp);
			i++;
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* )2 -> )*2 */
	{
		if( *(string+i)==')' && ( isdigit((int)(char)*(string+i+1)) || isalpha(*(string+i+1)) ) )
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i+1)='*';
			*(string+i+2)='\0';
			strcat(string,tmp);
			i++;
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* )( -> )*( */
	{
		if( *(string+i)==')' && *(string+i+1)=='(' )
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i+1)='*';
			*(string+i+2)='\0';
			strcat(string,tmp);
			i++;
		}
	}

	for ( i=0 ; i<strlen(string) ; i++ ) /* 2x -> 2*x */
	{
		if( isdigit((int)(char)*(string+i)) && *(string+i)!='0' && *(string+i+1)=='x' )
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			*(string+i+1)='*';
			*(string+i+2)='\0';
			strcat(string,tmp);
			i++;
		}
	}
/*	for ( i=0 ; i<strlen(string) ; i++ ) /* " " -> "" Space to spaces, for comparison tests. */
	{
		if( *(string+i)==' ' )
		{
			memset(tmp,0,sizeof(tmp));
			strcpy(tmp,(string+i+1));
			memset(string+i,0,sizeof(string)-i-1);
			*(string+i)='\0';
			strcat(string,tmp);
			i++;
		}
	} Den h�r koden hj�lper inte av n�n st�rd anledning. */

	Mfree(tmp);
	return string;
}