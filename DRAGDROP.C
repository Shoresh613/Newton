#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include <aes.h>
#include <vdi.h>
#include <nkcc.h>
#include <string.h>
#include <portab.h>
#include <dragdrop.h>
#include <vaproto.h>
#include "newton.h"
#include "xtras.h"


extern int msg[8],n, cursor, synliga_rader, teckenhojd;
extern int play_sample, boxes;
extern int appl_id, handle;
extern char s[RADER][TECKEN];
extern int scrollVector[RADER];
extern OBJECT *tree;
extern char *tmp_trashcan;
extern int MagX, MiNT;
extern int posi;

extern GS_INFO gs_info;
extern GS_PARTNER *gs_partner[8];

extern void scrap_write(int i);
extern void redraw_window( int all );
extern void insert_dragged_text(int my, int kstate, char *text);
extern void error_sound( int sound );
extern void insert(char* string, char* toBinserted);
extern void (*vqt_ex)(int handle, char* string, int* extent);
extern void eraseExpression(void);

int timeout, wait_for_thatizit;


void send_dragndrop(int mx, int my, int kstate)
{
	int dd_handle; /* Filen (pipen) allt skrivs i */
	ULONG format[8], Format1='.ASC', Format2='.TXT';
	WORD dd_partner, id, dummy, ok;
	char *name=NULL, *tmp=NULL;
	char *test=NULL;
	char *AV_Timeout=NULL;
	int extent[8], j=0, i=0;
	long *oldpipesig=NULL; /* Beh�ver kanske vara global */
	int scrollStart=n,scrollEnd=0;

	id=wind_find(mx,my); /* Vilket f�nster ligger under musen */
	wind_get(id,WF_OWNER,&dd_partner,&dummy,&dummy,&dummy); /* Vem �ger f�nstret */

	if(cursor)
		redraw_window(3);

	if(dd_partner==appl_id) /* Om draget till det egna f�nstret */
	{
		if(objc_find( tree, MAIN, MAX_DEPTH, mx, my )==RUTAN)
		{
			kstate=nkc_kstate(); /* Annars tas det som det var innan knappen blev intryckt */

			tmp = Malloc(TECKEN*synliga_rader);	/* Anv�nder h�r f�r att bygga upp flerradigt uttryck */
			memset(tmp,0,sizeof(tmp));
	
			i=n;
			if(i>0) {
				while(scrollVector[i-1] && i>0){	/* Tar reda p� var uttrycket b�rjar */
					i--;
				}
			}
			scrollStart=i;
			strcpy(tmp,s[i]);
			while(scrollVector[i]){ /* Kopiera in alla rader som h�r till uttrycket i bufferten. */
				strcat(tmp,"\r\n");	/* B�rja med ny rad. */
				strcat(tmp, s[i+1] );
				i++;
			}
			scrollEnd=i;
			if( (kstate&NKF_CTRL) ) /* Raden skall flyttas */
				while(scrollStart<=scrollEnd){
					memset(s[scrollStart],0,TECKEN);
					scrollVector[scrollStart]=0;
					scrollStart++;
				}
			insert_dragged_text(my,kstate,tmp);
			Mfree(tmp);
			redraw_window(2); /* Rita om textrutan */
		}
	}
	else if( dd_partner == 0 /*appl_find("JINNEE  ") ska ju funka med vilken AV-SERVER desktop som helst, alltid appl_id==0 */ )
	{
		/* Kolla f�rst om det �r bakgrunden */
		msg[0]=AV_WHAT_IZIT;
		msg[1]=appl_id;
		msg[2]=0;
		msg[3]=mx;
		msg[4]=my;
		msg[5]=0;
		msg[6]=0;
		msg[7]=0;

		if( appl_write( dd_partner, 16, msg ) == 0 ) /* Om det inte funkade */
			return;                                   /* G�r ingenting.      */

		timeout           = 0;
		wait_for_thatizit = 1;

		do{
			evnt_mesag(msg);
			if(msg[0]==VA_THAT_IZIT)
				wait_for_thatizit=0;
			if(timeout++ > 50)
			{
				rsrc_gaddr( R_STRING, VAPROTO, &AV_Timeout );
				form_alert(1,AV_Timeout);
				return;
			}
		}while(wait_for_thatizit);

		if(msg[0]==VA_THAT_IZIT)
		{
			if(msg[4] == VA_OB_TRASHCAN || msg[4] == VA_OB_SHREDDER ) /* Trash och Clipboard �r antagligen VA-protokollet */
			{
				if( play_sample )
					error_sound(ERASE_DD_SOUND);

				if(boxes) /* Om growboxar skall ritas ut. Inte helt r�tt vid flerradiga uttryck, men det ser v�l ingen? */
				{
					vqt_ex ( handle, s[n], extent );
					graf_growbox(tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +2,tree[MAIN].ob_y + (n*teckenhojd)+tree[RUTAN].ob_y + teckenhojd/4, extent[2]-extent[0], teckenhojd, mx, my, 20,10);
				}
				eraseExpression();
				redraw_window( 2 );
				return; }

			if(msg[4] == VA_OB_CLIPBOARD)
			{
				if(boxes)
				{
					vqt_ex ( handle, s[n], extent );
					graf_growbox(tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +2,tree[MAIN].ob_y + (n*teckenhojd)+tree[RUTAN].ob_y + teckenhojd/4, extent[2]-extent[0], teckenhojd, mx, my, 20,10);
				}
				kstate=nkc_kstate();
				if(kstate&NKF_CTRL)
					scrap_write(1);
				else
					scrap_write(0);
				redraw_window( 2 );
				return;
			}
		}

		/* Om draget till jinnee och det inte var n�gon papperskorg eller s�, g�r det via GEMScript */
		if( dd_partner == appl_find("JINNEE  ")){
			for (j = 0; gs_partner[j]->gs_partnerid!=-1 && j<7 ; j++ );
			gs_partner[j]->gs_partnerid=dd_partner;
			gs_partner[j]->gs_id=NOTEOPEN;

			msg[0]=GS_REQUEST;
			msg[1]=appl_id;
			msg[2]=0;
			msg[3]=(int)(((long) &gs_info >> 16) & 0x0000ffff);
			msg[4]=(int)((long) &gs_info & 0x0000ffff);
			msg[5]=0;
			msg[6]=0;
			msg[7]=NOTEOPEN;

			if( appl_write( dd_partner, 16, msg ) == 0 ) /* Om det inte funkade */
				return;                                   /* G�r ingenting.      */
		 /* Forts�ttning f�ljer i message.c: GS_REPLY */
		}
	}

	else /* Om draget till n�gon annan applikation */
	{
		dd_handle = ddcreate( appl_id, dd_partner, id, mx, my, kstate, format, (void**)&oldpipesig );

		if(dd_handle==-1 || dd_handle==-2)
			return;

		if (MiNT || MagX)
		{	test = (BYTE *) Mxalloc (32, 0 | MGLOBAL);
		    name = (BYTE *) Mxalloc (12, 0 | MGLOBAL); /* De m�ste ju vara tillr�ckligt l�nga! v�l? */
		}
		else
		{	test = (BYTE *) Malloc (32);
			name = (BYTE *) Malloc (12);
		}

		tmp = Malloc(TECKEN*synliga_rader);	/* Anv�nder h�r f�r att bygga upp flerradigt uttryck */
		memset(tmp,0,sizeof(tmp));

		i=n;
		if(i>0) {
			while(scrollVector[i-1] && i>0){	/* Tar reda p� var uttrycket b�rjar */
				i--;
			}
		}
		scrollStart=i;
		strcpy(tmp,s[i]);
		while(scrollVector[i]){ /* Kopiera in alla rader som h�r till uttrycket i bufferten. */
			strcat(tmp,"\n");	/* B�rja med ny rad. */
			strcat(tmp, s[i+1] );
			i++;
		}
		scrollEnd=i;

		test = (char*)(unsigned long)(&format[0]);

		if( strstr(test,".TXT")!=NULL)
		{	Format1='.TXT'; Format2='.ASC'; }
		else if( strstr(test,".ASC")!=NULL )
		{	Format1='.ASC'; Format2='.TXT'; }

		strcpy( name, "ASCII Text" );
		ok=ddstry( dd_handle, Format1, name, (long)strlen(tmp) );

		if(ok == DD_EXT)
			ok=ddstry( dd_handle, Format2, name, (long)strlen(tmp) );

		if(ok == DD_NAK || ok == DD_EXT || ok == DD_LEN )
		{	ddclose( dd_handle, oldpipesig );
			return; } /* Programmet vill inte D&D */

		if(ok == DD_OK)
		{
			Fwrite(dd_handle,strlen(tmp),tmp);
			Mfree(tmp);
			ddclose( dd_handle, oldpipesig );
			kstate=nkc_kstate();
			if(kstate&NKF_CTRL) {	/* Raden skall flyttas */
				while(scrollStart<=scrollEnd){
					memset(s[scrollStart],0,TECKEN);
					scrollVector[scrollStart]=0;
					scrollStart++;
				}
				
			/*	memset(s[n],0,TECKEN);  */
				redraw_window(2);
			}
		}
	}
	return;
}

void insert_dragged_text(int my, int kstate, char *text)
{
	int i=0, rowCount=1;
	char *tmp=NULL, *tmp2=NULL;

	tmp=(char*)Malloc(TECKEN);
	tmp2=(char*)Malloc(TECKEN);
	memset(tmp,0,TECKEN);
	memset(tmp2,0,TECKEN);

	for ( n=-1 ; n<synliga_rader && !i ; n++ ) /* Vilken rad �r man p�? */
		if( my < tree[MAIN].ob_y + (n*teckenhojd)+tree[RUTAN].ob_y+teckenhojd+6)
			i=1;
	n--; /* n b�rjar p� 0, inte 1 som synliga_rader */

	if(text[0]=='#') /* Notislappar fr�n jinnee skickar med �ven uppbyggnadsinfo om man h�llit inne alt */
		return;

	if(cursor)
		redraw_window(3); /* Sl�cker mark�ren */

	posi=(int)strlen(s[n]);	/* S�tt in p� slutet av raden. Fel, men vad� */

	if(kstate&NKF_ALT && (s[n][0]!=0)) /* Om alt, och raden inte tom, l�gg till */
		insert( s[n], text);
	else
	{
		if(strstr(text,"\r\n")){	/* Om flera rader */
			strcpy(tmp,text);
			while(strstr(tmp,"\r\n")!=NULL){
				strcpy(tmp2,(strstr(tmp,"\r\n")+2) ); /* Kopierar in det efter nyrad-tecknet till tmp2 */
				*(tmp + strlen(tmp)-strlen(strstr(tmp,"\r\n")))='\0'; /* Klipper tmp vid radslutet */
				strcpy(s[n],tmp);			/* S�tter in raden. */
				strcpy(tmp,tmp2);			/* Kopierar in det resterande i tmp */
				n++;
				rowCount++;
			}
			strcpy(s[n],tmp);			/* S�tter in sista raden. */

			while(rowCount>1) {	/* S�ger vilka rader som sitter ihop */
				scrollVector[n-rowCount+1]=1;
				rowCount--;
			}
		}
		else
			strcpy(s[n],text); /* Annars ers�tt. */
		posi=(int)strlen(s[n]);
	}
	Mfree(tmp);Mfree(tmp2);
}

void receive_dragndrop(void)
{
	char *pipename;
	char extname[3];
	char *buffert;
	char *ptr;
	char dd_msg;
	char header_msg[32]=".TXT.ASC000000000000000000000000";
	int message_header_size;
	char* rec_head_msg; /* Mottagna headern */
	int file; /* Filen (pipen) allt skrivs i */
	long datasize;
	int mx, my, kstate;
	long *lpref_datatypes, *lptr;

	mx=msg[4];
	my=msg[5];
	kstate=nkc_kstate();

	if(objc_find( tree, MAIN, MAX_DEPTH, mx, my )==RUTAN)
	{
		if (MiNT || MagX)
		{
			buffert=(char*) Mxalloc(RADER*TECKEN,0|MGLOBAL);
			pipename=(char*) Mxalloc(21,0|MGLOBAL);
		}
		else
		{
			buffert=(char*) Malloc(RADER*TECKEN);
			pipename=(char*) Malloc(21);
		}
		if(extname==NULL)
			return;
		memset(buffert, 0, sizeof(buffert));

		strcpy(pipename,"U:\\PIPE\\DRAGDROP.");
		ptr = (char*)(&msg[7]); /* Tar reda p� extensionen */
		extname[0] = *(ptr);
		extname[1] = *(ptr+1);
		extname[2] = 0;
		strcat(pipename,(char*)extname);

		if( (file=(int)(long)Fopen(pipename,O_RDWR|O_DENYNONE)) <0 )
			return;

		dd_msg=DD_OK;
		if( Fwrite(file,1,(void*)(&dd_msg)) <1 ) /* Det �r ok att skicka */
			return;
		if( Fwrite(file,32,(void*)header_msg) <32 ) /* Skriver vilka filtyper som kan l�sas */
			return;
		if( Fread(file,2,(void*)&message_header_size) <2 ) /* L�ser storlek p� headern */
			return;
		if (MiNT || MagX)
			rec_head_msg=(char*) Mxalloc(message_header_size,0|MGLOBAL); /* Reserverar minne f�r headern */
		else
			rec_head_msg=(char*) Malloc(message_header_size); /* Reserverar minne f�r headern */

		if( Fread(file,message_header_size,(void*)rec_head_msg) <message_header_size )  /* L�ser headern */
		{
			Mfree(rec_head_msg);
			return;
		}

		lpref_datatypes = (long*)rec_head_msg;
		if( (*(lpref_datatypes) == '.TXT') || (*(lpref_datatypes) == '.ASC')) /* Om han t�nker skicka text */
		{
			dd_msg=DD_OK;
			if( Fwrite(file,1,(void*)(&dd_msg)) <1)
			{
				Mfree(rec_head_msg);
				return;
			}
			lptr = (long*)(rec_head_msg); /* Kollar hur mycket som skall l�sas */
			datasize = *(lptr+1);
			if(datasize<TECKEN)
			{
				if( Fread(file,datasize,buffert) <datasize)
				{
					Mfree(rec_head_msg);
					return;
				}
				if(buffert[datasize-1]=='\n') /* Om nyrad-tecken skickats med */
					buffert[datasize-2]=0; /* S�tter den avslutande nollan, annars blir det en massa bj�fs ocks�. -2 f�r de tv� avslutande nyrads-tecknen */
				else
					buffert[datasize]=0; /* S�tter den avslutande nollan, annars blir det en massa bj�fs ocks�. */

				insert_dragged_text(my,kstate,buffert);
			}
		}
		else
		{
			dd_msg=DD_EXT;
			Fwrite(file,1,(void*)(&dd_msg)); /* Efter detta skall man egentligen v�nta p� en ny header och kolla den ocks�, men det f�r bli en anna dag.. */
		}
		Fclose(file);
		Mfree(rec_head_msg);Mfree(buffert);Mfree(pipename);
	}
}

/* H�r nedan �r lite standardbj�fs fr�n MagiC arkivet som jag f�tt
   patcha lite f�r att f� PC att kompilera ordentligt */

/*----------------------------------------------------------------------------------------*/
/* Drag & Drop - Pipe �ffnen (f�r den Sender)															*/
/* Funktionsresultat:	Handle der Pipe, -1 f�r Fehler oder -2 f�r Fehler bei appl_write	*/
/*	app_id:					ID des Senders (der eigenen Applikation)									*/
/*	rcvr_id:					ID des Empf�ngers																	*/
/*	window:					Handle des Empf�nger-Fensters													*/
/*	mx:						x-Koordinate der Maus beim Loslassen oder -1								*/
/*	my:						y-Koordinate der Maus beim Loslassen oder -1								*/
/*	kbstate:					Status der Kontrolltasten														*/
/*	format:					Feld f�r die max. 8 vom Empf�nger unterst�tzten Formate				*/
/*	oldpipesig:				Zeiger auf den alten Signal-Dispatcher										*/
/*----------------------------------------------------------------------------------------*/
WORD	ddcreate( WORD	app_id, WORD rcvr_id, WORD window, WORD mx, WORD my, WORD kbstate, ULONG format[8], void **oldpipesig )
{
	BYTE	pipe[24];
	BYTE	reply;
	WORD	mbuf[8];
	LONG	handle_mask;
	WORD	dd_handle, i;
	BYTE	*Ddprotocoll=NULL;

	strcpy( pipe, "U:\\PIPE\\DRAGDROP.AA" );
	pipe[18] = 'A' - 1;

	do
	{
		pipe[18]++;															/* letzten Buchstaben weitersetzen */
		if ( pipe[18] > 'Z' )											/* kein Buchstabe des Alphabets? */
		{
			pipe[17]++;														/* ersten Buchstaben der Extension �ndern */
			if ( pipe[17] > 'Z' )										/* lie� sich keine Pipe �ffnen? */
				return( -1 );
		}
		dd_handle = (WORD) Fcreate( pipe, 0x02 );						/* Pipe anlegen, 0x02 bedeutet, da� EOF zur�ckgeliefert wird, */
	} while ( dd_handle == EACCDN );

	if ( dd_handle < 0 )														/* lie� sich die Pipe nicht anlegen? */
		return( dd_handle );

	mbuf[0] = AP_DRAGDROP;												/* Drap&Drop-Message senden */
	mbuf[1] = app_id;														/* ID der eigenen Applikation */
	mbuf[2] = 0;
	mbuf[3] = window;														/* Handle des Fensters */
	mbuf[4] = mx;															/* x-Koordinate der Maus */
	mbuf[5] = my;															/* y-Koordinate der Maus */
	mbuf[6] = kbstate;													/* Tastatur-Status */
	mbuf[7] = (((WORD) pipe[17]) << 8 ) + pipe[18];				/* Endung des Pipe-Namens */

	if ( appl_write( rcvr_id, 16, mbuf ) == 0 )					/* Fehler bei appl_write()? */
	{
		Fclose( dd_handle );													/* Pipe schlie�en */
		return( -2 );
	}

	handle_mask = 1L << dd_handle;
	i = Fselect( DD_TIMEOUT, &handle_mask, 0L, 0L );			/* auf Antwort warten */

	if ( i && handle_mask )												/* kein Timeout? */
	{
		if ( Fread( dd_handle, 1L, &reply ) == 1 )					/* Antwort vom Empf�nger lesen */
		{
			if ( reply == DD_OK )										/* alles in Ordnung? */
			{
				if ( Fread( dd_handle, DD_EXTSIZE, format ) == DD_EXTSIZE )	/* unterst�tzte Formate lesen */
				{
					*oldpipesig = Psignal( SIGPIPE, (void *) SIG_IGN );	/* Dispatcher ausklinken */
					return( dd_handle );
				}
			}
		}
	}

	Fclose( dd_handle );														/* Pipe schlie�en */
	rsrc_gaddr( 5, DDPROTOCOLL, &Ddprotocoll );
	form_alert(3, Ddprotocoll);
	return( -1 );
}

/*----------------------------------------------------------------------------------------*/
/* Drag & Drop - �berpr�fen ob der Empf�nger ein Format akzeptiert								*/
/* Funktionsresultat:	DD_OK: Empf�nger unters�tzt das Format										*/
/*								DD_EXT: Empf�nger akzeptiert das Format nicht							*/
/*								DD_LEN: Daten sind zu lang f�r den Empf�nger								*/
/*								DD_NAK: Fehler bei Kommunikation												*/
/*	dd_handle:					Handle der Pipe																	*/
/*	format:					K�rzel f�r das Format															*/
/*	name:						Beschreibung des Formats als C-String										*/
/*	size:						L�nge der zu sendenen Daten													*/
/*----------------------------------------------------------------------------------------*/
WORD	ddstry( WORD dd_handle, ULONG format, BYTE *name, LONG size )
{
	LONG	str_len;
	WORD	hdr_len;
	BYTE	reply;
	LONG	written;

	str_len = strlen( name ) + 1;										/* L�nge des Strings inklusive Nullbyte */
	hdr_len = 4 + 4 + (WORD) str_len;								/* L�nge des Headers */

	if ( Fwrite( dd_handle, 2, &hdr_len ) == 2 )						/* L�nge des Headers senden */
	{
		written = Fwrite( dd_handle, 4, &format );					/* Formatk�rzel */
		written += Fwrite( dd_handle, 4, &size );						/* L�nge der zu sendenden Daten */
		written += Fwrite( dd_handle, str_len, name );				/* Beschreibung des Formats als C-String */

		if ( written == hdr_len )										/* lie� sich der Header schreiben? */
		{
			if ( Fread( dd_handle, 1, &reply ) == 1 )
				return( reply );											/* Antwort zur�ckliefern */
		}
	}
	return( DD_NAK );
}

/*----------------------------------------------------------------------------------------*/
/* Drag & Drop - Pipe schlie�en																				*/
/*	dd_handle:					Handle der Pipe																	*/
/* oldpipesig:				Zeiger auf den alten Signalhandler											*/
/*----------------------------------------------------------------------------------------*/
void	ddclose( WORD dd_handle, void *oldpipesig )
{
	Psignal( SIGPIPE, oldpipesig );									/* wieder alten Dispatcher eintragen */
	Fclose( dd_handle );														/* Pipe schlie�en */
}