
/* get_NVDI_cookie()
 * Shows which version of NVDI is installed
 ************************************************/

/* NOTE. This crashed computers with MMU. Now testing new routine in the new file */

#include <tos.h>
#include <portab.h>

long nvdi_cookie = 0L;

typedef struct nvdi_struc
{
   UWORD nvdi_version;            /*  z.B. 0x0301 for Version 3.01 */
   ULONG nvdi_datum;    /*  z.B. 0x18061990L for 18.06.1990 */
} NVDI_STRUC;


/*** get NVDI cookie ****/
static long get_NVDI()
{
	long *cookie;

	cookie = *((long **) 0x5a0);
	if (!cookie)
		nvdi_cookie = 0;
	else {
		while (*cookie) {

			if (*cookie == 0x4e564449L) {
				nvdi_cookie = cookie[1];
				return 0;
			}
			cookie += 2;
		}
	}
	nvdi_cookie = 0;
	return 0;
}

UWORD get_NVDI_cookie()
{
	NVDI_STRUC *nvdiptr;

	Supexec(get_NVDI);

	if (nvdi_cookie != 0L)
	{
		nvdiptr = (NVDI_STRUC*)(nvdi_cookie);
		return nvdiptr->nvdi_version;
	}
	else
		return 0;
}