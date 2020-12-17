#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

float dotprod(float * a, float * b, size_t N)
{
    int i;
    float sum;

    printf("Num of CPU: %d\n", omp_get_num_procs());
	omp_set_dynamic(0);     // Explicitly disable dynamic teams
	// omp_set_num_threads(4); // Use 4 threads for all consecutive parallel regions. 
	// export OMP_NUM_THREADS=4

#pragma omp parallel default(none) shared(a, b, N, sum)
{
#pragma omp for reduction(+:sum)
    for (i = 0; i < N; ++i)
    {
        sum += a[i] * b[i];
        printf("tid = %d i = %d\n", omp_get_thread_num(), i);
    }
}

    return sum;
}

int main (int argc, char *argv[])
{
    const size_t N = 100;
    int i;
    float sum;
    float a[N], b[N];

    for (i = 0; i < N; ++i)
    {
        a[i] = b[i] = (double)i;
    }

    sum = dotprod(a, b, N);

    printf("Sum = %f\n",sum);

    return 0;
}