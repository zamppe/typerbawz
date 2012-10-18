#ifndef _WORD_H_
#define _WORD_H_
struct word{
    double x, y, vx, vy;
    char *string;
};
void initializeWords(struct word *words, int size);
void moveWordTo(struct word *word, double newX, double newY);
void pushWord(struct word *words, int size, char *string, double newX, double newY, double newVx, double newVy);
void moveWord(struct word *words, int i, double dx, double dy);
void updateWordPosition(struct word *word, double dt);
void updateWordPositions(struct word *words, int size, double dt);
void printWords(struct word *words, int size);
int stringMatchesWord(struct word *word, char *string);
#endif
