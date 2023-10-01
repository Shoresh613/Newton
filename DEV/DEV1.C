	/* Petit exemple pour avoir des ic“nes dans les titres: */
	/* Small exemple to have icons in the titles: */

#include <stdio.h>
#include <string.h>
#include <aes.h>
#include <vdi.h>
#include <tos.h>
#include "w_xgem.h"
#include "dev1.h"


	/* Prototypage: */
int main (void);
void open_work (void);
void redraw_wind (int x, int y, int w, int h);
int rc_intersect (GRECT *p1, GRECT *p2);


	/* Variables globales */
int ap_id;									/* Identificateur application */
int buf[8];									/* buffer gem */
int ap_id, hcell, n_plane, vdi_handle, wcell, extended_inquire[57], work_out[57];	/* pour la vdi */
OBJECT *adr_icon1;

#define max(A, B) ((A) > (B) ? (A) : (B))
#define min(A, B) ((A) < (B) ? (A) : (B))


int main (void)
{
	char title[256];
	int handle;

	ap_id = appl_init ();					/* D‚clarer l'application au GEM */

	if (rsrc_load ("DEV1.RSC") <= 0)		/* Charger le ressource */
	{
		appl_exit ();
		return (0);
	}

	open_work ();

	rsrc_gaddr (0, ICON1, &adr_icon1);		/* Demander adresse ic“ne */
	graf_mouse (ARROW, 0L);					/* Souris : forme de flŠche */

	handle = wind_create (NAME|CLOSER|MOVER|FULLER|SIZER, 50, 50, 200, 100);	/* cr‚er la fenˆtre */
	if (handle < 0)
		goto end;

	if (is_xtitle ())						/* Windframe Xgem r‚cent ? */
		sprintf (title, "\xFF\xFF\xFF\xFF%08lX%s", adr_icon1, "Just a test !");	/* inscrire le titre avec ic“ne */
	else
		strcpy (title, "Just a test !");	/* texte seulement */
	wind_set (handle, WF_NAME, title);

	wind_open (handle, 50, 50, 200, 100);	/* ouvrir la fenˆtre */

	for (;;)
	{
		evnt_mesag (buf);

		switch (buf[0])
		{
			case WM_CLOSED:
				if (buf[3] == handle)
				{
					wind_close (handle);
					wind_delete (handle);
					goto end;
				}
			break;
			case WM_MOVED:
				if (buf[3] == handle)
					wind_set (handle, WF_CURRXYWH, buf[4], buf[5], buf[6], buf[7]);
			break;
			case WM_TOPPED:
				if (buf[3] == handle)
					wind_set (handle, WF_TOP);
			break;
			case WM_REDRAW:
				if (buf[3] == handle)
					redraw_wind (buf[4], buf[5], buf[6], buf[7]);
			break;
			case WM_SIZED:
				if (buf[3] == handle)
					wind_set (handle, WF_CURRXYWH, buf[4], buf[5], buf[6], buf[7]);
			break;
		}
	}

end:
	v_clsvwk (vdi_handle);					/* Rendre le handle vdi */
	rsrc_free ();							/* Lib‚rer le ressource */
	appl_exit ();							/* Quitter */

	return (0);
}


void open_work (void)
{
	int dummy, *p_intin;

	p_intin = &_VDIParBlk.intin[0];

		/* L'identificateur physique d‚pend de la r‚solution */
	(*p_intin ++) = Getrez () + 2;

		/* Remplir le tableau intin[] */
	(*((long *)p_intin) ++) = (*((long *)p_intin) ++) = (*((long *)p_intin) ++) = (*((long *)p_intin) ++) = 0x10001L;

		/* SystŠme de coordonn‚es RC */
	(*(long *)(p_intin)) = 0x10002L;

		/* Handle de la station que l'AES ouvre automatiquement */
	vdi_handle = graf_handle (&wcell, &hcell, &dummy, &dummy);

		/* Ouverture la station de travail virtuelle */
	v_opnvwk (_VDIParBlk.intin, &vdi_handle, work_out);

		/* Infos suppl‚mentaires */
	vq_extnd (vdi_handle, 1, extended_inquire);	
	n_plane = extended_inquire[4];
}


	/* Gestion des redraws de la fenˆtre: */
void redraw_wind (int x, int y, int w, int h)
{
	int pxy[4];
	GRECT r, rd;

	rd.g_x = x;								/* Coordonn‚es rectangle … redessiner */
	rd.g_y = y;
	rd.g_w = w;
	rd.g_h = h;

	graf_mouse (M_OFF, 0L);					/* Virer la souris */
	wind_update (BEG_UPDATE);				/* Bloquer les fonctions de la souris */

	vsf_perimeter (vdi_handle, 0);
	vsf_color (vdi_handle, BLUE);
	vsf_interior (vdi_handle, FIS_HATCH);
	vsf_style (vdi_handle, 2);

		/* Demande les coord. et dimensions du 1ø rectangle de la liste */
	wind_get (buf[3], WF_FIRSTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	while (r.g_w && r.g_h)					/* Tant qu'il y a largeur ou hauteur... */
	{
		if (rc_intersect (&rd, &r))			/* Si intersection des 2 zones */
		{
			pxy[0] = r.g_x;
			pxy[1] = r.g_y;
			pxy[2] = r.g_x + r.g_w - 1;
			pxy[3] = r.g_y + r.g_h - 1;
			vs_clip (vdi_handle, 1, pxy);	/* Clipping ON */

			wind_get (buf[3], WF_WORKXYWH, &pxy[0], &pxy[1], &pxy[2], &pxy[3]);
			pxy[2] += (pxy[0] - 1);
			pxy[3] += (pxy[1] - 1);
			v_bar (vdi_handle, pxy);

			vs_clip (vdi_handle, 0, pxy);	/* Clipping OFF */
		}
			/* Rectangle suivant */
		wind_get (buf[3], WF_NEXTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	};

	wind_update (END_UPDATE);				/* D‚bloquer les fonctions de la souris */
	graf_mouse (M_ON, 0L);					/* Rappeler la souris */
}


	/* Calcule l'intersection de 2 rectangles */
int rc_intersect (GRECT *p1, GRECT *p2)
{
	int tx, ty, tw, th;

	tw = min (p2->g_x + p2->g_w, p1->g_x + p1->g_w);
	th = min (p2->g_y + p2->g_h, p1->g_y + p1->g_h);
	tx = max (p2->g_x, p1->g_x);
	ty = max (p2->g_y, p1->g_y);
	p2->g_x = tx;
	p2->g_y = ty;
	p2->g_w = tw - tx;
	p2->g_h = th - ty;

	return ((tw > tx) && (th > ty));
}
