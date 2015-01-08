#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "wav.h"
#include "vinyl.h"
#include "point.h"
#include "surface.h"

#define DECODED_SOUND_FILE "vinyl_decoded.wav"
#define BLACK_THRESHOLD 200
#define DEGREE_STRIDE 0.1
#define SCREEN_WIDTH 2000
#define SCREEN_HEIGHT 2000

struct point surface_point_rotate(const struct point *p, double degree)
{
	// since point is rotated around the center (0,0), we substract
	// half of the screen WIDTH and HEIGHT from X and Y so as to move
	// it to the center (image coordinates are always positive and the 
	// (0, 0) position is in the top left corner
	int x_offset = SCREEN_WIDTH / 2;
	int y_offset = SCREEN_HEIGHT / 2;

	struct point aligned_pt = mkpoint(p->x - x_offset, p->y - y_offset);
	struct point rotated_pt = point_rotate(&aligned_pt, degree);

	return mkpoint(rotated_pt.x + x_offset, rotated_pt.y + y_offset);
}

// NOTE: since the plate is black & white, each pixel will have R == G == B
// (shades of grey). We use red pixel component but using G or B will produce
// the same result

// crawl 30 pixels in the direction of the plate center from the point equal to
// 'start', rotated 'deg' degree counterclockwise
// If we do not encounter any tracks on the way, we assume that we
// have reached the end of the plate (last track before large black
// circle in the middle of the plate)
bool looks_like_end(SDL_Surface *png_surface, struct point start, double deg)
{
	const int dead_end_pixel_count = 30;
	int black_pixels = 0;
	struct point pt;
	struct pixel pix;
	while (black_pixels < dead_end_pixel_count) {
		pt = surface_point_rotate(&start, deg);
		pix = get_pixel(png_surface, &pt);
		if (pix.r > BLACK_THRESHOLD) {
			break;
		}
		start.y++;
		black_pixels++;
	}
	return black_pixels == dead_end_pixel_count;
}

// crawl from the image top center (SCREEN_WIDTH / 2, 0), down along
// the Y axis till we spot the first white track coloured lighter than
// BLACK_THRESHOLD
struct point find_start(SDL_Surface* png_surface)
{
	int x_start, y_start;
	struct point start;
	int i;
	for (i = 0; i < SCREEN_HEIGHT / 2; i++) {
		struct point pt = mkpoint(SCREEN_WIDTH / 2, i);
		struct pixel pix = get_pixel(png_surface, &pt);
		if (pix.r > BLACK_THRESHOLD) {
			x_start = SCREEN_WIDTH / 2;
			y_start = i;
			start = mkpoint(x_start, y_start);

			put_red_pixel(png_surface, &start);
			break;
		}
	}
	return start;
}

void vinyl_decode(SDL_Window* sdl_window, SDL_Surface* screen_surface, SDL_Surface* png_surface)
{
	bool quit = false;
	int samples_written = 0;
	struct wav_hdr hdr;
	SDL_Event e;
	FILE *decoded_sound = fopen(DECODED_SOUND_FILE, "w");
	struct point start = find_start(png_surface);

	if (decoded_sound == NULL) {
		perror("Failed to write decoded data");
		return;
	}

	// save the place for wav header, which we will use to write
	// the actual header
	// We do not write it in advance since we have to know
	// how much samples we have read to put that count into wav_hdr
	fseek(decoded_sound, sizeof(hdr), SEEK_SET);

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		SDL_BlitSurface(png_surface, NULL, screen_surface, NULL);

		double deg;
		for (deg = 0; deg > -360.0; deg -= DEGREE_STRIDE) {
			struct point pnew;
			struct pixel pix;

			pnew = surface_point_rotate(&start, deg);
			pix = get_pixel(png_surface, &pnew);
			// check if we are still on the track
			if (pix.r > BLACK_THRESHOLD) {
				fwrite(&pix.r, sizeof(pix.r), 1, decoded_sound);
				samples_written++;
				put_red_pixel(png_surface, &pnew);
				continue;

			}

			// probe pixels above our point
			struct point yinc = mkpoint(start.x, start.y + 2);
			pnew = surface_point_rotate(&yinc, deg);
			pix = get_pixel(png_surface, &pnew);
			if (pix.r > BLACK_THRESHOLD) {
				start = yinc;
				fwrite(&pix.r, sizeof(pix.r), 1, decoded_sound);
				samples_written++;
				put_red_pixel(png_surface, &pnew);
				continue;
			}


			// probe pixels below our point
			struct point ydec = mkpoint(start.x, start.y - 2);
			pnew = surface_point_rotate(&ydec, deg);
			pix = get_pixel(png_surface, &pnew);
			if (pix.r > BLACK_THRESHOLD) {
				start = ydec;
				fwrite(&pix.r, sizeof(pix.r), 1, decoded_sound);
				samples_written++;
				put_red_pixel(png_surface, &pnew);
				continue;
			}

			// awkward case when we are on the dark spot and pixels
			// above and below are also dark - this means that we have
			// either reached the end of the plate...
			if (looks_like_end(png_surface, start, deg)) {
				quit = true;
				break;
			}

			// ...or we have stumbled upon the 'bump' which is lower than
			// BLACK_THRESHOLD value but is still on the track
			pnew = surface_point_rotate(&start, deg);
			pix = get_pixel(png_surface, &pnew);
			fwrite(&pix.r, sizeof(pix.r), 1, decoded_sound);
			put_red_pixel(png_surface, &pnew);
			samples_written++;
		}

		SDL_UpdateWindowSurface(sdl_window);
	}

	// replace placeholder wav_hdr struct in the beginning of file with the actual one
	hdr = mk_wav_hdr(samples_written);
	rewind(decoded_sound);
	fwrite(&hdr, sizeof(hdr), 1, decoded_sound);
	fclose(decoded_sound);
	printf("wrote samples: %d\n", samples_written);
	usleep(1000000);
}
int main(int argc, char* args[])
{
	SDL_Window* sdl_window = NULL;
	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;
	SDL_Surface* png_surface = NULL;
	const char *plate_name = args[1];

	if (argc != 2) {
		printf("Usage:\n"
			"%s <vinyl-image-to-decode>\n", args[0]);
		goto fail;
	}

	//Start up SDL and create window
	if (!vinyl_init()) {
		goto fail;
	}

	sdl_window = SDL_CreateWindow( "SDL Tutorial", 500, 500, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (sdl_window == NULL ) {
		printf( "Failed to create SDL Window: %s\n", SDL_GetError() );
		goto window_creation_fail;
	}

	screen_surface = SDL_GetWindowSurface(sdl_window);
	if (screen_surface == NULL ) {
		printf("Failed to get window surface: %s\n", SDL_GetError());
		goto surface_creation_fail;
	}

	png_surface = load_surface(screen_surface, plate_name);
	SDL_FreeSurface(screen_surface);
	if (png_surface == NULL) {
		printf("Failed to load PNG image: %s\n", SDL_GetError());
		goto surface_creation_fail;
	}

	vinyl_decode(sdl_window, screen_surface, png_surface);

	SDL_FreeSurface(png_surface);
surface_creation_fail:
	SDL_DestroyWindow(sdl_window);
window_creation_fail:
	vinyl_exit();
fail:
	return 0;
}

