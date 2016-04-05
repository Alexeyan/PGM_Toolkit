#include <stdio.h>
#include <stdlib.h>
#include "pgmIO.h"
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

double gradValue = 0.0;/*
 *  Rundet eine Radianzahl auf 4 mögliche Grad Ergebnisse.
 *
 */
double roundGradient(double grad) {
  gradValue = grad;
  while(grad < -90.0) gradValue = gradValue + 180.0;
  while(grad > 90.0) gradValue = gradValue - 180.0;

  if(grad >= -90.0 && grad <= -67.5) return 90.0;
  if(grad > -67.5 && grad <= -22.5) return -45.0;
  if(grad > -22.5 && grad <= 22.5) return 0.0;
  if(grad > 22.5 && grad <= 67.5) return 45.0;
  if(grad > 67.5 && grad <= 90.0) return 90.0;

  printf("ERROR IN DER RUNDUNGSFUNKTION: %f\n", gradValue);
  return 0;
}

PGMData* recognizeEdges(PGMData *src, float threshold) {
  PGMData *theFileSoft, theFileSoftFile, *Ghat, GhatFile, *G, GFile, *gX, gXFile, *gY, gYFile, *dst;
  //PGMData *dst = (PGMData *) malloc(sizeof(PGMData));

  // 1. Weichzeichnen
  float blurKernel[3][3] = {
    {1.0, 2.0, 1.0},
    {2.0, 4.0, 2.0},
    {1.0, 2.0, 1.0}
  };
  theFileSoft = filterPGM(src, 3, blurKernel);
  theFileSoftFile = *theFileSoft;

  // PGMData *writeThis, writeThisFile;
  // writeThis = &writeThisFile;
  // writeThis = copyPGM(theFileSoft);
  // writePGM("p4_step_blur.pgm", writeThis);
  // we may have memory problems here...
  // free(writeThis);

  dst = newPGM(src->row, src->col, src->max_gray);
  G = newPGM(src->row, src->col, src->max_gray);
  Ghat = newPGM(src->row, src->col, src->max_gray);
  GhatFile = *Ghat;
  GFile = *G;

  // 2. Partielle Ableitungen bilden
  float hX[3][3] = {
    {1.0, 0.0, -1.0},
    {2.0, 0.0, -2.0},
    {1.0, 0.0, -1.0}
  };
  gX = filterPGM(theFileSoft, 3, hX);
  gXFile = *gX;

  // PGMData *writeThis2, writeThisFile2;
  // writeThis2 = &writeThisFile2;
  // writeThis2 = copyPGM(gX);
  writePGM("p4_step_partX.pgm", gX);
  // // we may have memory problems here...
  // //free(writeThis2);
  // printf("test 2\n");

  float hY[3][3] = {
    { 1.0,  2.0,  1.0},
    { 0.0,  0.0,  0.0},
    {-1.0, -2.0, -1.0}
  };
  gY = filterPGM(theFileSoft, 3, hY);
  gYFile = *gY;

  // // PGMData *writeThis3;
  // writeThis = copyPGM(gY);
  writePGM("p4_step_partY.pgm", gY);
  // // we may have memory problems here...
  // //free(writeThis3);
  // printf("test 3\n");

  // 3, 4, 5
  float tmp = 0.0;
  double division = 0.0;
  double theta[theFileSoft->row][theFileSoft->col];

  for (int i = 0; i < theFileSoft->row; ++i) {
    for (int j = 0; j < theFileSoft->col; ++j) {
      // 3. Stärkung der Kanten
      // tmp = sqrt(hX^2 + sobelY^2)
      tmp = 0.0;
      tmp = sqrt((gX->matrix[i][j] * gX->matrix[i][j]) + (gY->matrix[i][j] * gY->matrix[i][j]));
      G->matrix[i][j] = tmp;

      // 4. Kantendetektion (Gesammt-Sobel)
      if(tmp < threshold) {
        Ghat->matrix[i][j] = 0.0;
      } else {
        Ghat->matrix[i][j] = tmp;
      }

      // 5. Richtung der Kanten (Gradienten)
      if(gX->matrix[i][j] != 0) {
        division = (double) gY->matrix[i][j] / (double) gX->matrix[i][j];
      } else {
        division = 90.0;
      }
      // printf("%f\n", division);
      theta[i][j] = atan(division);
    }
  }

  writePGM("p4_step_G.pgm", G);
  writePGM("p4_step_Ghat.pgm", Ghat);
  // writePGM("p4Edges.pgm", &theFileEdges);
  // printf("test 3-1\n");

  // 6. Non-maximum suppression
  double gradRad = 0.0, gradDeg = 0.0, roundedDeg = 0.0;
  float NSKernel[3][3] = {{0, 1, 0},
                          {0, 1, 0},
                          {0, 1, 0}};
  float WEKernel[3][3] = {{0, 0, 0},
                          {1, 1, 1},
                          {0, 0, 0}};
  float SWNEKernel[3][3] = {{0, 0, 1},
                            {0, 1, 0},
                            {1, 0, 0}};
  float SENWKernel[3][3] = {{1, 0, 0},
                            {0, 1, 0},
                            {0, 0, 1}};

  for (int i = 0; i < theFileSoft->row; ++i) {
    for (int j = 0; j < theFileSoft->col; ++j) {
      gradRad = theta[i][j];
      gradDeg = gradRad * 180.0 / M_PI;
      roundedDeg = roundGradient(gradDeg);
      switch((int) roundedDeg) {
        case 90:  nonMaxSuppr(Ghat, i, j, 3, NSKernel); break;
        case 45:  nonMaxSuppr(Ghat, i, j, 3, SWNEKernel); break;
        case 0:   nonMaxSuppr(Ghat, i, j, 3, WEKernel); break;
        case -45: nonMaxSuppr(Ghat, i, j, 3, SENWKernel); break;
        default: printf("Sollte nicht passieren!\n");
      }
    }
  }
  // printf("test4-2\n");
  // // PGMData *writeThis4;
  // writeThis = copyPGM(dst);
  // printf("test4-1\n");
  writePGM("p4_step_Ghat_after_NMS.pgm", Ghat);
  // // we may have memory problems here...
  // //free(writeThis4);
  // printf("test 4\n");

  // 7. Binärkantenbild
  for (int i = 0; i < dst->row; ++i) {
    for (int j = 0; j < dst->col; ++j) {
      if(Ghat->matrix[i][j] > 0.0) dst->matrix[i][j] = Ghat->max_gray;
      if(Ghat->matrix[i][j] < 0.0) dst->matrix[i][i] = 0.0;
    }
  }

  // free(writeThis);

  return dst;
}

int main(int argc, char *argv[])
{
   printf("P4\n");

    if(argc != 3) { // argc == 3 -> ProgName + 2 Argumente
      printf("usage: ./%s source.pgm edges.pgm\n", argv[0]);
    }
    else {
      PGMData *theFileAddress, theFile, *theFileEdges;

      theFileAddress = &theFile;
      theFileAddress = readPGM(argv[1], theFileAddress);

      const float threshold = 120.0;
      theFileEdges = recognizeEdges(theFileAddress, threshold);
      writePGM(argv[2], theFileEdges);
   }
   return 0;
}
