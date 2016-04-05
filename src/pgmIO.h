#ifndef PGMIO_H
#define PGMIO_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct _PGMFile {
    int row;
    int col;
    int max_gray;
    float** matrix;
} PGMData;

float** allocate_dynamic_matrix(int row, int col);
void deallocate_dynamic_matrix(float **matrix, int row);
void SkipComments(FILE *fp);
PGMData* readPGM(const char *file_name, PGMData *data);
void writeAndClosePGM(const char *filename, const PGMData *data);
void writePGM(const char *filename, const PGMData *data);
PGMData* newPGM(int row, int col, int max_gray);
PGMData* copyPGM(PGMData *src);
PGMData* filterPGM(PGMData *src, int dimension, float kernel[dimension][dimension]);
void nonMaxSuppr(PGMData *src, int x, int y, int dimension, float kernel[dimension][dimension]);

#endif /* PGMIO_H */
