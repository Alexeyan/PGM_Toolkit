#include <stdio.h>
#include <stdlib.h>
#include "pgmIO.h"

//TODO better memory management!!!


int main(int argc, char *argv[])
{
   printf("P2\n");

   if(argc != 4) { // argc == 4 -> ProgName + 3 Argumente
        printf("usage: %s src.pgm blur.pgm sharp.pgm”\n", argv[0]);
    }

    else {

        PGMData *theFileAddress, *theFileSoft, *theFileHard, theFile;

        theFileAddress = &theFile;
        theFileAddress = readPGM(argv[1], theFileAddress);

        // theFileSoft = blurPGM(theFileAddress);
        float blurKernel[3][3] = {
          {1, 2, 1},
          {2, 4, 2},
          {1, 2, 1},
        };
        theFileSoft = filterPGM(theFileAddress, 3, blurKernel);
        writePGM(argv[2], theFileSoft);

        float hardKernel[3][3] = {
          {0, -1, 0},
          {-1, 5, -1},
          {0, -1, 0},
        };
        theFileHard = filterPGM(theFileAddress, 3, hardKernel);
        writePGM(argv[3], theFileHard);
    }


   return 0;
}
