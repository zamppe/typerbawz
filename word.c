#include "word.h"

void initWords ( Words *w, size_t initialSize ) {
    w->array = (Word *)malloc(initialSize * sizeof(Word));
    w->used = 0;
    w->size = initialSize;
    w->max = 3;
}

void freeWords ( Words *w ) {
    free(w->array);
    w->array = NULL;
    w->used = w->size = 0;
}

int pushIntoWords ( Words *w, Word element ) {
    if ( w->used == w->max ){
        return 0;
    }
    if ( w->used == w->size ) {
        w->size += 1;
        w->array = (Word *)realloc(w->array, w->size * sizeof(Word));
    }
    w->array[w->used++] = element;
    return 1;
}

void setString ( Word *w, char *string ) {
    w->string = string;
}

void setVelocity ( Word *w, double vx, double vy ) {
    w->vx = vx;
    w->vy = vy;
}

void setPosition ( Word *w, double newX, double newY ) {
    w->x = newX;
    w->y = newY;
}

void setX ( Word *w, double newX ) {
    w->x = newX;
}

void updatePosition ( Word *w, double dt ) {
    w->x += w->vx * dt;
    w->y += w->vy * dt;
    if(w->x < 0){
        //todo: this needs to somehow know where the start is
        w->x = 600;
    }    
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
    free(w->strings);
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
