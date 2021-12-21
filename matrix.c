#include "matrix.h"
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

int n;
Matrix* og;
typedef struct param{
  int ind;
  int tasks;
  int startsign;
}param;

Matrix* makeMatrix(int r,int c)
{
   Matrix* m = malloc(sizeof(Matrix) + r * c * sizeof(int));
   m->r = r;
   m->c = c;
   return m;
}

int readValue(FILE* fd)
{
   int v = 0;
   char ch;
   int neg=1;
   while (((ch = getc_unlocked(fd)) != EOF) && isspace(ch)); // skip WS.      
   while (!isspace(ch)) {
      if (ch=='-')
         neg=-1;
      else
         v = v * 10 + ch - '0';
      ch = getc_unlocked(fd);
   }
   return neg * v;
}

Matrix* readMatrix(FILE* fd)
{
   int r,c,v;
   int nv = fscanf(fd,"%d %d",&r,&c);
   Matrix* m = makeMatrix(r,c);
   flockfile(fd);
   for(int i=0;i < r;i++)
      for(int j=0;j < c;j++) {
         v = readValue(fd);
         M(m,i,j) = v;
      }
   funlockfile(fd);
   return m;
}

Matrix* loadMatrix(char* fName)
{
   FILE* fd = fopen(fName,"r");
   if (fd==NULL) return NULL;
   Matrix* m = readMatrix(fd);
   fclose(fd);
   return m;
}

void freeMatrix(Matrix* m)
{
   free(m);
}

void printMatrix(Matrix* m)
{
   for(int i=0;i<m->r;i++) {
      for(int j=0;j < m->c; j++)
         printf("%3d ",M(m,i,j));     
      printf("\n");
   }
}

int helper(Matrix*m, int size){
  int det=0;
if (size==0){
  return 1;
}
else if (size==1){
int res=M(m, 0, 0);
return res;
}
else if (size==2){
  //the base case of a 2x2 matrix
  int topleft= M(m, 0, 0);
  int topright=M(m, 0, 1);
  int bottomleft=M(m, 1, 0);
  int bottomright=M(m, 1, 1);
  int first=topleft*bottomright;
  int second=topright*bottomleft;
  int res=first-second;
  return res;
}
else{
  int sign=1;
  for (int i=0; i<size; i++){
  int topelem=M(m, 0, i );
  int newr=0;
  int newc=0;
  Matrix* newMatrix=makeMatrix(size-1, size-1);
    for (int row=0; row<size; row++){
      for (int col=0;col<size; col++){
        if (row==0){
          continue;
        }
        if (col==i){
          continue;
        }
        else{
          int valtoInsert=M(m, row, col);
          M(newMatrix, newr, newc)=valtoInsert;
          newc+=1;
        }
      }
    }
    int adding=sign*topelem*helper(newMatrix, size-1);
    det+=adding;
    sign=sign*-1;
    free(newMatrix);
  }
}
return det;
}

int detMatrix(Matrix* m)
{
    int numrows=m->r;
    int numcols=m->c;
    int res=helper(m, numrows);
    return res; 
}

void*routine(void*arg){
  
  param* hi=(param*)arg;
  int index=hi->ind;
  int nummat=hi->tasks;
  int sign=hi->startsign;
  int results[nummat];
  for (int f=0; f<nummat; f++){
    results[f]=0;
  }
  int counter=0;
  int globnum=nummat;
while(nummat>0){
int newc=0;
int newr=0;
int topelem=M(og,0, index);
Matrix* submatrix=makeMatrix(n-1, n-1);
    for (int row=0; row<n; row++){
      for (int col=0;col<n; col++){
        if (row==0){
          continue;
        }
        if (col==index){

          continue;
        }
        else{
          int valtoInsert=M(og, row, col);
          M(submatrix, newr, newc)=valtoInsert;
          newc+=1;
        }
      }
    }
    int subdet=detMatrix(submatrix);
    int res=subdet*topelem;
    results[counter]=res;
    nummat-=1;
    index+=1;
    counter+=1;
    free(submatrix);
}
 int* res=malloc(sizeof(int));
 for (int i=0; i<globnum; i++){
   *res+=results[i]*sign;
   sign=sign*-1;  
}
return (void*)res;
}

int detMatrixPar(Matrix* m,int nbW)
{   n=m->r;
    og=m;
    int partsEach=n/nbW;
    int remaining= n%nbW;
    int workerList[nbW];
    for(int q=0; q<nbW; q++){
      workerList[q]=partsEach;
      if (remaining>0){
        workerList[q]++;
      }
      remaining--;
    }
    
    int det=0;
    pthread_mutex_t detProtect;
    pthread_mutex_init(&detProtect, NULL);
    pthread_t worker[nbW];
        for (int b=0; b<nbW; b++){
    }
    if (m->r==0||m->c==0){
      return 1;
    }
    if (nbW==1){
      return detMatrix(m);
    }
    int startingInd[nbW];
    startingInd[0]=0;
    int beg=0;
    int ct=0;
    for (int i=1; i<nbW; i++){
      beg+= workerList[ct];
      startingInd[i]=beg;
      ct+=1;
    }
 
    for (int i=0; i<nbW; i++){
      int numTasks=workerList[i];
      param* tryhelp=malloc(sizeof(struct param));
      tryhelp->ind=startingInd[i];
      tryhelp->tasks=workerList[i];
      if (startingInd[i]==0||startingInd[i]%2==0){
        tryhelp->startsign=1;
      }
      else{
        tryhelp->startsign=-1;
      }
      pthread_create(&worker[i], NULL, &routine, tryhelp);
    }
    int*res;
    for (int j=0; j<nbW; j++){
      pthread_join(worker[j], (void**)&res);
      pthread_mutex_lock(&detProtect);
      det+=*res;
      pthread_mutex_unlock(&detProtect);
    }
    return det; 
}


