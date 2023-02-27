#include "stdlib.h"
#include "stdio.h"
#include <inttypes.h>
#include <time.h>
#include "needle_pcs.h"
#include "needle_rho.h"
#include "pcs_storage.h"
#include "pcs.h"
#include "random_functions.h"

int main() {
    // the needle will appear 2^5=32 times, hash has been checked to not contain any other needles of similar magnitude.
    uint8_t n = 24, memory = 6, prob = 18;

    //pcsPcsModeDetection(n, memory, prob);
    //pcsRhoModeDetection(n, memory, prob);
    //rhoModeDetection(n, prob);

    FILE *fptr1;
    FILE *fptr2;

    fptr1 = fopen("benchmarksPCS.txt","a");
    fptr2 = fopen("benchmarksRho.txt","a");

    if(fptr1 == NULL || fptr2 == NULL) {
        printf("Error!");
        exit(1);
    }

    clock_t start_time;
    double elapsed_time;
    int flag, nb_tests = 100;

    for (int i = 0; i < nb_tests; ++i) {

        start_time = clock();

        flag = pcsPcsModeDetection(n, memory, prob);

        elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        printf("PCS done in %f seconds\n", elapsed_time);
        fprintf(fptr1,"%f : %d \n",elapsed_time, flag);
        fflush(fptr1);

        start_time = clock();

        flag = pcsRhoModeDetection(n, memory, prob);

        elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        printf("Rho done in %f seconds\n", elapsed_time);
        fprintf(fptr2,"%f : %d \n",elapsed_time, flag);
        fflush(fptr2);
    }

    fclose(fptr1);
    fclose(fptr2);

    return 0;
}