

/* Get_cookie(target,*p_value)
 * Returnerar v„rdet av den s”kta cookien.
 *************************************/


#include <tos.h>
#include <stdio.h>

typedef struct {
    long cookie;
    long value;
} COOKIE;


int Get_cookie( long target, long *p_value )
{
	register COOKIE *cookie_ptr;
	void *stack_ptr = NULL;             /* saved stack pointer          */


	if( !Super( (void *)1L ) )          /* are we in supervisor mode?   */
		stack_ptr = (void *)Super( NULL );  /* no, so change to it       */

	cookie_ptr = *(COOKIE **) 0x5a0;    /* get pointer to cookie jar    */
	if (cookie_ptr != NULL){
		do {
            if(cookie_ptr->cookie == target) {
                if(p_value != NULL) {
                    *p_value = cookie_ptr->value;
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