#if !defined (_W_X_GEM)
#define _W_X_GEM

	/* pour l'appel de fonctions externes: */
typedef void (*FNCPS)(int vdi_handle, int px, int py, int pw, int ph, int mode);

	/* structure pour l'apparence des boŒtes: */
#define STYLER struct STYLER
STYLER
{
	unsigned long ability;				/* ce que peut g‚rer le styler */

		/* modules d'aspect et autres: */
	void *mod_dfunc;					/* adresse du module de dessin du fond */
	FNCPS skin_dfunc;					/* fonction de dessin du fond */
	char res_m[24];						/* r‚serv‚ */

		/* r‚glages pour les boŒtes: */
	int background_dcolor;				/* couleur du fond des boŒtes */
	int box_dcolor;						/* couleur du cadre */
	int elight_dcolor;					/* couleur du bord clair */
	int edark_dcolor;					/* couleur du bord fonc‚ */
	int text_dcolor;					/* couleur du texte */
	char res_d[22];						/* r‚serv‚ */

		/* r‚glages pour les fenˆtres: */

		/* g‚n‚raux: */
	char wversion[8];					/* version du windframe */
	char wuser[32];						/* nom de l'utilisateur */
	int ifont_wtype;					/* type de fonte de la barre d'info (1: gem, 0: mini-fonte xgem) */
	char res_w[22];						/* r‚serv‚ */

		/* premier plan: */
	int background_wfcolor;				/* couleur du fond de la fenˆtre */
	int title_wfcolor;					/* couleur du 'bouton' du titre */
	int ttext_wfcolor;					/* couleur du texte du titre */
	int tshadow_wfcolor;				/* ombrage du titre (0: on / 1: off) */
	int ttab_wfform;					/* style du 'bouton' de titre (0: arrondi, 1: invisible, ...) */
	char res_wf[22];					/* r‚serv‚ */

		/* arriŠre plan: */
	int background_wbcolor;				/* couleur du fond de la fenˆtre */
	int title_wbcolor;					/* couleur du 'bouton' du titre */
	int ttext_wbcolor;					/* couleur du texte du titre */
	int tshadow_wbcolor;				/* ombrage du titre (0: on / 1: off) */
	int ttab_wbform;					/* style du 'bouton' de titre (0: arrondi, 1: invisible, ...) */
	char res_wb[22];					/* r‚serv‚ */

	char res_g[60];						/* r‚serv‚ */
};

	/* gestion des cookies: */
typedef struct
{
	long ident;
	union
	{
		void *v;
		long l;
		int i[2];
		char c[4];
	} datack;
} COOKIE;

COOKIE *next_cookie (COOKIE *ck);
COOKIE *vq_cookie (long id);

int is_xtitle (void);

#endif