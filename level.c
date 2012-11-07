#include "level.h"
#include "word.h"

void initLevel ( Level *level, int width, int fontsize) {
    initWords(&level->words, initamount);  // start with initamount words on the screen
    initWordpool(&level->wordpool, 100); // start with 100 strings of space, more space will be allocated automatically when needed 
    level->wordindex = 0;
    level->width = width;
    level->fontsize = fontsize;
    setRules( level, 0, 0, 0.0, 0, 0, 0, 0, 0, 0.0, 0 );
}

void openLevel ( Level *level, char *filename ){
    //fill wordpool with content
    FILE *pf;
    char baffer[30];
    pFile = fopen (filename , "r");
    if (pf == NULL) {
        perror ("Error opening file");
    } 
    else {
        while ( fgets (baffer , 30 , pFile) != NULL) {                                                    
            pushIntoWordpool(&level->wordpool, baffer);
        }
    fclose (pf);
    }     
    //default rules for time being
    setRules( level, 1000, 0, -1.0, 20000, 0, 1, -10, 3, -100.0, 20 );
    setUpLevel ( level, level->initamount );
    int i;
    for(i = 0; i < level->words.used; i++){     
        level->words.array[i].surface = TTF_RenderText_Solid(font, words.array[i].string, textcolor);
    }
}

void setRules( Level *level, int asi, int asc, double asa, int awi, int awc, int awa, double inits, int inita, double maxs, int maxa ) {  
    level->addspeedinterval = asi; //time interval for speed addition
    level->addspeedcounter = asc; //time counter since last addition in ms
    level->addspeedamount = asa; //amount of speed added when interval reached
    
    level->addwordinterval = awi; //time interval for word addition
    level->addwordcounter = awc; //time counter since last addition in ms
    level->addwordamount = awa; //amount of words added when interval reached
    
    level->initspeed = inits;
    level->initamount = inita;
    
    level->maxspeed = maxs;
    level->maxamount = maxa;
}

void setUpLevel( Level *level ) {
    int i;
    for(i = 0; i < level->initamount; i++) {        
        x = (double) (level->width - strlen(level->wordpool.strings[level->wordindex]) * level->fontsize);
        y = 100 * (i + 1);
        pushIntoWords ( &level->words, (Word) {x, y, level->initspeed, 0.0, 0.0, 0.0, level->wordpool.strings[level->wordindex], NULL}, level->fontsize );
        level->wordindex++;
    }
}

void freeLevel( Level *level ) {
    freeWords(&level->words);
    freeWordpool(&level->wordpool);
}
    