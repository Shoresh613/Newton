

/* char* hex_it(int value), char* bin_it(int value)
 * Converts a decimal number to a hex/binary number
 ************************************************/


#include <aes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "xtras.h"

void hex_it(long value, char* result){
	int i=0;
	char *hex=NULL, *hex2=NULL;
	int minus=0;
	
	if(value==0){
		strcpy(result,"0x0");
		return;
	}

	if(value<0){  /* If negative, quarrel a bit */
		value=labs( value );
		minus=1;
	}
	hex=(char*)Malloc(TECKEN);
	memset(hex,0,sizeof(hex));

	while(value>0) {
		if((value%16)>9)
			hex[i]=(value%16-10+'A');
		else
			hex[i]=(value%16+'0');
		value=value/16;
		i++;
	}

	hex[i]=0;
	hex2=(char*)Malloc(TECKEN);
	memset(hex2,0,sizeof(hex2));
	strcpy(hex2,strrev(hex));

	if(minus){
		strcpy(result,"-");
		strcat(result,"0x");
		strcat(result,hex2);
		Mfree(hex);Mfree(hex2);
		return;
	}
	strcpy(result,"0x");
	strcat(result,hex2);
	Mfree(hex);Mfree(hex2);
	return;
}

void bin_it(long value, char* result)
{
	int i=0;
	char *bin=NULL, *bin2=NULL;
	int minus=0;

	if(value==0){
		strcpy(result,"0b0");
		return;
	}
	if(value<0){  /* If negative, quarrel a bit */
		value=labs( value );
		minus=1;
	}

	bin=(char*)Malloc(TECKEN);
	memset(bin,0,sizeof(bin));

	while(value>0) {
		bin[i]=(value%2+'0');
		value=value/2;
		i++;
	}

	bin[i]=0;
	bin2=(char*)Malloc(TECKEN);
	memset(bin2,0,sizeof(bin2));
	strcpy(bin2,strrev(bin));

	if(minus) {
		strcpy(result,"-");
		strcat(result,"0b");
		strcat(result,bin2);
		Mfree(bin);Mfree(bin2);
		return;
	}
	strcpy(result,"0b");
	strcat(result,bin2);
	Mfree(bin);Mfree(bin2);
	return;
}