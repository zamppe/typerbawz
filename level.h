#ifndef _LEVEL_H_
#define _LEVEL_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct level{
    Words words; // floating strings on the screen
    Wordpool wordpool; // the pool of strings from which strings are randomly selected
    
    int width;
    int fontsize;
    
    int addspeedinterval; 
    int addspeedcounter;
    double addspeedamount;
    
    int addwordinterval;  
    int addwordcounter;
    int addwordamount;
    
    double initspeed;
    int initamount;
    
    double maxspeed;
    int maxamount;
}Level;
void initLevel ( Level *level );
void openLevel ( Level *level, char *filename );
void setRules( Level *level, int asi, int asc, double asa, int awi, int awc, int awa, double inits, int inita, double maxs, int maxa );
void setUpLevel( Level *level );

#endif