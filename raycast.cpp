#include <cmath>
#include <ctime>
#include <iostream>

#include <SDL2/SDL.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define FOV 60 * M_PI / 180

#define WORLD_X 20
#define WORLD_Y 20

#define MAXDIST 20

#define ROTATE_VEL 3.5
#define MOVE_VEL   3.5

typedef struct {
	double x, y;
} Vector;

SDL_Window *window = NULL;
SDL_Surface *surface = NULL;
SDL_Renderer *renderer = NULL;

clock_t last_frame_time, frame_time;

int world[WORLD_Y][WORLD_X] = {
	// {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	// {1,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,1},
	// {1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1},
	// {1,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,1},
	// {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
	// {1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
	// {1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
	// {1,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,0,1},
	// {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1},
	// {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	// {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
	// {1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1},
	// {1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1},
	// {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
	// {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
	// {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
	// {1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1},
	// {1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1},
	// {1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1},
	// {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},

	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,},
	{1,1,1,1,1,1,1,1,0,0,0,0,2,2,2,2,2,2,2,1,},
	{1,1,1,1,1,1,1,1,0,0,0,0,3,2,2,2,2,2,2,2,},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
	{1,2,2,2,2,2,2,2,0,0,0,0,1,1,1,1,1,1,1,1,},
	{1,2,2,2,2,2,2,2,0,0,0,0,1,1,1,1,1,1,1,1,},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
	{1,1,1,1,1,1,1,1,0,0,0,0,2,2,2,2,2,2,2,2,},
	{1,1,1,1,1,1,1,1,0,0,0,0,2,2,2,2,2,2,2,2,},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
	{1,3,3,3,3,3,3,3,0,0,0,0,1,1,1,1,1,1,1,1,},
	{1,2,2,2,2,2,2,2,0,0,0,0,1,1,1,1,1,1,1,1,},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
	{1,1,1,1,1,1,1,1,0,0,0,0,2,2,2,2,2,2,2,2,},
	{1,1,1,1,1,1,1,1,0,0,0,0,2,2,2,2,2,2,2,2,},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,},
};

bool init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL couldn't initialize: " << SDL_GetError() << std::endl;
		success = false;
	} else {
		window = SDL_CreateWindow("raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			std::cerr << "Window wasn't created: " << SDL_GetError() << std::endl;
			success = false;
		} else {
			surface = SDL_GetWindowSurface(window);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		}
	}

	return success;
}

void close() {
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void draw_bg() {
	SDL_Rect sky;

	SDL_SetRenderDrawColor(renderer, 70, 70, 70, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	sky.x = 0;
	sky.y = 0;
	sky.w = 640;
	sky.h = 240;

	SDL_SetRenderDrawColor(renderer, 90, 140, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &sky);
}

void rotate(Vector *v1, Vector *v2, double angle) {
	double ix = v1->x, iy = v1->y;
	v2->x = ix * cos(angle) - iy * sin(angle);
	v2->y = ix * sin(angle) + iy * cos(angle);
}

bool is_solid(double x, double y) {
	int mx = (int)x;
	int my = (int)y;

	if (mx < WORLD_X && my < WORLD_Y)
		return !!world[my][mx];

	return true;
}

void draw_slice(int i, double dist, int side, int colour) {
	static SDL_Rect slice = {0, 0, 1, 1};
	static Uint8 colours[4][4] = {
		{0, 0, 0, 0},
		{100, 220, 50, SDL_ALPHA_OPAQUE},
		{220, 100, 50, SDL_ALPHA_OPAQUE},
		{100, 50, 220, SDL_ALPHA_OPAQUE},
	};

	Uint8 r = colours[colour][0], g = colours[colour][1], b = colours[colour][2], a = colours[colour][3];
	double shade = 100.0;

	if (dist > MAXDIST)
		return;

	if (side) {
		r *= 0.8;
		g *= 0.8;
		b *= 0.8;
	}

	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	slice.x = i;
	slice.h = SCREEN_HEIGHT * (1 - (dist - 1) / (float)MAXDIST) * 1.1;
	slice.y = (SCREEN_HEIGHT - slice.h) / 2;

	SDL_RenderFillRect(renderer, &slice);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, shade * (dist / MAXDIST));
	SDL_RenderFillRect(renderer, &slice);
}

int main(int argc, char *argv[]) {
	bool initialized = init();

	if (!initialized) {
		std::cerr << "SDL failed to initialize" << std::endl;
		close();
		return 1;
	}

	bool die = false;
	SDL_Event ev;

	Vector ppos = {10.0, 2.5}, rpos, mpos;
	Vector pdir = {0, 1.0}, rdir; // pdir: player direction, rdir: ray direction

	Vector next, step, rdelta;
	double dist;
	double anglestep = FOV / SCREEN_WIDTH;
	int i, side = 0;
	double vrotate, vmove;
	const Uint8 *keys;

	while (!die) {
		last_frame_time = frame_time;
		frame_time = clock();
		vrotate = (double)(frame_time - last_frame_time) / CLOCKS_PER_SEC * ROTATE_VEL;
		vmove   = (double)(frame_time - last_frame_time) / CLOCKS_PER_SEC * MOVE_VEL;

		keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
			if (!is_solid(ppos.x + pdir.x * vmove, ppos.y))
				ppos.x += pdir.x * vmove;

			if (!is_solid(ppos.x, ppos.y + pdir.y * vmove))
				ppos.y += pdir.y * vmove;
		}

		if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
			if (!is_solid(ppos.x - pdir.x * vmove, ppos.y))
				ppos.x -= pdir.x * vmove;

			if (!is_solid(ppos.x, ppos.y - pdir.y * vmove))
				ppos.y -= pdir.y * vmove;
		}

		if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
			rotate(&pdir, &pdir, vrotate);

		if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
			rotate(&pdir, &pdir, -vrotate);

		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
				die = true;

			if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN) {
				std::cout << "Position: (" << ppos.x << ", " << ppos.y << ")" << std::endl;
				std::cout << "Direction: (" << pdir.x << ", " << pdir.y << ")" << std::endl;
			}
		}

		draw_bg();

		rotate(&pdir, &rdir, FOV / 2);

		for (i = 0; i < SCREEN_WIDTH; i++) {
			rpos.x = (int)ppos.x;
			rpos.y = (int)ppos.y;

			rdelta.x = sqrt((rdir.y / rdir.x) * (rdir.y / rdir.x) + 1);
			rdelta.y = sqrt((rdir.x / rdir.y) * (rdir.x / rdir.y) + 1);

			if (rdir.x > 0) {
				step.x = 1;
				next.x = rdelta.x * ((int)(ppos.x + 1) - ppos.x);
			} else {
				step.x = -1;
				next.x = rdelta.x * (ppos.x - (int)(ppos.x));
			}

			if (rdir.y > 0) {
				step.y = 1;
				next.y = rdelta.y * ((int)(ppos.y + 1) - ppos.y);
			} else {
				step.y = -1;
				next.y = rdelta.y * (ppos.y - (int)(ppos.y));
			}

			while (!is_solid(rpos.x, rpos.y)) {
				if (next.x < next.y) {
					next.x += rdelta.x;
					rpos.x += step.x;
					side = 0;
				} else {
					next.y += rdelta.y;
					rpos.y += step.y;
					side = 1;
				}
			}

			mpos.x = rpos.x;
			mpos.y = rpos.y;

			if (side)
				rpos.x = ppos.x + rdir.x * (rpos.y - ppos.y) / rdir.y;
			else
				rpos.y = ppos.y + rdir.y * (rpos.x - ppos.x) / rdir.x;
			
			dist = sqrt((rpos.x - ppos.x) * (rpos.x - ppos.x) + (rpos.y - ppos.y) * (rpos.y - ppos.y));

			draw_slice(i, dist, side, world[(int)mpos.y][(int)mpos.x]);

			rotate(&rdir, &rdir, -anglestep);
		}

		if (is_solid(ppos.x, ppos.y))
			std::cout << "Stuck in solid (" << (int)ppos.x << ", " << (int)ppos.y << ")" << std::endl;

		SDL_UpdateWindowSurface(window);
		last_frame_time = clock();
	}

	close();

	return 0;
}