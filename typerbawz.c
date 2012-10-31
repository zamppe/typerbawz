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
    srand(time(NULL)); //init rand seed
  
    int i, j, r;    //index and rand
    
    double x, y; //temporary double values
    
    int thisframe, prevframe; //time calculation
    double dt;    // delta time
    
    SDL_Surface *background = NULL;
    SDL_Surface *screen = NULL; //main surface
    SDL_Surface *typefieldsurface = NULL; 
    
    int fontsize = WIDTH / 64; //1024/64=16, 1280/64=20, 1600/64=25, 1920/64=30
    
    
    logfile = fopen("log.txt", "w");
	if (logfile == 0) {
		fprintf(stderr, "ERROR: Could not open logfile for writing.\n");
	}
    
    Words words; // floating strings on the screen
    Wordpool wordpool; // the pool of strings from which strings are randomly selected

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

    //main game variables for difficulty tuning!!
    int addv = 100; //every 1 seconds add velocity on wordsa
    int addvcounter = 0;
    double amountv = -1.0;
    
    int adda = 200; //every 20 seconds     
    int addacounter = 0;
    int amounta = 1;
    
    double initv = -10.0;
    int initamount = 3;
    
    
    initWords(&words, initamount);  // start with initamount words on the screen
    initWordpool(&wordpool, 100); // start with 100 strings of space, more space will be allocated automatically when needed  
    
    //fill wordpool with content
    FILE * pFile;
    char baffer[30];
    pFile = fopen ("wordsEn.txt" , "r");
    if (pFile == NULL) {
        perror ("Error opening file");
    } 
    else {
        while ( fgets (baffer , 30 , pFile) != NULL) {                                                    
            pushIntoWordpool(&wordpool, baffer);
        }
    fclose (pFile);
    }     
    //initial level
    for(i = 0; i < initamount; i++) {
        r = ( rand()+rand()+rand()+rand() ) % wordpool.used;            
        x = (double) (WIDTH - strlen(wordpool.strings[r]) * fontsize);
        y = HEIGHT / (i + 2);
        pushIntoWords ( &words, (Word) {x, y, initv, 0.0, 0.0, 0.0, wordpool.strings[r], NULL}, fontsize );
    }
    
    
    //initialize text surfaces
    for(i = 0; i < words.used; i++){     
        words.array[i].surface = TTF_RenderText_Solid(font, words.array[i].string, textcolor);
    }
    
    
	status = RUNNING;
    char typefield[20];
    for (i = 0; i < 20; i++){
        typefield[i] = 0;
    }
    thisframe = SDL_GetTicks();
	while (status) {
        prevframe = thisframe;
        thisframe = SDL_GetTicks();
        dt = ((double) (thisframe - prevframe)) / 1000;
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
                //fprintf(logfile, "%d\n", event.key.keysym.sym);
                //type chars
                if(event.key.keysym.sym > 96 && event.key.keysym.sym < 123 || event.key.keysym.sym == 39 ){     
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
                //enter || space
                else if(event.key.keysym.sym == 13 || event.key.keysym.sym == 32){                
                    //do teh comparison
                    if ((i = stringMatchesWords(&words, typefield)) != -1){                        
                        //there has been a match

                        //randomize the index for pulling a new string from the wordpool
                        r = ( rand()+rand()+rand()+rand() ) % wordpool.used;                                              
                        setString(&words.array[i], wordpool.strings[r]); 
                        
                        x = WIDTH - strlen(wordpool.strings[r]) * fontsize;
                        y = rand() % (HEIGHT - fontsize*2) + fontsize;
                        
                        setPosition( &words.array[i], x, y );                                                
                        
                        SDL_FreeSurface(words.array[i].surface);
                        words.array[i].surface = TTF_RenderText_Solid(font, words.array[i].string, textcolor);                        
                    }
                    //fprintf(logfile, "%d\n", i);
                    //
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
        
        updatePositions( &words, dt );
        //
        for(i = 0; i < words.used; i++) {  
            if( words.array[i].x < 0 ) {
                r = ( rand()+rand()+rand()+rand() ) % wordpool.used;                                              
                setString(&words.array[i], wordpool.strings[r]); 
                
                x = WIDTH - strlen(wordpool.strings[r]) * fontsize;
                y = rand() % (HEIGHT - fontsize*2) + fontsize;
                
                setPosition( &words.array[i], x, y );                                                
                
                SDL_FreeSurface(words.array[i].surface);
                words.array[i].surface = TTF_RenderText_Solid(font, words.array[i].string, textcolor);          
            }
        }
        
        //need to add?
        addvcounter += (thisframe - prevframe);
        addacounter += (thisframe - prevframe);
        
        if (addvcounter > addv) {
            addvcounter = 0;
            //when one has to add velocity, one has to add velocity
            for(i = 0; i < words.used; i++) {  
                addVelocity(&words.array[i], amountv, 0.0);
            }
        }
        
        if (addacounter > adda) {
            addacounter = 0;
            r = ( rand()+rand()+rand()+rand() ) % wordpool.used;            
            x = (double) (WIDTH - strlen(wordpool.strings[r]) * fontsize);
            y = rand() % (HEIGHT - fontsize*2) + fontsize;
            pushIntoWords ( &words, (Word) {x, y, words.array[0].vx, 0.0, 0.0, 0.0, wordpool.strings[r], NULL}, fontsize ); 
            words.array[words.used-1].surface = TTF_RenderText_Solid(font, words.array[words.used-1].string, textcolor);            
        }
        
        //collisions a     a
        for(i = 0; i < words.used - 1; i++) {
            for(j = i+1; j < words.used; j++) {           
                if ( wordsCollide(&words.array[i], &words.array[j]) ) {
                    fprintf(logfile, "time: %dms: %s %s %s\n", thisframe, "coll coll coll!!!", words.array[i].string, words.array[j].string);
                    r = 1;
                    //ok there was a collision, keep moving one of the words until no more collision between the two
                    while ( wordsCollide(&words.array[i], &words.array[j]) ) {                                           
                        //the correct word to move should be the one with smaller distance from right edge of the screen
                        if ( WIDTH - (words.array[j].x + words.array[j].w) < WIDTH - (words.array[i].x + words.array[i].w) ) {
                            words.array[j].y += 10; 
                            if ( (words.array[j].y + words.array[j].h) > HEIGHT) {
                                //if word was moved outside the screen put it to the top
                                words.array[j].y = 0;
                            }
                        }
                        else {
                            words.array[i].y += 10; 
                            if ( (words.array[i].y + words.array[i].h) > HEIGHT ) {
                                words.array[i].y = 0;
                            }
                        }                        
                        r++;
                    }    
                    fprintf(logfile, "solved collision between %s and %s, took %d rounds\n", words.array[i].string, words.array[j].string, r);
                    
                }
                else{
                    ;
                    /*
                    fprintf(logfile, "%d %d %d %d %f %d\n", 
                    words.array[i].x < (words.array[j].x + words.array[j].w),
                    words.array[i].x + words.array[i].w > words.array[j].x,
                    words.array[i].y < (words.array[j].y + words.array[j].h),
                    words.array[i].y + words.array[i].h > words.array[j].y),
                    words.array[i].x,
                    words.array[i].w;
                    */
                }
            }
        }
        
               
        
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
    SDL_FreeSurface(typefield);
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


