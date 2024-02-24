#include <stdio.h>
#include <portab.h>
#include <stdlib.h>
#include <string.h>
#include <aes.h>
#include <ctype.h>
#include "xtras.h"
#include "newton.h"

extern char konstant_lista[ANTAL_ENHETER][MAX_TECKEN];  /* These are declared in newton.c, loaded at boot from newton.cnf, then copied over to variabel* in cuttryck in calculate() */
extern double konstant_storhet[ANTAL_ENHETER];
extern int antal_konstanter;
extern int MagX, MiNT;

extern char* calculate(char* string);
extern char* preprocess(char* string);

void read_constants(void) /* This is called at boot */
{
    int i = 1, end = 0;
    FILE* fp = NULL;
    char* Constantsfile_error = NULL;
    char* ConstantTooLong = NULL;
    char* ConstantSyntax = NULL;
    char* home = NULL;
    char tmp[256]; /* Changed to use a fixed-size array (no need to Mfree()) */

    rsrc_gaddr(R_STRING, CONSTANTSERROR, &Constantsfile_error);
    rsrc_gaddr(R_STRING, CONSTANTTOOLONG, &ConstantTooLong);
    rsrc_gaddr(R_STRING, CONSTANTSYNTAX, &ConstantSyntax);

    home = getenv("HOME");
    if (home == NULL) /* If $HOME is not set */
    {
        if ((fp = fopen("newton.cnf", "r")) == NULL)
        {
            if ((fp = fopen("NEWTON.CNF", "r")) == NULL)
            {
                form_alert(1, Constantsfile_error);
                return;
            }
        }
    }
    else /* If $HOME is set */
    {
        sprintf(tmp, "%s\\defaults\\newton.cnf", home);
        if ((fp = fopen(tmp, "r")) == NULL)
        {
            sprintf(tmp, "%s\\newton.cnf", home);
            if ((fp = fopen(tmp, "r")) == NULL)
            {

                form_alert(1, Constantsfile_error);
                fclose(fp);
                return;
            }
        }
    }

    do
    {
        end = fscanf(fp, "%s", konstant_lista[i]);
        if (strlen(konstant_lista[i]) > MAX_TECKEN) /* If the variable name is too long */
        {
            form_alert(1, ConstantTooLong);
            fclose(fp);
            return;
        }

        if (end == EOF)  /* If you have already loaded the last one, you can skip the rest */
        {
            antal_konstanter = i;
            fclose(fp);
            return;
        }

        if (!((konstant_lista[i][0] <= 'Z') && (konstant_lista[i][0] >= 'A')))
        {
            form_alert(1, ConstantSyntax); /* Instead of alerts, you can make dialog boxes. Then you can tell where the error lies */
            fclose(fp);
            return;
        }

        end = fscanf(fp, "%s", tmp);
        if (!isdigit(*tmp))
        {
            form_alert(1, ConstantSyntax);
            fclose(fp);
            return;
        }
        konstant_storhet[i] = atof(calculate(preprocess(tmp)));
        i++;
        antal_konstanter = i;
    } while (end != EOF && i < ANTAL_ENHETER);

    fclose(fp);
    return;
}
