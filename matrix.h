#ifndef __MATRIX_H
#define __MATRIX_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Matrix {
   int   r,c;
   int   data[0];
} Matrix;

#define M(p,i,j) ((p)->data[(i)*((p)->c) + (j)])

Matrix* makeMatrix(int r,int c);
Matrix* readMatrix(FILE* fd);
Matrix* loadMatrix(char* fName);
void freeMatrix(Matrix* m);
void printMatrix(Matrix* m);
int detMatrix(Matrix* m);
int detMatrixPar(Matrix* m,int nbW);

#endif
