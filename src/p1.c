#include <stdio.h>
#include <stdlib.h>
#include "pgmIO.h"

char fileCode[2];

int main(int argc, char *argv[])
{
    if(argc != 3) { // argc == 3 -> ./p1 source.pgm destination.pgm
        printf("usage: %s source.pgm destination.pgm\n", argv[0]);
    }
    else {
        int succ = 1;
        FILE *file = fopen(argv[1], "r");
        if (file == 0) {
            printf("Could not open file!\n");
        }
        else {  
            fscanf(file, "%s", fileCode);
            if(strcmp(fileCode, "P2")==0) {
                PGMData *theFile, theFileB;
                theFile = &theFileB; // theFileB legt einen neuen Header an
                theFile = readPGM(argv[1], theFile);
                writePGM(argv[2], theFile);
            }
            succ = fclose(file);
        }
        if(succ == 0) printf("Success!\n");
    }
    return 0;
}
