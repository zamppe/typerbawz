#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "logging.h"
#include "word.h"
#include "level.h"
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>





#define WIDTH 1024
#define HEIGHT 800
#define TOPMARGIN 50
#define BOTTOMMARGIN 50
#define BPP 32
#define FPS 60
#define RUNNING 1
#define QUIT 0

#define GAMESTATE_MAINMENU 1
#define GAMESTATE_CHOOSELEVELMENU 2
#define GAMESTATE_LEVELCHOSENMENU 3
#define GAMESTATE_GAMERUN 4
#define GAMESTATE_GAMEEND 5 
#define GAMESTATE_HIGHSCOREBEFORE 6
#define GAMESTATE_HIGHSCOREAFTER 7 //the difference is that player can type name 

typedef struct item{
    int x, y, fontsize, index;
    char *string;
    struct SDL_SURFACE *surface;
}Item;

typedef struct items {
    Item *array;
    size_t used;
    size_t size;
    int max;
    int x, y, selected;
}Items;

void initItems ( Items *i, size_t initialSize, int x, int y ) {
    i->array = (Item *)malloc(initialSize * sizeof(Item));
    i->used = 0;
    i->size = initialSize;
    i->max = 20;
    //position of the 'items container'
    i->x = x; 
    i->y = y;
}

int pushIntoItems ( Items *i, char *text, int fontsize ) {

    if ( i->used == i->max ){
        return 0;
    }
    if ( i->used == i->size ) {
        i->size += 1;
        i->array = (Item *)realloc(i->array, i->size * sizeof(Item));
    }
    int x = i->x - (strlen(text)*fontsize);
    int y = i->y + (i->used * fontsize);
    Item item = {x, y, fontsize, i->used, text, NULL};
    i->array[i->used++] = item;
    i->selected = 0;
    return 1;
}

void freeItems ( Items *i ) {
    free(i->array);
    i->array = NULL;
    i->used = i->size = 0;
}

void nextItem ( Items *i ) {
    i->selected++;
    if (i->selected >= i->used) {
        i->selected = 0;
    }
}

void prevItem ( Items *i ) {
    i->selected--;
    if (i->selected < 0) {
        i->selected = i->used - 1;
    }

}

Item getSelectedItem(Items *i) {
    return i->array[i->selected];
}
//http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void applysurface(int x, int y, SDL_Surface *src, SDL_Surface *dst, SDL_Rect *clip);

int main(int argc, char *argv[])
{    
    TTF_Font *font = NULL;
    TTF_Font *titlefont = NULL;
    SDL_Color textcolor = {255, 255, 255};
    SDL_Color titlecolor = {0, 255, 0};
    SDL_Color selectedtextcolor = {0, 255, 0};


    FILE *logfile = NULL;

    SDL_Event event;
    int kbstate[255];    
    // Game status
    int status; // application 'master' status 0 = not running, status 1 = running
    int GAMESTATE; // state of the game for different types of I/O handling
    
    srand(time(NULL)); //init rand seed
  
    int i, j, r;    //index and rand
    
    double x, y; //temporary double values
    
    int thisframe, prevframe; //time calculation
    double dt;    // delta time
    
    SDL_Surface *screen = NULL; //main surface
    SDL_Surface *mainmenutitle = NULL;
    SDL_Surface *mainmenu0surface = NULL;    
    SDL_Surface *mainmenu1surface = NULL;    
    SDL_Surface *selectedlevelsurface = NULL;    
    SDL_Surface *gamearea = NULL;
    SDL_Surface *typefieldsurface = NULL; 
    
    int fontsize = WIDTH / 64; //1024/64=16, 1280/64=20, 1600/64=25, 1920/64=30
    int titlefontsize = WIDTH / 10;
    
    /* menu text */
    char *title = "TYPERBAWZ";
    char *mainmenu0text = "Play";
    char *mainmenu1text = "Quit";
    int mainmenuselecteditem = 0;
    
    Item selectedlevel;
    Items items;
    initItems(&items, 0, WIDTH / 2, HEIGHT / 2);
    DIR *dp;
    struct dirent *ep;     
    dp = opendir ("./levels");

    if (dp != NULL) {
        while (ep = readdir (dp)) {
            if ( !(strcmp(get_filename_ext(ep->d_name), "txt")) ) {
                pushIntoItems(&items, strdup(ep->d_name), fontsize);
            }
        }
        (void) closedir (dp);
    }
    /*
    pushIntoItems(&items, "perkele", fontsize);
    pushIntoItems(&items, "saatana", fontsize);
    pushIntoItems(&items, "helvetti :D", fontsize);
    */

    
    logfile = fopen("log.txt", "w");
	if (logfile == 0) {
		fprintf(stderr, "ERROR: Could not open logfile for writing.\n");
	}
    
    Words words; // floating strings on the screen
    Wordpool wordpool; // the pool of strings from which strings are randomly selected
    initWordpool(&wordpool, 100); // start with 100 strings of space, more space will be allocated automatically when needed   
    fprintf(logfile, "INFO: Attempting to initialise everything.\n");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(logfile, "ERROR: Initialisation failed.\n");
		return 1;
	}    
	if (TTF_Init() == -1) {
		return 1;
	}    
    //
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP, SDL_HWSURFACE);
	if (screen == 0) {
		fprintf(logfile, "ERROR: Failed to set video mode (SDL_SetVideoMode())\n");
		return 1;
	}
    
    if ((font = TTF_OpenFont("ARCADE_N.TTF", fontsize)) == NULL) {
    	return 1;
    }
    
    if ((titlefont = TTF_OpenFont("ARCADE_N.TTF", titlefontsize)) == NULL) {
    	return 1;
    }    
    
    mainmenutitle = TTF_RenderText_Solid(titlefont, title, titlecolor); 

	fprintf(logfile, "INFO: Everything has been initialised.\n");
	SDL_WM_SetCaption("typerbawz", NULL);

    fprintf(logfile, "INFO: Loaded SDL with flags = %d, width = %d, height = %d\n", screen->flags, screen->w, screen->h); 

    //main game variables for difficulty tuning!!
    int addv; 
    int addvcounter;
    double amountv;
    
    int adda;     
    int addacounter;
    int amounta;
    
    double initv;
    int initamount;

    
	status = RUNNING;
    GAMESTATE = GAMESTATE_MAINMENU;
    char typefield[30];
    for (i = 0; i < 30; i++){
        typefield[i] = 0;
    }
    for (i = 0; i < 255; i++){
        kbstate[i] = 0;
    }
    
    thisframe = SDL_GetTicks();
	while (status) {
        prevframe = thisframe;
        thisframe = SDL_GetTicks();
        dt = ( (double) (thisframe - prevframe) ) / 1000;
        /*********
		* EVENTS *
        *********/
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				status = QUIT;
			}
            if (GAMESTATE != GAMESTATE_GAMERUN) {
                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {                          
                        case SDLK_UP: kbstate[0] = 1;break;
                        case SDLK_DOWN: kbstate[1] = 1;break;     
                        case SDLK_RETURN: kbstate[2] = 1;break;
                        case SDLK_ESCAPE: kbstate[3] = 1;break;
                    }      
                            
                }
                /* on a 2nd thought this would be bad. before keyup is detected one esc from any point could be 10+ esces etc
                else if (event.type == SDL_KEYUP) {
                    switch (event.key.keysym.sym) {
                        case SDLK_UP: kbstate[0] = 0;break;
                        case SDLK_DOWN: kbstate[1] = 0;break;       
                        case SDLK_RETURN: kbstate[2] = 0;break;
                        case SDLK_ESCAPE: kbstate[3] = 0;break;
                    }
                } 
                */
            }
            else if (GAMESTATE == GAMESTATE_GAMERUN) {

                if (event.type == SDL_KEYDOWN) {
                    //fprintf(logfile, "%d\n", event.key.keysym.sym); 
                    
                    //esced
                    if(event.key.keysym.sym == 27) {
                        GAMESTATE = GAMESTATE_MAINMENU;
                        break;
                    }
                    //type chars
                    else if(event.key.keysym.sym > 96 && event.key.keysym.sym < 123 || event.key.keysym.sym == 39 ){     
                        //find first empty character
                        for (i = 0; i < 29; i++){
                            if(typefield[i] == 0){
                                typefield[i] = event.key.keysym.sym;                 
                                break;
                            }
                        }
                    }
                    //backspace
                    else if(event.key.keysym.sym == 8){
                        //find last nonempty character
                        for(i = 28; i >= 0; i--){
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
                            
                            setString(&words.array[i], wordpool.strings[r], fontsize); 
                            
                            x = WIDTH - strlen(wordpool.strings[r]) * fontsize;
                            y = rand() % (HEIGHT - (BOTTOMMARGIN + fontsize)*2) + fontsize + TOPMARGIN;
                            
                            setPosition( &words.array[i], x, y );                                                
                            
                            SDL_FreeSurface(words.array[i].surface);
                            words.array[i].surface = TTF_RenderText_Solid(font, words.array[i].string, textcolor);                        
                        }
                        
                        //
                        //clear typefield
                        for (i = 0; i < 30; i++){
                            typefield[i] = 0;
                        }
                    }
                }
            }
		}
        
        /*********
		* UPDATE *
        *********/
        if (GAMESTATE == GAMESTATE_MAINMENU) {
            SDL_FreeSurface( mainmenu0surface ); 
            SDL_FreeSurface( mainmenu1surface ); 
            mainmenu0surface = TTF_RenderText_Solid(font, mainmenu0text, textcolor); 
            mainmenu1surface = TTF_RenderText_Solid(font, mainmenu1text, textcolor); 
            if (mainmenuselecteditem == 0) {
                SDL_FreeSurface( mainmenu0surface ); 
                mainmenu0surface = TTF_RenderText_Solid(font, mainmenu0text, selectedtextcolor); 
            }
            else if (mainmenuselecteditem == 1) {
                SDL_FreeSurface( mainmenu1surface ); 
                mainmenu1surface = TTF_RenderText_Solid(font, mainmenu1text, selectedtextcolor); 
            }            
            //fprintf(logfile, "%d\n", wordpool.used);
            if (kbstate[3] == 1) {
                //fprintf(logfile, "%d\n", wordpool.used);
                status = QUIT; 
                break;
            }
            if (kbstate[0] == 1) {
                kbstate[0] = 0;
                if (mainmenuselecteditem == 0) {
                    mainmenuselecteditem = 1;
                }
                else if (mainmenuselecteditem == 1) {
                    mainmenuselecteditem = 0;
                }
            }
            if (kbstate[1] == 1) {
                kbstate[1] = 0;
                if (mainmenuselecteditem == 1) {
                    mainmenuselecteditem = 0;
                }
                else if (mainmenuselecteditem == 0) {
                    mainmenuselecteditem = 1;
                }                
            }
            if (kbstate[2] == 1) {
                kbstate[2] = 0;
                if (mainmenuselecteditem == 0) {
                    GAMESTATE = GAMESTATE_CHOOSELEVELMENU;
                }
                else if (mainmenuselecteditem == 1) {
                    status = QUIT; 
                    break;
                }
            }
        }
        else if (GAMESTATE == GAMESTATE_CHOOSELEVELMENU) {
            if (kbstate[3] == 1) {
                //fprintf(logfile, "%d\n", wordpool.used);
                kbstate[3] = 0;
                GAMESTATE = GAMESTATE_MAINMENU;
            }
            if (kbstate[0] == 1) {
                kbstate[0] = 0;
                prevItem(&items);             
            }
            if (kbstate[1] == 1) {
                kbstate[1] = 0;
                nextItem(&items);              
            }
            if (kbstate[2] == 1) {
                kbstate[2] = 0;                
                //main game variables for difficulty tuning!!
                addv = 1000; //every 1 seconds add velocity on wordsa
                addvcounter = 0;
                amountv = -1.0;
                
                adda = 20000; //every 20 seconds     
                addacounter = 0;
                amounta = 1;
                
                initv = -10.0;
                initamount = 3;
                                
                freeWords(&words);
                freeWordpool(&wordpool);
                initWords(&words, initamount);  // start with initamount words on the screen
                initWordpool(&wordpool, 100); // start with 100 strings of space, more space will be allocated automatically when needed                
                loadLevel(&wordpool, selectedlevel.string);
                //initial level
                for(i = 0; i < initamount; i++) {
                    r = ( rand()+rand()+rand()+rand() ) % wordpool.used;            
                    x = (double) (WIDTH - strlen(wordpool.strings[r]) * fontsize);
                    y = HEIGHT / (i + 2);
                    pushIntoWords ( &words, (Word) {x, y, initv, 0.0, 0.0, 0.0, wordpool.strings[r], NULL}, fontsize );
                }
                //
                
                //initialize text surfaces
                for(i = 0; i < words.used; i++){     
                    words.array[i].surface = TTF_RenderText_Solid(font, words.array[i].string, textcolor);
                }
                
                GAMESTATE = GAMESTATE_GAMERUN;
            }    
        }
        else if (GAMESTATE == GAMESTATE_GAMERUN) {        
            updatePositions( &words, dt );
            //
            for(i = 0; i < words.used; i++) {  
                if( words.array[i].x < 0 ) {
                    r = ( rand()+rand()+rand()+rand() ) % wordpool.used;                                              
                    setString(&words.array[i], wordpool.strings[r], fontsize); 
                    
                    x = WIDTH - strlen(wordpool.strings[r]) * fontsize;
                    y = rand() % (HEIGHT - (BOTTOMMARGIN + fontsize)*2) + fontsize + TOPMARGIN;
                    
                    setPosition( &words.array[i], x, y );                                                
                    //
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
                y = rand() % (HEIGHT - (BOTTOMMARGIN + fontsize) * 2) + fontsize + TOPMARGIN;
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
                                if ( (words.array[i].y + words.array[i].h) > HEIGHT - BOTTOMMARGIN) {
                                    words.array[i].y = TOPMARGIN;
                                }
                            }                        
                            r++;
                        }    
                        fprintf(logfile, "solved collision between %s and %s, took %d rounds\n", words.array[i].string, words.array[j].string, r);
                        
                    }
                }
            }
        }
        
               
        
        /* render */
        
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
        if (GAMESTATE != GAMESTATE_GAMERUN){
            //render title if not ingame
            applysurface(WIDTH / 2 - strlen(title) * titlefontsize / 2, HEIGHT/10, mainmenutitle, screen, NULL);
        }
        
        if (GAMESTATE == GAMESTATE_MAINMENU) {
            
            applysurface(WIDTH / 2 - strlen(mainmenu0text) * fontsize / 2, HEIGHT/2, mainmenu0surface, screen, NULL);
            applysurface(WIDTH / 2 - strlen(mainmenu1text) * fontsize / 2, HEIGHT/2+fontsize+5, mainmenu1surface, screen, NULL);
        }
        else if (GAMESTATE == GAMESTATE_CHOOSELEVELMENU) {
            for ( i = 0; i < items.used; i++ ) {                
                SDL_FreeSurface(items.array[i].surface);
                items.array[i].surface = TTF_RenderText_Solid(font, items.array[i].string , textcolor); 
                applysurface(items.array[i].x, items.array[i].y, items.array[i].surface, screen, NULL);                        
            }
            selectedlevel = getSelectedItem(&items);
            SDL_FreeSurface(selectedlevel.surface);
            selectedlevel.surface = TTF_RenderText_Solid(font, selectedlevel.string , selectedtextcolor); 
            applysurface(selectedlevel.x, selectedlevel.y, selectedlevel.surface, screen, NULL);                  
        }
        else if (GAMESTATE == GAMESTATE_GAMERUN) {
            for(i = 0; i < words.used; i++){        
                applysurface(words.array[i].x, words.array[i].y, words.array[i].surface, screen, NULL);
            }
            
            //applysurface(0, TOPMARGIN, gamearea, screen, NULL);
            SDL_FreeSurface( typefieldsurface ); 
            typefieldsurface = TTF_RenderText_Solid(font, typefield, textcolor);     
            applysurface(WIDTH/2-200, HEIGHT-30, typefieldsurface, screen, NULL);	   
        }
        
		if (SDL_Flip(screen) == -1) {
			return 1;
		}
	}	
    SDL_FreeSurface(gamearea);
    SDL_FreeSurface(mainmenutitle);
    SDL_FreeSurface(typefieldsurface);
    SDL_FreeSurface(selectedlevelsurface);
    SDL_FreeSurface(mainmenu0surface);
    SDL_FreeSurface(mainmenu1surface);
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



