/* F”r att underl„tta f”r grafber„kningen */

#define XMIN			(tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) +6)
#define XMAX			(tree[MAIN].ob_x + (tree[MAIN].ob_width / 2) - (tree[RUTAN].ob_width / 2) + tree[RUTAN].ob_width - 8 )
#define YMIN			(tree[MAIN].ob_y + tree[RUTAN].ob_height + 8)
#define YMAX			(tree[MAIN].ob_y + tree[RUTAN].ob_height + 20  - tree[RUTAN].ob_height)
#define WIDTH			(XMAX-XMIN)
#define HEIGHT			(YMAX-YMIN)
