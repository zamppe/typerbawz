#include "word.h"

void initWords ( Words *w, size_t initialSize ) {
    w->array = (Word *)malloc(initialSize * sizeof(Word));
    w->used = 0;
    w->size = initialSize;
    w->max = 20;
}

void freeWords ( Words *w ) {
    /*
    int i;
    for(i = 0; i < w->used; i++){     
        SDL_FreeSurface(w->array[i].surface);
        free(w->array[i].string);
        w->array[i].string = NULL;
    }
    */
    free(w->array);
    w->array = NULL;
    w->used = w->size = 0;
}

int pushIntoWords ( Words *w, Word element, int fontsize ) {
    if ( w->used == w->max ){
        return 0;
    }
    if ( w->used == w->size ) {
        w->size += 1;
        w->array = (Word *)realloc(w->array, w->size * sizeof(Word));
    }
    element.w = fontsize * strlen(element.string);
    element.h = fontsize;
    w->array[w->used++] = element;
    return 1;
}

void setString ( Word *w, char *string, int fontsize ) {
    w->string = string;
    w->w = fontsize * strlen(string);
}

void setVelocity ( Word *w, double vx, double vy ) {
    w->vx = vx;
    w->vy = vy;
}

void addVelocity ( Word *w, double dvx, double dvy ) {
    w->vx += dvx;
    w->vy += dvy;
}

void setPosition ( Word *w, double newX, double newY ) {
    w->x = newX;
    w->y = newY;
}

void setX ( Word *w, double newX ) {
    w->x = newX;
}

int wordsCollide( Word *w1, Word *w2 ) {
    
    if(    (  w1->x          < (w2->x + w2->w) ) 
        && ( (w1->x + w1->w) >  w2->x          ) 
        && (  w1->y          < (w2->y + w2->h) ) 
        && ( (w1->y + w1->h) >  w2->y          ) 
      ) { return 1; }
    

    /*
    //if topleft corner of w1 is inside area of w2, return 1
    if(    ( w1->x           < (w2->x + w2->w) ) 
        && ( w1->x           >  w2->x          ) 
        && ( w1->y           < (w2->y + w2->h) ) 
        && ( w1->y           >  w2->y          ) 
      ) { return 1; } 
      
    //if bottomleft corner of w1 is inside area of w2, return 1    
    if(    (  w1->x          < (w2->x + w2->w) ) 
        && (  w1->x          >  w2->x          ) 
        && ( (w1->y + w1->h) < (w2->y + w2->h) ) 
        && ( (w1->y + w1->h) >  w2->y          ) 
      ) { return 1; } 
      
    //if topright corner of w1 is inside area of w2, return 1    
    if(    ( (w1->x + w1->w) < (w2->x + w2->w) ) 
        && ( (w1->x + w1->w) >  w2->x          ) 
        && (  w1->y          < (w2->y + w2->h) ) 
        && (  w1->y          >  w2->y          ) 
      ) { return 1; } 
      
    //if bottomright corner of w1 is inside area of w2, return 1    
    if(    ( (w1->x + w1->w) < (w2->x + w2->w) ) 
        && ( (w1->x + w1->w) >  w2->x          ) 
        && ( (w1->y + w1->h) < (w2->y + w2->h) ) 
        && ( (w1->y + w1->h) >  w2->y          ) 
      ) { return 1; } 
    */     
      
    return 0;
}

void updatePosition ( Word *w, double dt ) {
    w->x += w->vx * dt;
    w->y += w->vy * dt; 
}

void updatePositions ( Words *w, double dt ) {
    int i;
    for (i = 0; i < w->used; i++) {
        updatePosition(&w->array[i], dt);
    }
    

}

void reInitWord ( Word *w, double newX, double newY, double vx, double vy, char *string ) {
    w->x = newX;
    w->y = newY;
    w->vx = vx;
    w->vy = vy;
    w->string = string;
}

void printWords ( Words *w ) {
    int i;
    printf("words size / used: %d / %d: \n", w->size, w->used);
    for (i = 0; i < w->used; i++) {
        printf("word i: %d x: %.2f y: %.2f vx: %.2f vy: %.2f string: %s\n",
                i, w->array[i].x, w->array[i].y, w->array[i].vx, w->array[i].vy, w->array[i].string);
    }
}

int stringMatchesWord ( Word *w, char *string ) {
    return !(strcmp(string, w->string));
}

int stringMatchesWords ( Words *w, char *string ) {
    int i;
    for (i = 0; i < w->used; i++) {
        if (stringMatchesWord(&w->array[i], string)){
            return i;
        }
    }
    return -1;
}

void initWordpool( Wordpool *w, size_t initialSize ) {
    w->strings = (char **)malloc(initialSize * sizeof(char **));
    w->used = 0;
    w->size = initialSize;   
}

void freeWordpool ( Wordpool *w ) {
    int i;
    for (i = 0; i < w->used; i++) {
        free(w->strings[i]);
    }
    //free(w->strings);
    w->strings = NULL;
    w->used = w->size = 0;
}

void pushIntoWordpool ( Wordpool *w, char *element ) {
    if (w->used == w->size) {
        w->size *= 2;
        w->strings = (char **)realloc(w->strings, w->size * sizeof(char **));
    }
    if(element[strlen(element)-1] == '\n') element[strlen(element)-1] = '\0';
    w->strings[w->used++] = strdup(element);
}

void printWordpool ( Wordpool *w ) {
    int i;
    printf("wordpool size / used: %d / %d: \n", w->size, w->used);
    for (i = 0; i < w->used; i++) {
        printf("pool i: %i string: %s\n", i, w->strings[i]);
    }
}

void loadLevel ( Wordpool *w, char *filename ) {
    FILE * pFile;
    char baffer[30];
    char path[50];
    strcpy(path, "levels/");
    strcat(path, filename);
    pFile = fopen (path , "r");
    if (pFile == NULL) {
        perror ("Error opening file");
    } 
    else {
        while ( fgets (baffer , 30 , pFile) != NULL) {                                                    
            pushIntoWordpool(w, baffer);
        }
    fclose (pFile);
    }     
}