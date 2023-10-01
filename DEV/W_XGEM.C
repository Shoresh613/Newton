#include <tos.h>
#include "w_xgem.h"

	/* chercher si Xgem est pr‚sent, si le windframe Xgem est pr‚sent et r‚cent: */
int is_xtitle (void)
{
	STYLER styler;
	COOKIE *cook;

	if ((cook = vq_cookie (0x5867656DL)) != 0L)			/* Xgem pr‚sent ? */
	{
		styler = *(STYLER *)cook->datack.v;				/* r‚cup‚rer les infos sur Xgem */

		if (*styler.wversion)							/* infos sur le windframe ? */
			return (1);									/* -> c'est une version r‚cente */
	}

	return (0);
}


	/* Chercher cookie suivant: */
COOKIE *next_cookie (COOKIE *ck)
{
	if (! ck->ident)									/* C'‚tait le dernier cookie */
		return (0L);
	return (++ ck);
}


	/* Chercher un cookie et sa valeur: */
COOKIE *vq_cookie (long id)
{
	COOKIE *cook;
	long save_ssp;

	save_ssp = Super (0L);
	cook = *(COOKIE **)0x5a0;							/* Le cookie jar */
	Super ((void *)save_ssp);

	while (cook)
	{
		if (cook->ident == id)							/* Trouv‚ ! */
			return (cook);
		cook = next_cookie (cook);
	}

	return ((COOKIE *)0L);								/* Pas trouv‚ */
}
