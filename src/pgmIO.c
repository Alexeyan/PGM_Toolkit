#include "pgmIO.h"
typedef enum { false, true } bool;

float** allocate_dynamic_matrix(int row, int col){
    float** ret_val;
    int i;

    ret_val = (float **)malloc(sizeof(float *) * row);
    if (ret_val == NULL) {
        perror("memory allocation failure");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < row; ++i) {
        ret_val[i] = (float *)malloc(sizeof(float) * col);
        if (ret_val[i] == NULL) {
            perror("memory allocation failure");
            exit(EXIT_FAILURE);
        }
    }
    return ret_val;
}

void deallocate_dynamic_matrix(float **matrix, int row)
{
    int i;
    for (i = 0; i < row; ++i)
        free(matrix[i]);
    free(matrix);
}

// Überspringe Kommentare
void SkipComments(FILE *fp) {
    int ch;

    //Überspringe alle Lehrzeichen
    while ((ch = fgetc(fp)) != EOF && isspace(ch)) ;
    if (ch == '#') {
        char line[100];
        fgets(line, sizeof(line), fp); //Speichere eine Zeile des Bilds in line
        SkipComments(fp);
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
}

PGMData* readPGM(const char *file_name, PGMData *data)
{
    FILE *pgmFile;
    char version[3];
    int i, j;

    pgmFile = fopen(file_name, "rb");
    if (pgmFile == NULL) {
        perror("cannot open file to read");
        exit(EXIT_FAILURE);
    }

    fgets(version, sizeof(version), pgmFile);
    if (strcmp(version, "P2")) {
        fprintf(stderr, "Wrong file type!\n");
        exit(EXIT_FAILURE);
    }
    SkipComments(pgmFile);

    fscanf(pgmFile, "%d", &data->col);
    SkipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->row);
    SkipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->max_gray);
    fgetc(pgmFile);

    data->matrix = allocate_dynamic_matrix(data->row, data->col);
    int test;
    for (i = 0; i < data->row; ++i) {
            for (j = 0; j < data->col; ++j) {
                fscanf(pgmFile, "%d", &test);
                //test = fgetc(pgmFile);
                if(test == EOF) break;
                data->matrix[i][j] = (float)test;
            }
    }

    fclose(pgmFile);
    return data;

}

/*and for writing*/
void writePGM(const char *filename, const PGMData *data)
{
    FILE *pgmFile;
    int i, j;

    pgmFile = fopen(filename, "w");
    if (pgmFile == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }

    fprintf(pgmFile, "P2 \n");
    fprintf(pgmFile, "%d %d \n", data->col, data->row);
    fprintf(pgmFile, "%d \n", data->max_gray);

    for (i = 0; i < data->row; ++i) {
        for (j = 0; j < data->col; ++j) {
            int value = (int) data->matrix[i][j];
            fprintf(pgmFile, "%d ", value);
        }
        fprintf(pgmFile, "\n");
    }

    fclose(pgmFile);
    // deallocate_dynamic_matrix(data->matrix, data->row);
}

void writeAndClosePGM(const char *filename, const PGMData *data) {
    writePGM(filename, data);
    deallocate_dynamic_matrix(data->matrix, data->row);
}

PGMData* newPGM(int row, int col, int max_gray) {
    PGMData *dst = (PGMData*) malloc(sizeof(PGMData));
    if (dst == NULL) {
        perror("memory allocation failure");
        exit(EXIT_FAILURE);
    }
    dst->matrix = allocate_dynamic_matrix(row, col);
    dst->row = row;
    dst->col = col;
    dst->max_gray = max_gray;
    return dst;
}

PGMData* copyPGM (PGMData *src) {
  PGMData *dst = (PGMData*) malloc(sizeof(PGMData));
  // *dst = *src;
  memcpy(dst, src, sizeof(PGMData));
  // dst->matrix = (float**) malloc (dst->row * dst->col * sizeof(float));
  dst->matrix = allocate_dynamic_matrix(src->row, src->col);
  if (dst == NULL) {
        perror("memory allocation failure");
        exit(EXIT_FAILURE);
    }
  memcpy(dst->matrix, src->matrix, sizeof(float) * src->row * src->col);
  return dst;
}

PGMData* filterPGM (PGMData *src, int dimension, float kernel[dimension][dimension]) {
    PGMData *dst;
    dst = (PGMData *) malloc(sizeof(PGMData));
    *dst = *src;
    dst->matrix = allocate_dynamic_matrix(dst->row, dst->col);

    float normalizer = 0.0;
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            normalizer += kernel[i][j];
        }
    }
    if(normalizer == 0.0) {

        normalizer = 0.0;
    } else {
        normalizer = 1.0 / normalizer;
    }

    const int rightEnd = src->col- 1;
    const int bottomEnd = src->row - 1;
    int g, h, i, j, row, col;
    float temp, weight;

    //Für jeden Pixel im Bild
    for (i = 0; i < src->row; ++i) {
        for (j = 0; j < src->col; ++j) {
            temp = 0.0;

            //Wenden wir den Filter-Kernel an
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

                    weight = kernel[g][h];
                    temp = temp + (weight * src->matrix[row][col]);
                }
            }

            if(normalizer != 0.0) {
                temp = temp * normalizer;
                if (temp < 0) temp = 0;
                else if (temp > dst->max_gray) temp = dst->max_gray;
            }
            dst->matrix[i][j] = temp;
        }
    }

    return dst;
}

void nonMaxSuppr (PGMData *src, int x, int y, int dimension, float kernel[dimension][dimension]) {
    const int rightEnd = src->col- 1;
    const int bottomEnd = src->row - 1;
    int g = 0, h = 0, row = 0, col = 0;
    bool breaking = false;

    // Wir schauen und jedes Feld im Kernel an.
    for(g = 0; g < dimension; g++) {
        for(h = 0; h < dimension; h++) {
            row = x-1+g;
            if(row < 0) {
                breaking = true;
                break;
            } else if(row > bottomEnd) {
                breaking = true;
                break;
            }

            col = y-1+h;
            if(col < 0) {
                breaking = true;
                break;
            } else if(col > rightEnd) {
                breaking = true;
                break;
            }
            if(kernel[g][h] == 1.0 && src->matrix[row][col] > src->matrix[x][y]) {
                src->matrix[x][y] = 0.0;
                return;
            }
        }
        if(breaking) {
            breaking = false;
            break;
        }
    }
}
