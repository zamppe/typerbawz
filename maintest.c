#include "word.h"

int main(){
    struct word words[50];
    int size = sizeof(words) / sizeof(*words);
    initializeWords(words, size);
    //                      word    x    y    vx   vy
    pushWord(words, size, "yigga", 2.3, 1.2, 5.3, 1.2);
    pushWord(words, size, "nigga", 2.3, 1.2, 5.3, 1.2);
    printWords(words, size);

    return 0;
}
