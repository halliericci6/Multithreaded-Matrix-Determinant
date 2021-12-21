#include "matrix.h"

int main(int argc,char* argv[])
{
   if (argc < 3) {
      printf("usage: detMT <file1> <#workers>\n");
      exit(1);
   }
   Matrix* a = loadMatrix(argv[1]);
   int nbW = atoi(argv[2]);
   if (a==NULL) {
      printf("Wrong filenames. Couldn't load [%s]\n",argv[1]);
      exit(2);
   }
   int dm = detMatrixPar(a,nbW);
   printf("Determinant = %d\n",dm);
   return 0;
}
