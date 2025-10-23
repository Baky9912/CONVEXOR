#include "myrandom.h"
#include <stdlib.h>


float rand_float(){
    return ((float)rand() / RAND_MAX);
}

float rand_centered(){
    return rand_clt_on_level(2);
}

float rand_clt_on_level(int lvl){
    float sum=0;
    for(int i=0; i<lvl; ++i){
        sum += rand_float();
    }
    return (sum / lvl);
}

int rand_int(int from, int to){
    // inkluzivno
    // ne radi za velike brojeve, treba 2 puta da se generise
    return (rand() % (to-from+1)) + from;
}