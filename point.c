#include <math.h>

#include "point.h"

struct point mkpoint(int x, int y)
{
	struct point ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

static inline double deg_to_rad(double deg)
{
	return deg * M_PI / 180.0;
}

struct point point_rotate(const struct point *p, double degree)
{
	double rad = deg_to_rad(degree);
	int x_new = p->x * cos(rad) - p->y * sin(rad);
	int y_new = p->x * sin(rad) + p->y * cos(rad);

	return mkpoint(x_new, y_new);
}
