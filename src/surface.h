struct SDL_Surface;
struct point;

struct pixel
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

SDL_Surface* load_surface(SDL_Surface *screen, SDL_Surface *loadedSurface);
void put_red_pixel(SDL_Surface *surface, const struct point *pt);
struct pixel get_pixel(SDL_Surface *surface, const struct point *pt);
