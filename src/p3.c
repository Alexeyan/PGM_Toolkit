#include <stdio.h>
#include <stdlib.h>
#include "pgmIO.h"

//Sorts a int-array in ascending order
void quicksort(int x[9],int first,int last){

     if(first<last){
         int pivot,j,temp,i;

         pivot=first;
         i=first;
         j=last;

         while(i<j){
             while(x[i]<=x[pivot]&&i<last)
                 i++;
             while(x[j]>x[pivot])
                 j--;
             if(i<j){
                 temp=x[i];
                  x[i]=x[j];
                  x[j]=temp;
             }
         }

         temp=x[pivot];
         x[pivot]=x[j];
         x[j]=temp;
         quicksort(x,first,j-1);
         quicksort(x,j+1,last);

    }
}

PGMData *minPGM (PGMData *src, int dimension) {
  	PGMData *dst = (PGMData *) malloc(sizeof(PGMData));
  	*dst = *src;
  	dst->matrix = allocate_dynamic_matrix(dst->row, dst->col);

  	const int rightEnd = src->col - 1;
  	const int bottomEnd = src->row - 1;


  	for (int i = 0; i < src->row; ++i) {
   		for (int j = 0; j < src->col; ++j) {
            int values[dimension*dimension], row, col, count;

      		count = 0;
      		for(int g = 0; g < dimension; g++) {
      			for(int h = 0; h < dimension; h++) {
      				row = i-1+g;
      				if(row < 0) {
      					row = 0;
      				} else if(row > bottomEnd) {
      					row = bottomEnd;
      				}

      				col = j-1+h;
      				if(col < 0) {
      					col = 0;
      				} else if(col > rightEnd) {
      					col = rightEnd;
      				}
      				values[count] = src->matrix[row][col];
      				count++;
      			}
      		}
      		quicksort(values, 0, dimension*dimension);
   			dst->matrix[i][j] = values[0];
   		}
   	}
   	return dst;
}

PGMData* medPGM (PGMData *src, int dimension) {
  	PGMData *dst = (PGMData *) malloc(sizeof(PGMData));
  	*dst = *src;
  	dst->matrix = allocate_dynamic_matrix(dst->row, dst->col);

  	const int rightEnd = src->col - 1;
  	const int bottomEnd = src->row - 1;

  	for (int i = 0; i < src->row; ++i) {
   		for (int j = 0; j < src->col; ++j) {


      		int values[dimension*dimension], g, h, row, col, count = 0;
      		for(g = 0; g < dimension; g++) {
      			for(h = 0; h < dimension; h++) {
      				row = i-1+g;
      				if(row < 0) {
      					row = 0;
      				} else if(row > bottomEnd) {
      					row = bottomEnd;
      				}

      				col = j-1+h;
      				if(col < 0) {
      					col = 0;
      				} else if(col > rightEnd) {
      					col = rightEnd;
      				}
      				values[count] = src->matrix[row][col];
      				count++;
      			}
      		}
      		quicksort(values, 0, dimension*dimension);
      		int med = (dimension*dimension+1) / 2;
   			dst->matrix[i][j] = values[med];
   		}
   	}
   	return dst;
}

PGMData* maxPGM (PGMData *src, int dimension) {
  	PGMData *dst = (PGMData *) malloc(sizeof(PGMData));
  	*dst = *src;
  	dst->matrix = allocate_dynamic_matrix(dst->row, dst->col);

  	const int rightEnd = src->col - 1;
  	const int bottomEnd = src->row - 1;

  	for (int i = 0; i < src->row; ++i) {
   		for (int j = 0; j < src->col; ++j) {

      		int values[dimension*dimension], g, h, row, col, count = 0;
      		for(g = 0; g < dimension; g++) {
      			for(h = 0; h < dimension; h++) {
      				row = i-1+g;
      				if(row < 0) {
      					row = 0;
      				} else if(row > bottomEnd) {
      					row = bottomEnd;
      				}

      				col = j-1+h;
      				if(col < 0) {
      					col = 0;
      				} else if(col > rightEnd) {
      					col = rightEnd;
      				}
      				values[count] = src->matrix[row][col];
      				count++;
      			}
      		}
      		quicksort(values, 0, dimension*dimension);
   			dst->matrix[i][j] = values[dimension*dimension-1];
   		}
   	}
   	return dst;
}

int main(int argc, char *argv[])
{
   printf("P3\n");
	if(argc != 6) { // argc == 6 -> ProgName + 5 Argumente
        printf("usage: %s src.pgm N min.pgm med.pgm max.pgm”\n", argv[0]);
    }

    else {
    	PGMData *theFileAddress, *theFileMIN, *theFileMED, *theFileMAX, theFile;

        theFileAddress = &theFile;
        theFileAddress = readPGM(argv[1], theFileAddress);
        int dim;
        sscanf(argv[2],"%d",&dim);

        theFileMIN = minPGM(theFileAddress, dim);
        writePGM(argv[3], theFileMIN);

        theFileMED = medPGM(theFileAddress, dim);
        writePGM(argv[4], theFileMED);

        theFileMAX = maxPGM(theFileAddress, dim);
        writePGM(argv[5], theFileMAX);
	}


   return 0;
}
