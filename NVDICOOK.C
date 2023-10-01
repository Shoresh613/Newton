
/* int get_NVDI_cookie(&val)
 * Visar vilken version av NVDI som „r installerad
 ************************************************/

#include <tos.h>
#include <stdio.h>
#include <portab.h>
#include "xtras.h"

typedef struct {
    long cookie;
    long value;
} COOKIE;

/*typedef struct {
   UWORD nvdi_version;  /*  z.B. 0x0301 fr Version 3.01 */
   ULONG nvdi_datum;    /*  z.B. 0x18061990L fr 18.06.1990 */
} NVDI_STRUC;*/


int get_NVDI_cookie( long *p_value )
{
	long target=NVDI_COOKIE;
	register COOKIE *cookie_ptr;
	void *stack_ptr = NULL;             /* saved stack pointer          */
	NVDI_STRUC *nvdiptr = NULL;

	if( !Super( (void *)1L ) )          /* are we in supervisor mode?   */
		stack_ptr = (void *)Super( NULL );  /* no, so change to it       */

	cookie_ptr = *(COOKIE **) 0x5a0;    /* get pointer to cookie jar    */
	if (cookie_ptr != NULL){
		do {
            if(cookie_ptr->cookie == target) {
                nvdiptr = (NVDI_STRUC*)cookie_ptr->value;
                if(p_value != NULL) {

                    *p_value = nvdiptr->nvdi_version;

                    if( stack_ptr ) {       /* if we switched stacks    */
                        Super( stack_ptr ); /* restore old stack        */
                    }
                    return (1);
                }
            }
        } while((cookie_ptr++)->cookie != 0L);
    }
    if( stack_ptr )                     /* if we switched stacks        */
        Super( stack_ptr );             /* restore old stack            */
    return (0);                         /* return error status          */
}