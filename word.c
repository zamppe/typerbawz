#include "word.h"
#include <stdio.h>
#include <string.h>



void initializeWords(struct word *words, int size){
    int i;
    for(i = 0; i < size; i++){
        words[i].x = 0.0;
        words[i].y = 0.0;
        words[i].vx = 0.0;
        words[i].vy = 0.0;
        words[i].string = "";
    }
}

void moveWordTo(struct word *word, double newX, double newY){
    word->x = newX;
    word->y = newY;
}

void pushWord(struct word *words, int size, char *string, double newX, double newY, double newVx, double newVy){
    int i = 0;
    while(words[i].string[0] != '\0'){
        i++;
        printf("%d \n", i);
    }
    moveWordTo(&words[i], newX, newY);
    words[i].vx = newVx;
    words[i].vy = newVy;
    words[i].string = string;
}


void moveWord(struct word *words, int i, double dx, double dy){
    words[i].x += dx;
    words[i].y += dy;
}

void updateWordPosition(struct word *word,  double dt){
    word->x += word->vx * dt;
    word->y += word->vy * dt;
}

void updateWordPositions(struct word *words, int size, double dt){
    int i;
    for(i = 0; i < size; i++){
        updateWordPosition(&words[i], dt);
    }

}

void printWords(struct word *words, int size){
    int i;
    for(i = 0; i < size; i++){
        if(words[i].string[0] != '\0'){
                printf("word%d.x: %.2f word%d.y: %.2f word%d.vx: %.2f word%d.vy: %.2f %s\n",
                        i+1, words[i].x, i+1, words[i].y, i+1, words[i].vx, i+1, words[i].vy, words[i].string);
        }

    }
}

int stringMatchesWord(struct word *word, char *string){
    return !(strcmp(string, word->string));
}
