#include <aes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "newton.h"
#include "xtras.h"

extern OBJECT* graph_dial;
extern char* ekv;
extern int graph_mode, draw_grid, linethgraph;
extern long grid_distx, grid_disty;
extern double xmin, xmax, ymin, ymax;
extern int n, synliga_rader;
extern char s[RADER][TECKEN];
extern int scrollVector[RADER];

extern int do_dialog(OBJECT *dialog);
extern void redraw_window( int all );

int graph_dialog(void)
{
	char *strptr, *strptr2, *strptr3, *strptr4, *strptr5, *strptr6, *strptr7, *strptr8;
	TEDINFO *obspec, *obspec2, *obspec3, *obspec4, *obspec5, *obspec6, *obspec7, *obspec8;
	int exitbutn, done=0, i=0;
	char *ErrorForm=NULL, *buf=NULL, *tmp=NULL;

	rsrc_gaddr( R_TREE, GRAPH, &graph_dial );
	rsrc_gaddr( R_STRING, ALLFIELDS, &ErrorForm );
	
	obspec = graph_dial[FX].ob_spec.tedinfo;
	strptr = obspec->te_ptext;
	memset(strptr, 0, 59);
	obspec2 = graph_dial[XMING].ob_spec.tedinfo;
	strptr2 = obspec2->te_ptext;
	memset(strptr2, 0, 20);
	obspec3 = graph_dial[XMAXG].ob_spec.tedinfo;
	strptr3 = obspec3->te_ptext;
	memset(strptr3, 0, 20);
	obspec4 = graph_dial[YMING].ob_spec.tedinfo;
	strptr4 = obspec4->te_ptext;
	memset(strptr4, 0, 20);
	obspec5 = graph_dial[YMAXG].ob_spec.tedinfo;
	strptr5 = obspec5->te_ptext;
	memset(strptr5, 0, 20);
	obspec6 = graph_dial[GRIDX].ob_spec.tedinfo;
	strptr6 = obspec6->te_ptext;
	memset(strptr6, 0, 20);
	obspec7 = graph_dial[GRIDY].ob_spec.tedinfo;
	strptr7 = obspec7->te_ptext;
	memset(strptr7, 0, 20);
	obspec8 = graph_dial[LINETHGRAPH].ob_spec.tedinfo;
	strptr8 = obspec8->te_ptext;
	memset(strptr8, 0, 1);

	if(strlen(strptr)==0) { /* Funkar ju inte s† bra eftersom den memsettas ovan */
		tmp = Malloc(TECKEN*synliga_rader);	/* Anv„nder h„r f”r att bygga upp flerradigt uttryck */
		memset(tmp,0,sizeof(tmp));
	
		i=n;
		if(i>0) {
			while(scrollVector[i-1] && i>0){	/* Tar reda p† var uttrycket b”rjar */
				i--;
			}
		}
		strcpy(tmp,s[i]);
		while(scrollVector[i]){ /* Kopiera in alla rader som h”r till uttrycket i bufferten. */
			strcat(tmp, s[i+1] );
			i++;
		}
		strcpy(strptr,tmp);
	}
	if(xmin==4038){	/* Den „r detta udda tal vid uppstart, f”r initialisering */
		strcpy(strptr2,"-10");
		strcpy(strptr3,"10");
		strcpy(strptr4,"-10");
		strcpy(strptr5,"10");
		strcpy(strptr6,"1"); /* Denna „r gridavst†nd x*/
		strcpy(strptr7,"1"); /* Denna „r gridavst†nd y*/
		strcpy(strptr8,"1"); /* Denna „r linjetjocklek */
	}
	else{
		buf=(char*)Malloc(40);
		memset(buf,0,sizeof(buf));
		strcpy(strptr2,gcvt(xmin,10,buf));
		strcpy(strptr3,gcvt(xmax,10,buf));
		strcpy(strptr4,gcvt(ymin,10,buf));
		strcpy(strptr5,gcvt(ymax,10,buf));
		strcpy(strptr6,ltoa(grid_distx,strptr6,10)); /* Denna „r gridavst†nd x */
		strcpy(strptr7,ltoa(grid_disty,strptr7,10)); /* Denna „r gridavst†nd y */
		strcpy(strptr8,itoa(linethgraph,strptr8,10)); /* Denna „r linjetjocklek */	
		Mfree(buf);
	}
	do{
		exitbutn = do_dialog( graph_dial );
		if(exitbutn!=CANCELGRAPH && ( strlen(strptr)==0 || strlen(strptr2)==0 || strlen(strptr3)==0 || strlen(strptr4)==0 || strlen(strptr5)==0 || strlen(strptr6)==0 || strlen(strptr7)==0 || strlen(strptr8)==0) )
			form_alert(1,ErrorForm);
		else
			done=1;
	}while( !done );

	if(exitbutn==CANCELGRAPH)
		return -1;
	else
	{
		if(graph_dial[DRAWGRID].ob_state & SELECTED)
			draw_grid=1;
		else
			draw_grid=0;

		strcpy(ekv,strptr);
		xmin=atof(strptr2);
		xmax=atof(strptr3);
		ymin=atof(strptr4);
		ymax=atof(strptr5);
		grid_distx=atol(strptr6);
		grid_disty=atol(strptr7);
		linethgraph=atoi(strptr8);
		return 0;
	}
}