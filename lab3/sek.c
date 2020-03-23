#include <stdio.h>
#include <stdlib.h>

double estimatePi(long long numberOfPoints) {
    double x,y,pi,point;
    long long i, circle = 0;

    for(i = 0; i < numberOfPoints; i++){
        x = (double)rand()/(double)(RAND_MAX);
        y = (double)rand()/(double)(RAND_MAX);

        point = x * x + y * y;

        if(point <= 1){
            circle++;
        }
    }

    pi = (double)4 * (double)circle / (double)numberOfPoints;

    return pi;

}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("%s", "Wrong argument number, try ./sek <number of points>\n");
        return 1;
    }
    char *ptr;
    long long numberOfPoints = 0;
    numberOfPoints = strtol(argv[1], &ptr, 10);

    if(numberOfPoints < 1 || numberOfPoints > LLONG_MAX - 1) {
        printf("%s", "Wrong number of points\n");
        return 1;
    }
    int i;
    double pi;

    for(i = 0; i < 10; i++){
        pi = estimatePi(numberOfPoints);
        printf("%.10f\n", pi);
    }


}
