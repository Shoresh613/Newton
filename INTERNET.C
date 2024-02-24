/*
 * Calls the browser or mailer for contact purposes.
 *******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <aes.h>
#include <tos.h>
#include <string.h>
#include <ext.h>
#include <vaproto.h>
#include "xtras.h"
#include "newton.h"

extern int MagX, MiNT;
extern int msg[8], appl_id;

void start_browser( void )
{
    char *No_Browser=NULL;
    char *env=NULL, *cmdline=NULL, *tmp=NULL;

    if (MiNT || MagX)
    {
        env = (char *) Mxalloc (256, 0 | MGLOBAL);
        cmdline = (char *) Mxalloc (256, 0 | MGLOBAL);
        tmp = (char *) Mxalloc (256, 0 | MGLOBAL);
    }
    else
    {
        env = (char *) Malloc (256);
        cmdline = (char *) Malloc (256);
        tmp = (char *) Malloc (256);
    }
    if (!cmdline)       /* If there is not enough memory */
        return;

    env=getenv("BROWSER");
    if( env==NULL )
        env=getenv("WWW_CLIENT");
    if( env!=NULL ) /* If $BROWSER or $WWW_CLIENT is set */
    {
        strcpy(tmp,env);
        strcpy(cmdline,"https://github.com/Shoresh613/Newton");
        msg[0]=AV_STARTPROG;
        msg[1]=appl_id;
        msg[2]=0;
        AVSTR2MSG(msg,3,tmp);
        AVSTR2MSG(msg,5,cmdline);
        msg[7]=0;

        if( appl_write( 0 /* AV-Server, desktop */, 16, msg ) == 0 ) /* If it didn't work */
            return;                                  /* Do nothing.      */
    }
    else
    {
        rsrc_gaddr( R_STRING, BROWSER, &No_Browser );
        form_alert(1,No_Browser);
    }
    evnt_timer(200,0);
    Mfree(env);Mfree(cmdline);Mfree(tmp);
    return;
}

void start_mailer( void )
{
    char *No_Mailer=NULL;
    char *env=NULL, *cmdline=NULL, *tmp=NULL;

    if (MiNT || MagX)
    {
        env = (char *) Mxalloc (256, 0 | MGLOBAL);
        cmdline = (char *) Mxalloc (256, 0 | MGLOBAL);
        tmp = (char *) Mxalloc (256, 0 | MGLOBAL);
    }
    else
    {
        env = (char *) Malloc (256);
        cmdline = (char *) Malloc (256);
        tmp = (char *) Malloc (256);
    }
    if (!tmp)       /* If there is not enough memory */
        return;

    memset(env, 0, 256);
    memset(cmdline, 0, 256);
    memset(tmp, 0, 256);

    env=getenv("MAILER");
    if( env!=NULL ) /* If $MAILER is set */
    {
        strcpy(tmp,env);
        strcpy(cmdline,"mailto:mikael_f@swipnet.se");
        
        msg[0]=AV_STARTPROG;
        msg[1]=appl_id;
        msg[2]=0;
        AVSTR2MSG(msg,3,tmp);
        AVSTR2MSG(msg,5,cmdline);
        msg[7]=0;

        if( appl_write( 0 /* AV-Server, desktop */, 16, msg ) == 0 ){ /* If it didn't work */
            Mfree(env);Mfree(cmdline);Mfree(tmp);    /* Free memory */
            return;                                  
        }
    }
    else
    {
        rsrc_gaddr( R_STRING, MAILER, &No_Mailer );
        form_alert(1,No_Mailer);
    }
    Mfree(env);Mfree(cmdline);Mfree(tmp);
    return;
}
