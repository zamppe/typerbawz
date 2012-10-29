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
    SDL_Surface *typefieldsurface = NULL;
    
    Words words; // floating strings on the screen
    Wordpool wordpool; // the pool of strings from which floating strings are randomly selected
    initWords(&words, 3);  // start with 3 strings on the screen
    initWordpool(&wordpool, 100); // start with 100 strings of space, more space will be allocated automatically when needed

    pushIntoWordpool(&wordpool, "try");
    pushIntoWordpool(&wordpool, "to");
    pushIntoWordpool(&wordpool, "type");
    pushIntoWordpool(&wordpool, "press");
    pushIntoWordpool(&wordpool, "shittink");
    pushIntoWordpool(&wordpool, "this");
    pushIntoWordpool(&wordpool, "pool");
    pushIntoWordpool(&wordpool, "for");
    pushIntoWordpool(&wordpool, "little");
    pushIntoWordpool(&wordpool, "testing");
    
    //                                  word moves 10 pixel / second to "left"
    pushIntoWords ( &words, (Word) {500.0, 100.0, -10.0, 0.0, wordpool.strings[0], NULL} );
    pushIntoWords ( &words, (Word) {500.0, 200.0, -50.0, 0.0, wordpool.strings[1], NULL} );
    pushIntoWords ( &words, (Word) {300.0, 300.0, -1.0 , 0.0, wordpool.strings[2], NULL} );  


    
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
    char typefield[20];
    for (i = 0; i < 20; i++){
        typefield[i] = 0;
    }
	while (status) {
		// EVENTS
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				status = QUIT;
			}
            /*
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: status = QUIT; break;
                    case SDLK_q:
                        r = rand()%10;
                        SDL_FreeSurface(words.array[0].surface); 
                        //setString(&words.array[0], wordpool.strings[r]); 
                        setString(&words.array[0], "q is down"); 
                        words.array[0].surface = TTF_RenderText_Solid(font, words.array[0].string, textcolor);
				}
			}
            */ 
            if (event.type == SDL_KEYDOWN) {
                //type chars
                if(event.key.keysym.sym > 96 && event.key.keysym.sym < 123){     
                    //find first empty character
                    for (i = 0; i < 19; i++){
                        if(typefield[i] == 0){
                            typefield[i] = event.key.keysym.sym;                 
                            break;
                        }
                    }
                }
                //backspace
                else if(event.key.keysym.sym == 8){
                    //find last nonempty character
                    for(i = 18; i >= 0; i--){
                        if(typefield[i] != 0){
                            typefield[i] = 0;
                            break;
                        }
                    }
                }
                //enter || backspace
                else if(event.key.keysym.sym == 13 || event.key.keysym.sym == 32){                
                    //do teh comparison
                    if (i = stringMatchesWords(&words, typefield)){
                        if (i != -1){
                            //there has been a match, do some magic
                            //pull new word from word pool and put at words[i] 
                            
                            r = rand()%10;
                            setX(&words.array[i-1], 600);
                            setString(&words.array[i-1], wordpool.strings[r]); 
                            SDL_FreeSurface(words.array[i-1].surface);
                            words.array[i-1].surface = TTF_RenderText_Solid(font, words.array[i-1].string, textcolor);
                        }
                    }
                    //fprintf(logfile, "%d\n", i);
                    
                    //clear typefield
                    for (i = 0; i < 20; i++){
                        typefield[i] = 0;
                    }
                }
                //sprintf(typefield, "%s", SDL_GetKeyName(event.key.keysym.sym));             
                //setString(&words.array[0], (char*) typefield); 
                //words.array[0].surface = TTF_RenderText_Solid(font, words.array[0].string, textcolor);
                //SDL_FreeSurface(words.array[0].surface); 
                //words.array[0].surface = TTF_RenderText_Solid(font, typefield, textcolor);
            }
            /*
            else if (event.type == SDL_KEYUP) {
            	switch (event.key.keysym.sym) {
					case SDLK_q:
                        SDL_FreeSurface(words.array[0].surface); 
                        setString(&words.array[0], "q is up"); 
                        words.array[0].surface = TTF_RenderText_Solid(font, words.array[0].string, textcolor);
                }
            }
            */
		}
        /*update */
        updatePositions( &words, 0.01 );
               
        
        /* render */
        
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );

        for(i = 0; i < words.used; i++){        
            applysurface(words.array[i].x, words.array[i].y, words.array[i].surface, screen, NULL);
        }
        
        applysurface(50, 50, background, screen, NULL);

        SDL_FreeSurface( typefieldsurface ); 
        typefieldsurface = TTF_RenderText_Solid(font, typefield, textcolor);     
        applysurface(WIDTH/2, HEIGHT-30, typefieldsurface, screen, NULL);	    
        
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


