/**
*   \file main.c
*   \brief main
*   \author Antoine Milot - Tiphaine Diot 
*   \date Avril/Mai 2017
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int main() {
    FILE *fichier = fopen("/dev/ttyACM0", "r+");

    while(1){
        fprintf(fichier, "50,50\n");
        usleep(1000*1000);
        fprintf(fichier, "90,90\n");
        usleep(1000*1000);
    }
 
    fclose(fichier);
    return 0;
 
}

