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

int pushIntoWordpool ( Wordpool *w, char *element ) {
    if (w->used == w->size) {
        w->size += 100;
        w->strings = (char **)realloc(w->strings, w->size * sizeof(char **));
    }
    w->strings[w->used++] = element;
    return 1;
}

void printWordpool ( Wordpool *w ) {
    int i;
    printf("wordpool size / used: %d / %d: \n", w->size, w->used);
    for (i = 0; i < w->used; i++) {
        printf("pool i: %i string: %s\n", i, w->strings[i]);
    }
}
