#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "logging.h"

#define WIDTH 800
#define HEIGHT 600
#define BPP 32
#define FPS 60
#define RUNNING 1
#define QUIT 0

#define GAMESTATE_MAIN_MENU 1
#define GAMESTATE_CHOOSE_LEVEL_MENU 2
#define GAMESTATE_LEVEL_CHOSEN_MENU 3
#define GAMESTATE_GAME_RUN 4
#define GAMESTATE_GAME_END 5 
#define GAMESTATE_HIGHSCORE_BEFORE_GAME 6
#define GAMESTATE_HIGHSCORE_AFTER_GAME 7 //the difference is that player can type name 


// Font stuff
TTF_Font *font = NULL;
SDL_Color textcolor = {255, 255, 255};

// Logfile
FILE *logfile = NULL;

// SDL Event handler
SDL_Event event;

// Game status
int status; // status 0 = game not running, status 1 = game running
int GAME_STATE;

void applysurface(int x, int y, SDL_Surface *src, SDL_Surface *dst, SDL_Rect *clip);

int main(int argc, char *argv[])
{  
    int x = 30;
    int y = 30;
    SDL_Surface *background = NULL;
    SDL_Surface *screen = NULL;
    SDL_Surface *message = NULL;
    
	logfile = fopen("log.txt", "w");
	if (logfile == 0) {
		fprintf(stderr, "ERROR: Could not open logfile for writing.\n");
	}
    
    fprintf(logfile, "INFO: Attempting to initialise everything.\n");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(logfile, "ERROR: Initialisation failed.\n");
		return 1;
	}    
	if (TTF_Init() == -1) {
		return 1;
	}    
    
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP, SDL_HWSURFACE|SDL_RESIZABLE);
	if (screen == 0) {
		fprintf(logfile, "ERROR: Failed to set video mode (SDL_SetVideoMode())\n");
		return 1;
	}
    
    if ((font = TTF_OpenFont("ARCADE_N.TTF", 18)) == NULL) {
    	return 1;
    }

	fprintf(logfile, "INFO: Everything has been initialised.\n");
	SDL_WM_SetCaption("typerbawz", NULL);
	fprintf(logfile, "INFO: Loaded SDL with flags = %d, width = %d, height = %d\n", screen->flags, screen->w, screen->h);
	message = TTF_RenderText_Solid(font, "Welchom", textcolor);
	if (message == NULL) {
		return 1;
	}
	status = RUNNING;
	while (status) {
		// EVENTS
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				status = QUIT;
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: status = QUIT; break;
				}
			}
		}
        //try commenting this and see the mess 
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
        
		applysurface(x, y, message, screen, NULL);
        x += 1;
        if (x > 600){
          x = 50;
        }
		// RENDERING
		if (SDL_Flip(screen) == -1) {
			return 1;
		}
	}	

	SDL_FreeSurface(background);
	TTF_CloseFont(font);
	TTF_Quit();
	fclose(logfile);
	SDL_Quit();

	return 0;
	// End of main()
}

void applysurface(int x, int y, SDL_Surface *src, SDL_Surface *dst, SDL_Rect *clip) {
	// Make a temporary rectangle to hold the offsets
	SDL_Rect offset;

	// Give the offsets to the rectangle
	offset.x = x;
	offset.y = y;

	// Blit the surface
	SDL_BlitSurface(src, clip, dst, &offset);
}


