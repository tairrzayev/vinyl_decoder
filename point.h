struct point {
	int x;
	int y;
};

struct point mkpoint(int x, int y);
struct point point_rotate(const struct point *p, double degree);

