#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "logging.h"
#include "word.h"
#include <time.h>
#include <stdlib.h>





#define WIDTH 1024
#define HEIGHT 800
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
    srand(time(NULL));
    
    int i, r;    
    SDL_Surface *background = NULL;
    SDL_Surface *screen = NULL;
    
    Words words; // floating strings on the screen
    Wordpool wordpool; // the pool of strings from which floating strings are randomly selected
    initWords(&words, 3);  // start with 3 strings on the screen
    initWordpool(&wordpool, 100); // start with 100 strings of space, more space will be allocated automatically when needed

    pushIntoWordpool(&wordpool, "manually");
    pushIntoWordpool(&wordpool, "pushing");
    pushIntoWordpool(&wordpool, "some");
    pushIntoWordpool(&wordpool, "press q to pull random word from wordpool");
    pushIntoWordpool(&wordpool, "to");
    pushIntoWordpool(&wordpool, "this");
    pushIntoWordpool(&wordpool, "pool");
    pushIntoWordpool(&wordpool, "for");
    pushIntoWordpool(&wordpool, "little");
    pushIntoWordpool(&wordpool, "testing");
    
    //                                  word moves 50 pixel / second to "left" once we have timing code
    pushIntoWords ( &words, (Word) {500.0, 100.0, -10.0, 0.0, wordpool.strings[7], NULL} );
    pushIntoWords ( &words, (Word) {500.0, 200.0, -50.0, 0.0, wordpool.strings[6], NULL} );
    pushIntoWords ( &words, (Word) {300.0, 300.0, -1.0 , 0.0, wordpool.strings[3], NULL} );  


    
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
    
    if ((font = TTF_OpenFont("ARCADE_N.TTF", 16)) == NULL) {
    	return 1;
    }

	fprintf(logfile, "INFO: Everything has been initialised.\n");
	SDL_WM_SetCaption("typerbawz", NULL);
	
    fprintf(logfile, "INFO: Loaded SDL with flags = %d, width = %d, height = %d\n", screen->flags, screen->w, screen->h); 

    //initialize texts
    for(i = 0; i < words.used; i++){     
        words.array[i].surface = TTF_RenderText_Solid(font, words.array[i].string, textcolor);
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
                    case SDLK_q:
                        r = rand()%10;
                        SDL_FreeSurface(words.array[0].surface); 
                        setString(&words.array[0], wordpool.strings[r]); 
                        words.array[0].surface = TTF_RenderText_Solid(font, words.array[0].string, textcolor);
				}
			} 
		}
        /*update */
        updatePositions( &words, 0.01 );
        
        
        /* render */
        //try commenting this and see the mess 
        
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );

        for(i = 0; i < words.used; i++){        
            applysurface(words.array[i].x, words.array[i].y, words.array[i].surface, screen, NULL);
        }
        
        applysurface(50, 50, background, screen, NULL);

		// RENDERING
		if (SDL_Flip(screen) == -1) {
			return 1;
		}
	}	
    
	SDL_FreeSurface(background);
	TTF_CloseFont(font);
	TTF_Quit();
	fclose(logfile);
    freeWords(&words);
    freeWordpool(&wordpool);
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


