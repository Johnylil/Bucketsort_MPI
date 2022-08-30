#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]){

    int i,x = atoi(argv[1]);

    srand(time(NULL)); //correct way to do
    float sample_array[x];

    for (i;i<x;i++){
        sample_array[i] = (float)rand()/(RAND_MAX/(float)(10000.0)) ;
    }

    FILE *fpA;
    fpA = fopen("sample_array.txt", "w");
    if (fpA == NULL){
        printf("Error opening file A!\n");
        exit(1);
    }

    for (i=0;i<x;i++){
            fprintf(fpA, "%f ",*(sample_array + i));
        }
    fprintf(fpA, "\n");

    fclose(fpA);

    return 0;
}
