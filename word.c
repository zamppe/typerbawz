#include "word.h"

void initWords(Words *words, size_t initialSize) {
    words->array = (Word *)malloc(initialSize * sizeof(Word));
    words->used = 0;
    words->size = initialSize;
    words->max = 3;
}

void freeAll(Words *words) {
    free(words->array);
    words->array = NULL;
    words->used = words->size = 0;
}

int pushInto(Words *words, Word element) {
    if (words->used == words->max){
        return 0;
    }
    if (words->used == words->size) {
        words->size += 1;
        words->array = (Word *)realloc(words->array, words->size * sizeof(Word));
    }
    words->array[words->used++] = element;
    return 1;
}

void setString(Word *word, char *string){
    word->string = string;
}

void setVelocity(Word *word, double vx, double vy){
    word->vx = vx;
    word->vy = vy;
}

void setPosition(Word *word, double newX, double newY){
    word->x = newX;
    word->y = newY;
}

void updatePosition(Word *word, double dt){
    word->x += word->vx * dt;
    word->y += word->vy * dt;
}

void updatePositions(Words *words, double dt){
    int i;
    for(i = 0; i < words->used; i++){
        updatePosition(&words->array[i], dt);
    }

}

void reInitWord(Word *word, double newX, double newY, double vx, double vy, char *string){
    word->x = newX;
    word->y = newY;
    word->vx = vx;
    word->vy = vy;
    word->string = string;
}

void printWords(Words *words){
    int i;
    for(i = 0; i < words->used; i++){
        if(words->array[i].string[0] != '\0'){
                printf("word i: %d x: %.2f y: %.2f vx: %.2f vy: %.2f string: %s\n",
                        i, words->array[i].x, words->array[i].y, words->array[i].vx, words->array[i].vy, words->array[i].string);
        }
    }
}

int stringMatchesWord(Word *word, char *string){
    return !(strcmp(string, word->string));
}
