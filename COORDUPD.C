/* Updates the x and y coordinates when the mouse hovers over the graph in
   graph mode. */

#include <string.h>
#include <stdlib.h>
#include <aes.h>
#include <tos.h>
#include "newton.h"

extern double coordarray[2048];
extern char* xcoordinate, *ycoordinate;
extern OBJECT *tree;

void update_coordinates(int mx){

	char* tmp=0L;
	tmp=(char*)Malloc(30);
	memset(tmp,0,30);

	strcpy(xcoordinate,"x: ");
	strcat(xcoordinate,gcvt(coordarray[2*(mx-(tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +6)) ],15,tmp));

	strcpy(ycoordinate,"y: ");
	strcat(ycoordinate,gcvt(coordarray[2*(mx-(tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +6)) +1 ],15,tmp));
	Mfree(tmp);
}
