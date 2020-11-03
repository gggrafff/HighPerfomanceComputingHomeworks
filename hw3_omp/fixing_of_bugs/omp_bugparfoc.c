#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    const size_t N = 100;
    const size_t chunk = 3;

    int i;
    float a[N], b[N], c[N];

    for (i = 0; i < N; ++i)
    {
        a[i] = b[i] = (float)i;
    }

    printf("Num of CPU: %d\n", omp_get_num_procs());
	omp_set_dynamic(0);     // Explicitly disable dynamic teams
	// omp_set_num_threads(4); // Use 4 threads for all consecutive parallel regions. 
	// export OMP_NUM_THREADS=4

#pragma omp parallel \
    default(none) \
    shared(a,b,c)
    {
#pragma omp for schedule(static,chunk)
        for (i = 0; i < N; ++i)
        {
            c[i] = a[i] + b[i];
            printf("tid = %d, c[%d] = %f\n", omp_get_thread_num(), i, c[i]);
        }
    } 

    return 0;
}