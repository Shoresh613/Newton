
/* R„knar ut ett uttryck.
 *********************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <tos.h>
#include <portab.h>
#include <aes.h>
#include <time.h>
#include "newton.h"
#include "xtras.h"
#include "graf.h"

struct expression_a {
	double variabel;     /* variabelv„rde av uttrycket */
	double data;         /* konstant data               */
	int var;             /* pekare till variabel lista  */
	char operation;
	struct expression_a *v_expr;
	struct expression_a *h_expr;
};

typedef struct expression_a *pExpression_a;

extern OBJECT *tree;
extern OBJECT *standard;
extern OBJECT *div_by_zero;
extern OBJECT *variable;
extern OBJECT *error;
TEDINFO *obspec, *obspec2, *obspec3;
char *strptr, *strptr2, *strptr3;

extern int n,hex,bin,antal_konstanter, posi, handle, teckenbredd, teckenhojd;
extern char s[RADER][TECKEN];
extern char konstant_lista[ANTAL_ENHETER][MAX_TECKEN];
extern double konstant_storhet[ANTAL_ENHETER];
extern int MagX, MiNT, syntax_error;
extern char *errortmpstring;

extern int do_dialog(OBJECT *dialog);
extern void hex_it(long value, char* result);
extern void bin_it(long value, char* result);
extern char* preprocess(char *string);
extern void  redraw_window( int all );
extern int bubble_help(int mx, int my, int exitobj);
extern void (*vqt_ex)(int handle, char* string, int* extent);

pExpression_a basen=0;
extern char* uttryck;
double vaerde=0;
int exitbutn=0;

int hexNumber=0;
int binNumber=0;
int octNumber=0;
int nextcharpos;
char nextchar;
char variabel_lista[ANTAL_ENHETER][MAX_TECKEN];
double variabel_storhet[ANTAL_ENHETER];
extern int rad;
int In_fel=0;
int antal_steg=0;
int antal_variabler=0;
double value();
int ok();
int alpha(char ch);
int isHexNumber(char ch);
int isBinNumber(char ch);
int isOctNumber(char ch);
int finns_i_funklista(char *strng, int *j);
void read_tecken(pExpression_a* teckenvalue);
void readchar(void);
void reporterror(void);
void readfactor_ar(pExpression_a *);
void readterm_ar(pExpression_a *);
void readexpression_a(pExpression_a *);
void evalue_expr_ar(pExpression_a result);
void readunaer(pExpression_a *);
void readnumber(double* numvalue);
void readpotens_ar(pExpression_a* value);
int liten_alpha(char ch);
int stor_alpha(char ch);
int readfunk(int* funktion);
int finns_i_variabellista(char *strng, int* j);
void laes_variabel(int* variabel);
void kolla_uttryck(void);
void delete_expr_ar(pExpression_a T);
char* calculate ( char *string );

struct{
	double (*funk)(double);
}PekGrad[ANTAL_FUNKTIONER];

struct{
	double (*funk)(double);
}Pek[ANTAL_FUNKTIONER];

double (*ff)(double,double);

const char funk_lista[ANTAL_FUNKTIONER][8]={"abs","acos","asin","atan","cos","cosh","exp"
				  ,"log","logd","sin","sinh","sqrt","tan","tanh"
				  ,"heav","rand","fak","neg","ndist","floor","ceil"};


double heav(double x)
{
	if(x<0)
		return 0;
	else
		return 1;
}

double acosGrad(double x)
{
	return acos(x)/M_PI*180;
}

double asinGrad(double x)
{
	return asin(x)/M_PI*180;
}

double atanGrad(double x){
	return atan(x)/M_PI*180;
}

double cosGrad(double x){
	return cos(x*M_PI/180);
}

double sinGrad(double x){
	return sin(x*M_PI/180);
}

double tanGrad(double x){
	return tan(x*M_PI/180);
}

double logd(double x){
	return log10(x);
}

unsigned long lrand(void) /* Denna och f”ljande h”r ihop */
{	/* Berechnet unsigned long Zufallszahl */
	unsigned long answer=0;
	unsigned long l=0;

		/* rand() liefert nur 3 Byte-Zufallszahlen, daher durch Shiften eine */
		/* sizeof(unsigned long)-Byte Zufallszahl erzeugen */
		/* Anmerkung: Da generierte Zufallszahlen in den unteren Bits haeufig nicht */
		/* sonderlich gut zufaellig sind wird das mittlere Byte benutzt */
	for(l=0;l<sizeof(unsigned long);l++)
		answer=answer*0x100+((rand() & 0xff00l) / 0x0100);

	return answer;
}

double randomGrad(double x)
{
	long nr=0, x_long=0;
	x_long=(long)x;
	srand((double) time(NULL)); /* Initiera slumpgenerator */
	nr=lrand(); /* Slumpa fram det */
	nr=nr % x_long; /* F† ner talet till r„tt omr†de */
	return (double) nr;
}

double fak(double x)
{
	if (x > 1760) /* annars kraschar han, stacken sv„mmar ”ver */
		return -1; /* Hade det inte varit f”r denna kunde stacken varit kvar p† 8K */
	if (x < 2)
		return 1;
	else
		return x * fak(x - 1);
}

double neg(double x) /* Logisk negation */
{
	if (x != 0 )
		return 0;
	else
		return 1;
}

double ndist (double x){ /* Normaldistribution, m†ste ropas upp fr†n integrate() */
	return (1/(sqrt(2*M_PI))) * exp( -( ((x*x)/2) ) );
}

double integrate(double x){ /* Intergration enligt Simpsons formel */
	double xLow = 0,
	xHigh = 0,
	p = 1,
	koeff = 0,
	h = 0,
	tolerance = 0,	/* Tolerated error limit */
	old = 0,
	fours = 0,
	lastInt = 0,
	integral = 0,
	diff = 0,
	i=1;
	char* dummy=NULL;

	if(x>=6.5)	/* Den kommer „nd† alltid returnera 1, mattebiblioteken „r inte tillr„ckligt exakta */
		return 1;
	if(x<=-6.5)
		return 0;
	
	xHigh=fabs(x);
	h=(xHigh-xLow)/2;
	old=ndist(xLow)+ndist(xHigh);
	fours=ndist(xLow+h);
	lastInt=(h/3.0)*(old+4*fours);
	
	if(x==0)	/* As defined: ndist(0)=0.5 */
		return 0.5;

	graf_mouse( M_OFF, (void *)0 );
	graf_mouse( BUSYBEE, NULL );
	graf_mouse( M_ON, (void *)0 );

	while( diff >= tolerance ) {	/* If the error isn't tolerable, repeat */
		tolerance = 0.000000001;
		h=h/2;
		old += 2*fours;
		fours=0;
		koeff=-1;

		for( i=1 ; i <= pow(2,p)  ; i++ ){
			koeff += 2;
			fours += ndist( xLow+koeff*h );
		}
		integral = (h/3.0) * (old+4*fours);
		diff = fabs( integral - lastInt );

		if( diff >= tolerance ){
			p++;
			lastInt = integral;
		}
	}

	graf_mouse( M_OFF, (void *)0 );
	graf_mouse( ARROW, NULL );
	graf_mouse( M_ON, (void *)0 );

	dummy=(char*)Malloc(15);
	memset(dummy,0,sizeof(dummy));
	integral = atof(gcvt(integral,10,dummy)); /* Avrunda till feltoleransen */ 
	Mfree(dummy);

	if(x<0)
		return 0.5-integral;
	else
		return 0.5+integral;
}

char* calculate ( char *string )
{
	int i=0;
	char *tmp=NULL;

	strcpy(errortmpstring,string);	/* F”r report_error() */

	antal_variabler=antal_konstanter;
	basen=NULL;
	ff=pow;
	PekGrad[0].funk=fabs;
	PekGrad[1].funk=acosGrad;
	PekGrad[2].funk=asinGrad;
	PekGrad[3].funk=atanGrad;
	PekGrad[4].funk=cosGrad;
	PekGrad[5].funk=cosh;
	PekGrad[6].funk=exp;
	PekGrad[7].funk=log;
	PekGrad[8].funk=logd;
	PekGrad[9].funk=sinGrad;
	PekGrad[10].funk=sinh;
	PekGrad[11].funk=sqrt;
	PekGrad[12].funk=tanGrad;
	PekGrad[13].funk=tanh;
	PekGrad[14].funk=heav;
	PekGrad[15].funk=randomGrad;
	PekGrad[16].funk=fak;
	PekGrad[17].funk=neg;
	PekGrad[18].funk=integrate;
	PekGrad[19].funk=floor;
	PekGrad[20].funk=ceil;
	Pek[0].funk=fabs;
	Pek[1].funk=acos;
	Pek[2].funk=asin;
	Pek[3].funk=atan;
	Pek[4].funk=cos;
	Pek[5].funk=cosh;
	Pek[6].funk=exp;
	Pek[7].funk=log;  /* ln  */
	Pek[8].funk=logd; /* log */
	Pek[9].funk=sin;
	Pek[10].funk=sinh;
	Pek[11].funk=sqrt;
	Pek[12].funk=tan;
	Pek[13].funk=tanh;
	Pek[14].funk=heav;
	Pek[15].funk=randomGrad;
	Pek[16].funk=fak;
	Pek[17].funk=neg;
	Pek[18].funk=integrate;
	Pek[19].funk=floor;
	Pek[20].funk=ceil;

	memset(uttryck, 0, sizeof(uttryck));
	tmp = (char *) Malloc(TECKEN);
	memset(tmp,0,sizeof(tmp));

	strcpy(uttryck,string);
	In_fel=FALSE;

	for(i=0;i<ANTAL_ENHETER;i++)	{
		variabel_storhet[i]=0;
		variabel_lista[i][0]=0;
	}

	for ( i=1 ; i<antal_konstanter+1 ; i++)	{
		strcpy(variabel_lista[i],konstant_lista[i]);
		variabel_storhet[i]=konstant_storhet[i];
	}

/*	strcpy(variabel_lista[1],"Pi"); /* Konstanterna l„ses nu in i constants.c */
	variabel_storhet[1]=M_PI;
	strcpy(variabel_lista[2],"C");
	variabel_storhet[2]=atof("299792500"); */

	if(uttryck[0]!=0)	{
		kolla_uttryck();
		if(! In_fel)	{
			nextcharpos=0;
			readchar();
			if(basen != NULL)
				delete_expr_ar(basen);
			readexpression_a(&basen);
		}
		else { /* Lika b„st att avbryta, annars h†ller han p† i evighet */
			n--;
			Mfree(tmp);
			return string;
		}
	}

	if(! In_fel){
		for(i=antal_konstanter;i<antal_variabler;i++)	{
			rsrc_gaddr( R_TREE, ENTER_VARIABLE, &variable );
			obspec = variable[VARIABLE_IS].ob_spec.tedinfo;
			strptr = obspec->te_ptext;
			strptr[0] = 0;
			obspec2 = variable[VARIABLE].ob_spec.tedinfo;
			strptr2 = obspec2->te_ptext;
			strptr2[0] = 0;
			strcpy(strptr2,variabel_lista[i]);
			exitbutn=do_dialog(variable);
			if( exitbutn==CANCEL_FUNCTION )	{
				n--;
				Mfree(tmp);
				return string;
			}
			variabel_storhet[i]=atof(preprocess(strptr));
		}

		evalue_expr_ar(basen);
		vaerde=basen->variabel;
	}

	if(!In_fel)	{
		if(hex)
			hex_it(vaerde, tmp);
		else if(bin)
			bin_it(vaerde, tmp);
		else
			gcvt( vaerde, 15, tmp );

		/* om & eller | p† raden som ska r„knas ut s† svara med true om !=0 annars false */
		if( (strstr(s[n],"&") != NULL) || (strstr(s[n],"|") != NULL) || (strstr(s[n],"~") != NULL) || (strstr(s[n],"Ü") != NULL))
		{
			if(vaerde==0)
				strcpy(string,"false");
			else
				strcpy(string,"true");
			Mfree(tmp);
			return string;
		}
		strcpy(string,tmp);
		Mfree(tmp);
		return string;
	}
	if( exitbutn==CANCEL_ERROR )	{ /* Annars h†ller den p† i evighet */
		n--;
		Mfree(tmp);
		return string;
	}
	else if (!In_fel)	{  /* Allt h„r p† slutet g„ller bara om det var fel n†nstans i n†t uttryck */
		Mfree(tmp);
		return calculate(string);
	}
	Mfree(tmp);
	return "NaN"; /* Borde kanske bara h„nda om division med noll? */
}

int  ok() {
	return ! In_fel;
}


double  value() {
	return vaerde;
}


int  finns_i_funklista(char *strng, int *j) {
	int i=0;
	int finns=0;

	finns=FALSE;
	*j=0;
	if(! In_fel) {
		for(i=0;i<ANTAL_FUNKTIONER;i++)
			if(! strcmp(funk_lista[i],strng)) {
				finns=TRUE;
				*j=i;
				break;
			}
	}
	return finns;
}

int  alpha(char ch) {
	return (ch<='Z' && ch>='A') || (ch<='z' && ch>='a');
}

int isHexNumber(char ch) {
	return (ch<='F' && ch>='A') || (ch<='f' && ch>='a') || (ch<='9' && ch>='0') || (ch=='x');
}

int isBinNumber(char ch) {
	return (ch=='0' || ch=='1' || ch=='b');
}

int isOctNumber(char ch) {
	return (ch<='7' && ch>='0' || ch=='o');
}

int  stor_alpha(char ch) {
	return ch<='Z' && ch>='A';
}

void kolla_uttryck(void) {
	char temp[MAX_TECKEN];
	int i=0,j=0,pos=0;
	i=0;
	In_fel=FALSE;

	do{
		while( (!alpha(uttryck[i]) || uttryck[i]=='x' || uttryck[i]=='b' || uttryck[i]=='o') && i<strlen(uttryck) )
		{
			if(i!=0 && uttryck[i]=='x')
			{
				if(uttryck[i-1]!='0')	/* det „r inte b”rjan p† ett hextal */
				{
					i--;
					break;	/* Ta det d† inte som ett tal utan avbryt inl„sn. av talet */
				}
				else
				{
					while( ( isHexNumber(uttryck[i]) || !alpha(uttryck[i]) ) && i<strlen(uttryck) )
						i++;	/* L„ser in resten av hex-talet */
					i--;
				}
			}
			if(i!=0 && uttryck[i]=='b')
			{
				if(uttryck[i-1]!='0')	/* det „r inte b”rjan p† ett bintal */
				{
					i--;
					break;	/* Ta det d† inte som ett tal utan avbryt inl„sn. av talet */
				}
				else
				{
					while( ( isBinNumber(uttryck[i]) || !alpha(uttryck[i]) ) && i<strlen(uttryck) )
						i++;	/* L„ser in resten av bin-talet */
					i--; /* Den ++:as sedan */
				}
			}
			if(i!=0 && uttryck[i]=='o')
			{
				if(uttryck[i-1]!='0')	/* det „r inte b”rjan p† ett octtal */
				{
					i--;
					break;	/* Ta det d† inte som ett tal utan avbryt inl„sn. av talet */
				}
				else
				{
					while( ( isOctNumber(uttryck[i]) || !alpha(uttryck[i]) ) && i<strlen(uttryck) )
						i++;	/* L„ser in resten av oct-talet */
					i--; /* Den ++:as sedan */
				}
			}
			if(i==0 && (uttryck[i]=='x' || uttryck[i]=='b' || uttryck[i]=='o'))
				break;

			i++;
		}
		j=0;
		memset(temp,0,sizeof(temp));
		while(alpha(uttryck[i]) && j<MAX_TECKEN && i<strlen(uttryck))
		{
			temp[j]=uttryck[i];
			i++;
			j++;
		}
		temp[j]=0;
		if(!finns_i_funklista(temp,&pos) && temp[0]!=0 && ! stor_alpha(temp[0]))
		{
			rsrc_gaddr( R_TREE, STANDARDFUNCTION, &standard );
			
			obspec=standard[FUNCTION].ob_spec.tedinfo;
			strptr= obspec->te_ptext;
			strcpy( strptr, temp );
			do_dialog(standard);

		/*	printf("%s is no standard function.\nAll own varibles must begin with a capital letter.\nOnly these standard functions are available:\nabs acos asin atan cos cosh exp\nlog logd sin sinh sqrt tan tanh heav\n",temp);*/
			In_fel=TRUE;
		}
		i++;
	}while(i<strlen(uttryck) && !In_fel);
}

void  readchar()
{
	do {
		nextchar=uttryck[nextcharpos];
		nextcharpos++;
	}while (nextchar==' ');
}

void reporterror()
{
	char *tmp=NULL;
	int extent[4];

	posi=nextcharpos-1;
	In_fel=TRUE;
	syntax_error=1;
	exitbutn=CANCEL_ERROR;	/* Annars h†ller den p† hur l„nge som helst */

	tmp = (char *) Malloc (strlen(errortmpstring));
	if (!tmp)       /* Om det inte finns tillr„ckligt med minne*/
	    return;
	memset(tmp, 0, sizeof(tmp));
	strcpy(tmp, errortmpstring);
	*(tmp+posi)=0;
	vqt_ex ( handle, tmp, extent );
	Mfree(tmp);
	
	bubble_help(XMIN + extent[2] - teckenbredd/4, YMAX +teckenhojd*n + teckenhojd/4 , -2); /* -2 signalerar att det „r fel */
}

void  delete_expr_ar(pExpression_a T)
{
	if(T->v_expr !=NULL)
		delete_expr_ar(T->v_expr);
	if(T->h_expr !=NULL)
		delete_expr_ar(T->h_expr);
	free(T);
}

void  evalue_expr_ar(pExpression_a T)
{
	if(! In_fel)
	{
		if(T->h_expr !=NULL)
		{
			if(T->v_expr !=NULL)
			evalue_expr_ar(T->v_expr);
			evalue_expr_ar(T->h_expr);
			switch(T->operation)
			{
				case  '^':
					T->variabel=(*ff)(T->h_expr->variabel , T->v_expr->variabel);
				break;
				case  '!':
					T->variabel=0 - (T->h_expr->variabel);
				break;
			   case  '+':
					T->variabel=T->v_expr->variabel + T->h_expr->variabel;
				break;
				case  '-':
					T->variabel=T->h_expr->variabel - T->v_expr->variabel;
				break;
			   case  '*':
			   case  'ú':
					T->variabel=T->h_expr->variabel * T->v_expr->variabel;
				break;
				case  '/':
					if(T->v_expr->variabel==0)
					{
						rsrc_gaddr( R_TREE, DIV_BY_ZERO, &div_by_zero );
						do_dialog(div_by_zero);
						
						In_fel=TRUE;
					}
					else
						 T->variabel=T->h_expr->variabel / T->v_expr->variabel;
				break;
				case  '%':
					if(T->v_expr->variabel==0)
					{
						rsrc_gaddr( R_TREE, DIV_BY_ZERO, &div_by_zero );
						do_dialog(div_by_zero);
						
						In_fel=TRUE;
					}
					else
						T->variabel=fmod(T->h_expr->variabel, T->v_expr->variabel);
				break;
				case '&':
					if( (T->v_expr->variabel!=0) && (T->h_expr->variabel!=0) )
						T->variabel=1;
					else
						T->variabel=0;
				break;
				case '|':
					if( (T->v_expr->variabel!=0) || (T->h_expr->variabel!=0) )
						T->variabel=1;
					else
						T->variabel=0;
				break;
				case 'Ü': /* logisk om..s† */
					if( (T->v_expr->variabel==0) && (T->h_expr->variabel==1) )
						T->variabel=0;
					else
						T->variabel=1;
				break; 
			   default:
					if(rad)
						T->variabel=(*Pek[T->operation].funk)(T->h_expr->variabel);
					else
						T->variabel=(*PekGrad[T->operation].funk)(T->h_expr->variabel);
				break;
			}
		}
		else
		{
			if(T->var==0)
				T->variabel=T->data;
			else
				T->variabel=variabel_storhet[T->var];
		}
	}
}


int readfunk(int* funktion)
{
	char temp[MAX_TECKEN],i=0;
	int ok=FALSE,pos=0;

	if(! In_fel)
	{
		while(liten_alpha(nextchar))
		{
			temp[i]=nextchar;
			i++;
			readchar();
		}
		temp[i]=0;
		if(finns_i_funklista(temp,&pos))
		{
			ok=TRUE;
			*funktion=pos;
		}
		else
			reporterror();
		return ok;
	}
	return ok;
}


void  readnumber(double* numvalue)
{
	char temp[ANTAL_ENHETER],i=0;
	memset(temp,0,sizeof(temp));
	if(! In_fel)
	{
		if(hexNumber)
		{
			while(isHexNumber(nextchar))
			{
				temp[i]=nextchar;
				i++;
				readchar();
			}
		}
		else if(binNumber)
		{
			while(isBinNumber(nextchar))
			{
				temp[i]=nextchar;
				i++;
				readchar();
			}
		}
		else if(octNumber)
		{
			while(isOctNumber(nextchar))
			{
				temp[i]=nextchar;
				i++;
				readchar();
			}
		}
		else
		{
			while(isdigit(nextchar) || nextchar=='.')
			{
				temp[i]=nextchar;
				i++;
				readchar();
			}
		}
		temp[i]=0;
		if(strstr(temp,"x")) /* Om det „r ett hex-tal */
			*numvalue=strtoul(temp,NULL,16);
		else if(strstr(temp,"b")) /* Om det „r ett bin-tal */
			*numvalue=strtoul(temp+2,NULL,2);
		else if(strstr(temp,"o")) /* Om det „r ett oct-tal */
			*numvalue=strtoul(temp+2,NULL,8);
		else
			*numvalue=atof(temp);
	}
}

int  finns_i_variabellista(char *strng, int* j)
{
	int i=0;
	int finns=FALSE;

	*j=0;
	if(! In_fel)
	{
		for(i=0;i<ANTAL_ENHETER;i++)
		{
			if(! strcmp(variabel_lista[i],strng))
			{
				finns=TRUE;
				*j=i;
				break;
			}
		}
		return finns;
	}
	return finns;
}


void  laes_variabel(int* variabel)
{
	char temp[ANTAL_ENHETER],i=0;
	int index=0;
	if(! In_fel)
	{
		while(isalnum(nextchar) && i< MAX_TECKEN)
		{
			temp[i]=nextchar;
			i++;
			readchar();
		}
		temp[i]=0;
		if(finns_i_variabellista(temp,&index))
		{
			*variabel=index;
		}
		else
		{
			strcpy(variabel_lista[antal_variabler],temp);
			*variabel=antal_variabler;
			antal_variabler++;
		}
	}
}

void  read_tecken(pExpression_a* teckenvalue)
{
	double variabel_value=0;
	int variabel=0;
	pExpression_a next_expr,next_expr1;

	readchar();
	hexNumber=0;
	binNumber=0;
	octNumber=0;
	if(! In_fel)
	{
		if(isalnum(nextchar) || nextchar=='.' || nextchar=='(' || nextchar=='-')
		{
			switch(nextchar)
			{
				case '0':
					readchar();
					if(nextchar=='x')
						hexNumber=1; /* Talet b”rjar p† 0x och ska l„sas som hex */
					if(nextchar=='b')
						binNumber=1;
					if(nextchar=='o')
						octNumber=1;
					nextcharpos-=2;
					readchar();
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '.':
					next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
					next_expr->operation='!';
					next_expr->v_expr=NULL;
					readnumber(&variabel_value);
					next_expr1 = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
					next_expr->h_expr=next_expr1;
					next_expr1->data=variabel_value;
					next_expr1->var=0;
					next_expr1->v_expr=NULL;
					next_expr1->h_expr=NULL;
					*teckenvalue=next_expr;
				break;
				case '(':
				{
					next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
					next_expr->operation='!';
					next_expr->v_expr=NULL;
					readchar();
					readexpression_a( &(next_expr->h_expr));
					if(nextchar==')')
						readchar();
					else
					{
						reporterror();
						*teckenvalue=NULL;
					}
					*teckenvalue=next_expr;
				}
				break;
				default :
					if('A'<=nextchar && 'Z'>=nextchar)
					{
						next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
						next_expr->operation='!';
						next_expr->v_expr=NULL;
						next_expr1 = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
						next_expr->h_expr=next_expr1;
						laes_variabel(&variabel);
						next_expr1->var=variabel;
						next_expr1->v_expr=NULL;
						next_expr1->h_expr=NULL;
						*teckenvalue=next_expr;
					}
					else
					{
						if(liten_alpha(nextchar))
						{
							int funk;
							next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
							next_expr->operation='!';
							next_expr->v_expr=NULL;
							next_expr1 = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
							next_expr->h_expr=next_expr1;
							readfunk(&funk);
							next_expr1->operation=funk;
							next_expr1->v_expr=NULL;
							if(nextchar=='(')
								readchar();
							else
							{
								reporterror();
								*teckenvalue=NULL;
							}
							readexpression_a(&(next_expr1->h_expr));
							if(nextchar==')')
								readchar();
							else
							{
								reporterror();
								*teckenvalue=NULL;
							}
							*teckenvalue=next_expr;
						}
						else
						{
							next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
							next_expr->operation='!';
							next_expr->v_expr=NULL;
							readchar();
							readexpression_a( &(next_expr->h_expr));
							*teckenvalue=next_expr;
						}
					}
				break;
				}
			}
		else
		{
			reporterror();
			*teckenvalue=NULL;
		}
	}
}


void  readunaer(pExpression_a* unaervalue)
{
	double variabel_value=0;
	int variabel=0;
	hexNumber=0;
	binNumber=0;
	octNumber=0;
	if(! In_fel)
	{
		if(isalnum(nextchar) || nextchar=='.' || nextchar=='(' || nextchar=='-' || nextchar=='x')
		{
			switch(nextchar)
			{
				case '0':
					readchar();
					if(nextchar=='x')
						hexNumber=1; /* Talet b”rjar p† 0x och ska l„sas som hex */
					if(nextchar=='b')
						binNumber=1; /* Talet b”rjar p† 0b och ska l„sas som bin */
					if(nextchar=='o')
						octNumber=1; /* Talet b”rjar p† 0o och ska l„sas som oct */
					nextcharpos-=2;
					readchar();
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '.':
					*unaervalue = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
					readnumber(&variabel_value);
					(*unaervalue)->data=variabel_value;
					(*unaervalue)->var=0;
					(*unaervalue)->v_expr=NULL;
					(*unaervalue)->h_expr=NULL;
				break;
				case '(':
				{
					readchar();
					readexpression_a(unaervalue);
					if(nextchar==')')
						readchar();
					else
					{
						reporterror();
						*unaervalue=NULL;
					}
				}
				break;
				case '-':
					read_tecken(unaervalue);
				break;
				default :
					if('A'<=nextchar && 'Z'>=nextchar)
					{
						*unaervalue = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
						laes_variabel(&variabel);
						(*unaervalue)->var=variabel;
						(*unaervalue)->v_expr=NULL;
						(*unaervalue)->h_expr=NULL;
					}
				break;
			}
		}
		else
		{
			reporterror();
			*unaervalue=NULL;
		}
	}
}


int  liten_alpha(char ch)
{
	return ch<='z' && ch>='a';
}


void  readpotens_ar(pExpression_a* value)
{
	pExpression_a next_expr;
	int funk=0;
	if(! In_fel)
	{
		if( liten_alpha(nextchar) )
		{
			readfunk(&funk);
			next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
			next_expr->operation=funk;
			next_expr->v_expr=NULL;
			if(nextchar=='(')
				readchar();
			else
			{
				reporterror();
				value=NULL;
			}
			readexpression_a(&(next_expr->h_expr));
			if(nextchar==')')
				readchar();
			else
			{
				reporterror();
				*value=NULL;
			}
			*value=next_expr;
		}
		else
			readunaer(value);
	}
}


void  readfactor_ar(pExpression_a* termvalue)
{
	pExpression_a next_expr;

	readpotens_ar(termvalue);
	if(! In_fel)
	{
		while(nextchar=='^')
		{
			next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
			next_expr->operation=nextchar;
			next_expr->h_expr=*termvalue;
			readchar();
			readpotens_ar(&(next_expr->v_expr));
			*termvalue=next_expr;
		}
	}
}

void  readterm_ar(pExpression_a* termvalue)
{
	pExpression_a next_expr;

	readfactor_ar(termvalue);
	if(! In_fel)
	{
		while(nextchar=='*' || nextchar=='ú' || nextchar=='/' || nextchar=='%' || nextchar=='&' || nextchar=='|' || nextchar=='Ü')
		{
			next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
			next_expr->operation=nextchar;
			next_expr->h_expr=*termvalue;
			readchar();
			readfactor_ar(&(next_expr->v_expr));
			*termvalue=next_expr;
		}
	}
}


void  readexpression_a(pExpression_a* exprvalue)
{
	pExpression_a next_expr;

	readterm_ar(exprvalue);
	if(! In_fel)
	{
		while(nextchar=='+' || nextchar=='-')
		{
			next_expr = (struct expression_a *) calloc(sizeof(struct expression_a), 1);
			next_expr->operation=nextchar;
			next_expr->h_expr=*exprvalue;
			readchar();
			readterm_ar(&(next_expr->v_expr));
			*exprvalue=next_expr;
		}
	}
}