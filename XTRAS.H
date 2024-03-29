

/* Some extra definitions that are not in
 * any special header file
 **************************************/

#include <aes.h>
#include <portab.h>
/*#include <magx.h>*/


#define MGLOBAL            0x20
#define BUBBLEGEM_REQUEST  0xBABA
#define BUBBLEGEM_SHOW     0xBABB
#define BUBBLEGEM_ACK      0xBABC
#define BUBBLEGEM_ASKFONT  0xBABD
#define BUBBLEGEM_FONT     0xBABE
#define BUBBLEGEM_HIDE     0xBABF
#define BA_BUBBLE_REQUEST  0x4600
#define AB_OPEN_BUBBLE     0x4601
#define BA_BUBBLE_OPENED   0x4602
#define AB_CLOSE_BUBBLE    0x4603
#define BA_BUBBLE_CLOSED   0x4604
#define MagX_COOKIE        0x4D616758L
#define MiNT_COOKIE        0x4D694E54L
#define NVDI_COOKIE			0x4e564449L
/*#define FONT_CHANGED       0x7a18
#define FONT_ACK           0x7a1a*/
#define AV_SENDCLICK       0x4709

#define min(a, b) 				((a) < (b) ? (a) : (b))
#define max(a, b) 				((a) > (b) ? (a) : (b))

/* Colors Drag'n Drop */

#define COLOR_ID 0xC010

/* GEMScript defines */

#define GS_REQUEST 0x1350
#define GS_REPLY   0x1351
#define GS_COMMAND 0x1352
#define GS_ACK     0x1353
#define GS_QUIT    0x1354

#define GSACK_OK      0    /* OK, the command has been or will be executed */
#define GSACK_UNKNOWN 1    /* Unknown command */
#define GSACK_ERROR   2    /* Error (Command not executed) */

/*typedef struct {
   long len;       /* Length in structure in bytes                      */
   int  version;   /* The senders' version number of the protocoll
                      (currently 0x0100 = 1.0)                             */
   int  msgs;      /* Bitmap of the supported messages (GSM_xxx)   */
   long ext;       /* used extension, like '.SIC'                     */
} GS_INFO; */

typedef struct {
	int gs_id;
	int gs_partnerid;
} GS_PARTNER;

#define GSM_COMMAND 0x0001  /* can receive GS_COMMAND                */
#define GSM_MACRO   0x0002  /* can receive GS_OPENMACRO, GS_WRITE and GS_CLOSEMACRO */
                            /* can send GS_MACRO (Interpreter)       */
#define GSM_WRITE   0x0004  /* can send GS_OPENMACRO, GS_WRITE and GS_CLOSEMACRO */
                            /* receive GS_MACRO (Application that can record scripts)  */


/* AVSTR2MSG inserts the value of a string pointer into a message array.
       msg    : The Integer array.
       offset : Lower index number of the interger pair
       str    : Pointer to str�ng
*/
#define	AVSTR2MSG(msg,offset,str)	(*(char **)(msg+offset) = str)

/* AVMSG2STR makes a string pointer from an integer pair.
       msg    : The name of the integer array
       offset : Smaller index of the integer pair.
*/
#define	AVMSG2STR(msg,offset)			((char *)(((long)msg[offset]<<16)|(((long)msg[offset+1])&0xffff)))

#define RADER  128
#define TECKEN 255

#define ANTAL_FUNKTIONER  21
#define ANTAL_ENHETER	255
#define MAX_TECKEN    16

#define BAD_LANGUAGE_SOUND 1  /* Tells the sound routine which sound to play */
#define ERASE_DD_SOUND 2
#define STARTUP_SOUND 3
#define QUIT_SOUND 4

#define ADDTOMENU 121
#define NOTEOPEN 134

#define LEFT 0
#define RIGHT 1

extern WORD MT_form_popup( OBJECT *tree, WORD x, WORD y, WORD *global );		/* MAG!X */
extern WORD MT_form_xdo( OBJECT *tree, WORD startob, WORD *lastcrsr, XDO_INF *tabs, void *flydial, WORD *global );	/* MAG!X       */
extern WORD MT_form_xdial( WORD subfn, GRECT *lg, GRECT *bg, void **flyinf, WORD *global );
/*
#define	form_popup( a, b, c ) \
			MT_form_popup( a, b, c, 0L )
#define	form_xdo( a, b, c, d, e ) \
			MT_form_xdo( a, b, c, d, e, 0L )
#define	form_xdial( a, b, c, d ) \
			MT_form_xdial( a, b, c, d, 0L ) */