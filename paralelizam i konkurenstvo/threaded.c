#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "omp.h"
//  f(x) = x^3

void monteCarlo_x_pow_3(long N, double a, double b){

    double num = 0, num3 = 0, sum = 0, sqsum = 0;
    #pragma omp parallel reduction (+:sum, sqsum)
    {
        unsigned int seed = time(0) + omp_get_thread_num();

        #pragma omp for
            for(long i = 0; i < N; i++){
                num = ((double)rand_r(&seed) / RAND_MAX) * (b - a) + a;
                num3 = (pow(num, 3));
                sum += num3;
                sqsum += pow(num3,2);
            }
    }
    double mc = (double)(b - a)/N * sum;
    printf("\nMonte Carlo Estimate: %f", mc);

    double avg = sum/N;
    double dev = sqrt((sqsum / N) - pow(avg,2));
    double V = dev / sqrt(N);
    printf("\nError (Standard Error): %f", V);
}

int main(int argc, char **argv){
    double a = 0, b = 10;
    double begin, end;
    begin = omp_get_wtime();

    printf("Usage: ./mct [num_points]");

    long N = 500000000;
    if(argc > 1){
        sscanf(argv[1], "%ld", &N);
        printf("\nNumber of points: %ld", N);
    } else printf("\nNumber of points not specified, default number of points is used: %ld", N);

    monteCarlo_x_pow_3(N, a, b);
    end = omp_get_wtime();

    printf("\nElapsed time: %f\n", (end - begin));
}
