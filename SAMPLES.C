/* Samples.c
 * Plays sound samples for different events */

#include <aes.h>
#include <string.h>
#include <stdio.h>
#include <tos.h>
#include <string.h>
#include "newton.h"
#include "xtras.h"

extern OBJECT *samples_tree;

extern int do_dialog(OBJECT *dialog);   /* Run a dialog with the user */
extern int MagX, MiNT;

char *file_startup, *path_startup;
char *strptr_startup,*tmp_startup;  
TEDINFO *obspec_startup;

char *file_trashcan, *path_trashcan;
char *strptr_trashcan,*tmp_trashcan;
TEDINFO *obspec_trashcan;

char *file_language, *path_language;
char *strptr_language,*tmp_language;
TEDINFO *obspec_language;

char *file_quit, *path_quit;
char *strptr_quit,*tmp_quit;
TEDINFO *obspec_quit;

char *korta_filnamn(char *text); /* Shortens the filename to fit in the window */

void choose_samples(void)
{
	int val=0,i=0;
	char *tmp,*startTrash,*trashTrash,*langTrash,*quitTrash;
	int exitbutn=0;
	int done=0;

	tmp= (char *) Malloc (256);
	startTrash= (char *) Malloc (256);
	trashTrash= (char *) Malloc (256);
	langTrash= (char *) Malloc (256);
	quitTrash= (char *) Malloc (256);
	
	if(path_startup[0]==0)
		strcpy(path_startup, "c:\\*.wav" );
	if(path_trashcan[0]==0)
		strcpy(path_trashcan, "c:\\*.wav" );
	if(path_language[0]==0)
		strcpy(path_language, "c:\\*.wav" );
	if(path_quit[0]==0)
		strcpy(path_quit, "c:\\*.wav" );

	strcpy(tmp,tmp_startup);
	strcpy(startTrash,tmp_startup); /* The path  */
	if(tmp_startup[0]!=0)    /* Sets the dialog so that all filenames are correct */
	{
		for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* The file name */
		strcpy(file_startup,(tmp+i+1));
		
		strcpy(tmp,tmp_startup);
		tmp=korta_filnamn(tmp); /* What is shown in the dialog */
		strcpy(strptr_startup,tmp);
		
		for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* Removes wildcard */
		tmp[i+1]=0;
		strcat(tmp,"*.wav");
		strcpy(path_startup,tmp);
	}

	strcpy(tmp,tmp_trashcan);
	strcpy(trashTrash,tmp_trashcan); /* The path  */
	if(tmp_trashcan[0]!=0)    /* Sets the dialog so that all filenames are correct */
	{
		for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* The file name */
		strcpy(file_trashcan,(tmp+i+1));
		
		strcpy(tmp,tmp_trashcan);
		tmp=korta_filnamn(tmp); /* What is shown in the dialog */
		strcpy(strptr_trashcan,tmp);
		
		strcpy(tmp,tmp_trashcan);
		for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* Removes wildcard */
		tmp[i+1]=0;
		strcat(tmp,"*.wav");
		strcpy(path_trashcan,tmp);
	}
	strcpy(tmp,tmp_language);
	strcpy(langTrash,tmp_language); /* The path  */
	if(tmp_language[0]!=0)    /* Sets the dialog so that all filenames are correct */
	{
		for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* The file name */
		strcpy(file_language,(tmp+i+1));
		
		strcpy(tmp,tmp_language);
		tmp=korta_filnamn(tmp); /* What is shown in the dialog */
		strcpy(strptr_language,tmp);

		strcpy(tmp,tmp_language);
		for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* Removes wildcard */
		tmp[i+1]=0;
		strcat(tmp,"*.wav");
		strcpy(path_language,tmp);
	}
	strcpy(tmp,tmp_quit);
	strcpy(quitTrash,tmp_quit); /* The path  */
	if(tmp_quit[0]!=0)    /* Sets the dialog so that all filenames are correct */
	{
		for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* The file name */
		strcpy(file_quit,(tmp+i+1));
		
		strcpy(tmp,tmp_quit);
		tmp=korta_filnamn(tmp); /* What is shown in the dialog */
		strcpy(strptr_quit,tmp);
		
		for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* Removes wildcard */
		tmp[i+1]=0;
		strcat(tmp,"*.wav");
		strcpy(path_quit,tmp);
	}

	do
	{		
		exitbutn=do_dialog(samples_tree);

		if(exitbutn==STARTUPSOUND){
			wind_update(BEG_MCTRL);
			fsel_exinput( path_startup, file_startup, &val, "Choose Startup sound" );
			wind_update(END_MCTRL);
			if(val==1)
			{
				strcpy(tmp,path_startup);

				if(file_startup[0]!=0)    /* If no file was selected, but still OK */
				{
					for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* Removes wildcard */
					tmp[i+1]=0;
					strcat(tmp,file_startup);
				}
				strcpy(tmp_startup,tmp); /* tmp_startup shows the full path if cancel is then selected */
				tmp=korta_filnamn(tmp);
				strcpy(strptr_startup,tmp);
			}
		}
		if(exitbutn==TRASHCANSOUND){
			wind_update(BEG_MCTRL);
			fsel_exinput( path_trashcan, file_trashcan, &val, "Choose Trashcan sound" );
			wind_update(END_MCTRL);
			if(val==1)
			{
				strcpy(tmp,path_trashcan);

				if(file_trashcan[0]!=0)    /* If no file was selected, but still OK */
				{
					for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* Removes wildcard */
					tmp[i+1]=0;
					strcat(tmp,file_trashcan);
				}
				strcpy(tmp_trashcan,tmp);
				tmp=korta_filnamn(tmp);
				strcpy(strptr_trashcan,tmp);
			}
		}
		if(exitbutn==LANGUAGESOUND){
			wind_update(BEG_MCTRL);
			fsel_exinput( path_language, file_language, &val, "Choose Bad Language Error sound" );
			wind_update(END_MCTRL);
			if(val==1)
			{
				strcpy(tmp,path_language);

				if(file_language[0]!=0)    /* If no file was selected, but still OK */
				{
					for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* Removes wildcard */
					tmp[i+1]=0;
					strcat(tmp,file_language);
				}
				strcpy(tmp_language,tmp);
				tmp=korta_filnamn(tmp);
				strcpy(strptr_language,tmp);				
			}
		}
		if(exitbutn==QUITSOUND){
			wind_update(BEG_MCTRL);
			fsel_exinput( path_quit, file_quit, &val, "Choose Quit sound" );
			wind_update(END_MCTRL);
			if(val==1)
			{
				strcpy(tmp,path_quit);

				if(file_language[0]!=0)    /* If no file was selected, but still OK */
				{
					for (i=(int)(long)strlen(tmp); tmp[i]!='\\' ; i--); /* Removes wildcard */
					tmp[i+1]=0;
					strcat(tmp,file_quit);
				}
				strcpy(tmp_quit,tmp);
				tmp=korta_filnamn(tmp);
				strcpy(strptr_quit,tmp);				
			}
		}
		
		if(exitbutn==SOUNDOK || exitbutn==SOUNDCANCEL)
			done=1;
		if(exitbutn==SOUNDCANCEL)
		{
			strcpy(tmp_startup,startTrash);
			strcpy(tmp_trashcan,trashTrash);
			strcpy(tmp_language,langTrash);
			strcpy(tmp_quit,quitTrash);
		}
		if(exitbutn==SOUNDOK)
		{
			if( strcmp(strptr_startup,"")==0 )
				strcpy(tmp_startup,"");
			if( strcmp(strptr_trashcan,"")==0 )
				strcpy(tmp_trashcan,"");
			if( strcmp(strptr_language,"")==0 )
				strcpy(tmp_language,"");
			if( strcmp(strptr_quit,"")==0 )
				strcpy(tmp_quit,"");
		}

	}while(!done);
	
	Mfree(tmp);Mfree(startTrash);Mfree(trashTrash);Mfree(langTrash);Mfree(quitTrash);
	return;
}

char *korta_filnamn(char *text)
{
	char *temp=NULL;
	int i=0;

	temp = (char *) Malloc (256);
	memset(temp, 0, sizeof(temp));

	if( (strlen(text)>=29) )
	{
		strrev(text);

		for (i=0; i<29 || text[i]==0 ; i++ ) /* Copies max 29 characters */
			temp[i]=text[i];
		temp[i]=0;

		strcat(temp,"...");                   /* Here are the last three.. */
		strrev(temp);
		strcpy(text,temp);
		Mfree(temp);
		return text;
	}
	else{
		Mfree(temp);
		return text;
	}
}

void initSampleDialogue(void) /* This is run at the start of the program */
{
	strptr_startup = (char *) Malloc (256);
	file_startup = (char *) Malloc (256);
	tmp_startup = (char *) Malloc (256);
	path_startup= (char *) Malloc (256);

	memset(file_startup, 0, sizeof(file_startup));
	memset(tmp_startup, 0, sizeof(tmp_startup));
	memset(strptr_startup, 0, sizeof(strptr_startup));
	memset(path_startup, 0, sizeof(path_startup));

	obspec_startup = samples_tree[STARTUPSOUND].ob_spec.tedinfo;
	strptr_startup = obspec_startup->te_ptext;

	strptr_startup[0] = 0;

	strptr_trashcan = (char *) Malloc (256);
	file_trashcan = (char *) Malloc (256);
	tmp_trashcan = (char *) Malloc (256);
	path_trashcan= (char *) Malloc (256);
	
	memset(strptr_trashcan, 0, sizeof(strptr_trashcan));
	memset(file_trashcan, 0, sizeof(file_trashcan));
	memset(tmp_trashcan, 0, sizeof(tmp_trashcan));
	memset(path_trashcan, 0, sizeof(path_trashcan));

	obspec_trashcan = samples_tree[TRASHCANSOUND].ob_spec.tedinfo;
	strptr_trashcan = obspec_trashcan->te_ptext;

	strptr_trashcan[0] = 0;
	
	strptr_language = (char *) Malloc (256);
	file_language = (char *) Malloc (256);
	tmp_language = (char *) Malloc (256);
	path_language= (char *) Malloc (256);

	memset(strptr_language, 0, sizeof(strptr_language));
	memset(file_language, 0, sizeof(file_language));
	memset(tmp_language, 0, sizeof(tmp_language));
	memset(path_language, 0, sizeof(path_language));

	obspec_language = samples_tree[LANGUAGESOUND].ob_spec.tedinfo;
	strptr_language = obspec_language->te_ptext;

	strptr_language[0] = 0;

	strptr_quit = (char *) Malloc (256);
	file_quit = (char *) Malloc (256);
	tmp_quit = (char *) Malloc (256);
	path_quit= (char *) Malloc (256);

	memset(strptr_quit, 0, sizeof(strptr_quit));
	memset(file_quit, 0, sizeof(file_quit));
	memset(tmp_quit, 0, sizeof(tmp_quit));
	memset(path_quit, 0, sizeof(path_quit));

	obspec_quit = samples_tree[QUITSOUND].ob_spec.tedinfo;
	strptr_quit = obspec_quit->te_ptext;

	strptr_quit[0] = 0;
}
