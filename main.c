#include <stdio.h>
#include <stdlib.h>
#include "omp.h"


int dataLength = 20;
double  *maximums;
int *counts;
int total;

double maximum(double *data) {
    double max =  data[0] ;
#pragma omp parallel for reduction(max: max)
    for (int i = 0; i < dataLength; i++) {
        if (max < data[i])
            max = data[i];
    }
    return max;
}

double minimum(double *data) {
    double min =  data[0];
#pragma omp parallel for reduction(min: min)
    for (int i = 0; i < dataLength; i++) {
        if ( min >  data[i])
            min = data[i];
    }
    return min;
}


int findbin(double d) {
    int i = total / 2;
    int index = -1;
    while (index ==-1) {
        if (i == 0 || i == total - 1) {
		   index = i;
		   break;
		}
        if (d <= maximums[i - 1]) {
            i = i / 2;
        }
        else if (d > maximums[i]) {
            i = (total + i) / 2;
        }
        else {
            index = i;
			 break;
        }
    }
    return index;
}

void compute_histogram( double *data, double min, double max, int count) {
    total = count;
    maximums = (double *) malloc(total * sizeof(double));
    counts = (int *) malloc(total* sizeof(int));

	for (int i=0; i<total* sizeof(int); i++)
{
  counts[i] = 0;
}


    double norm = (max - min) / total;
#pragma omp parallel for
    for (int i = 0; i < total; i++){
        maximums[i] = min + norm * (i + 1);
		
	}

    omp_lock_t *locks = (omp_lock_t *) malloc(sizeof(omp_lock_t*));
#pragma omp parallel for
    for (int i = 0; i < total; i++)
        omp_init_lock(&locks[i]);

#pragma omp parallel for 
    for (int i = 0; i < dataLength; i++) {
        int index = findbin(data[i]);
        omp_set_lock(&locks[index]);
        counts[index] = counts[index]+1;
        omp_unset_lock(&locks[index]);
    }

#pragma omp parallel for
    for (int i = 0; i < total; i++)
        omp_destroy_lock(&locks[i]);

}

int main(int argc, char *argv[]) {
	
    double data[25] = {1.3, 2.9, 0.4, 0.3, 1.3, 2.4, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4,3.9, 0.4,4.2, 4.5, 4.9, 0.9};
    double maxValue = maximum(data);
    double minValue = minimum(data);

    double start = omp_get_wtime();
     compute_histogram(data, minValue, maxValue, 5);
    double end = omp_get_wtime();
	float elapsedTime = start-end;

	printf("Bin counts:");
    for (int i = 0; i < total; i++) {
        printf( "%d\t", counts[i]);
    }
    printf( "\n\n");
    printf( "Bin max:");
    for (int i = 0; i < total; i++) {
      printf("%d\t", maximums[i]);
    }
  printf("\n\nElapsed time in milli seconds: %d\n", elapsedTime);
    return 0;
}

