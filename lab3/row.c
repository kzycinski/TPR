#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <time.h>
#include <limits.h>

int getNumberOfPointsInCircle(int numberOfPoints) {
    double x,y,pi,point;
    int i, circle = 0;

    for(i = 0; i < numberOfPoints; i++){
        x = (double)rand()/(double)(RAND_MAX);
        y = (double)rand()/(double)(RAND_MAX);

        point = x * x + y * y;

        if(point <= 1){
            circle++;
        }
    }

    return circle;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("%s", "Wrong argument number, try ./row <number of points>\n");
        return 1;
    }
    char *ptr;
    int numberOfPoints = 0;
    numberOfPoints = strtol(argv[1], &ptr, 10);

    if(numberOfPoints < 1 || numberOfPoints > LLONG_MAX - 1) {
        printf("%s", "Wrong number of points\n");
        return 1;
    }

    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    srand(time(NULL)*world_rank);
    int i;
    int localSum;
    int numberOfPointsPerProc = numberOfPoints / world_size;
    double start, end;

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();
    localSum = getNumberOfPointsInCircle(numberOfPointsPerProc);
    end = MPI_Wtime();

    double elapsed = end - start;
    printf("Local  number of points %d - %d, avg = %d, time = %f\n",
           world_rank, localSum, localSum / numberOfPointsPerProc, elapsed);

    int globalSum;

    MPI_Reduce(&localSum, &globalSum, 1, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD);

    double pi = (double)4 * (double)globalSum / (double)numberOfPoints;
    MPI_Barrier(MPI_COMM_WORLD);

    if (world_rank == 0) {
        printf("Pi = %f\n", pi);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}
