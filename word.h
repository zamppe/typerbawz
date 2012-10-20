#ifndef _WORD_H_
#define _WORD_H_
#include "SDL/SDL.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct word{
    double x, y, vx, vy;
    char *string;
    struct SDL_SURFACE *surface;
}Word;

typedef struct words {
    Word *array;
    size_t used;
    size_t size;
    int max;
}Words;

typedef struct wordpool {
    char **strings;
    size_t used;
    size_t size;
}Wordpool;

void initWords ( Words *w, size_t initialSize );
void freeWords ( Words *w );
int pushIntoWords ( Words *w, Word element );
void setString ( Word *w, char *string );
void setVelocity ( Word *w, double vx, double vy );
void setPosition ( Word *w, double newX, double newY );
void updatePosition ( Word *w, double dt );
void updatePositions ( Words *w, double dt );
void reInitWord ( Word *w, double newX, double newY, double vx, double vy, char *string );
void printWords ( Words *w );
int stringMatchesWord ( Word *w, char *string );
void initWordpool ( Wordpool *w, size_t initialSize );
void freeWordpool ( Wordpool *w );
int pushIntoWordpool ( Wordpool *w, char *element );
void printWordpool ( Wordpool *w );
#endif
