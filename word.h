#ifndef _WORD_H_
#define _WORD_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct word{
    double x, y, vx, vy;
    char *string;
}Word;

typedef struct words {
    Word *array;
    size_t used;
    size_t size;
    int max;
}Words;

void initWords(Words *a, size_t initialSize);
void freeAll(Words *a);
int pushInto(Words *a, Word element);
void setString(Word *word, char *string);
void setVelocity(Word *word, double vx, double vy);
void setPosition(Word *word, double newX, double newY);
void updatePosition(Word *word, double dt);
void updatePositions(Words *words, double dt);
void reInitWord(Word *word, double newX, double newY, double vx, double vy, char *string);
void printWords(Words *words);
int stringMatchesWord(Word *word, char *string);
#endif
