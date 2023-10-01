#include <string.h>
#include <stdio.h>
#include <string.h>
#include "xtras.h"
#include <portab.h>

extern int MagX, MiNT;

int bad_language( char *string )
{
	char* tmp=NULL;

	tmp = (BYTE *) Malloc (strlen(string));

	strcpy(tmp,string);
	tmp=strlwr(tmp);
	
	if( strstr(tmp, "fuck") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "dick") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "pussy") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "shit") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "kuk") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "fitta") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "helvete") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "kut") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "lul") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "reet") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "merde") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "putain") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "bordel") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "chier") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "bite") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "couille") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "cul") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "pute") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "con") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "perkele") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "scheisse") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "mist") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "kacke") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "arschloch") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "tonto") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "puta") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "satan") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "fanden") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "helvede") != NULL ){
		Mfree(tmp); return 1;}
	if( strstr(tmp, "microsoft") != NULL ){
		Mfree(tmp); return 1;}

	Mfree(tmp);
	return 0;
}