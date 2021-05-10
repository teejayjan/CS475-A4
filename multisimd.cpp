#include "simd.h"

// tries for performance
#ifndef NUMTRIES
#define NUMTRIES 500
#endif

// array size
#ifndef ARRAY_SIZE
#define ARRAY_SIZE 100000
#endif

// threads
#ifndef NUMT
#define NUMT 4
#endif

// elements per core
#define NUM_ELEMENTS_PER_CORE ARRAY_SIZE/NUMT

// initialize arrays
float A[ARRAY_SIZE];
float B[ARRAY_SIZE];
float C[ARRAY_SIZE];

void
SimdMul( float *a, float *b,   float *c,   int len )
{
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	__asm
	(
		".att_syntax\n\t"
		"movq    -24(%rbp), %r8\n\t"		// a
		"movq    -32(%rbp), %rcx\n\t"		// b
		"movq    -40(%rbp), %rdx\n\t"		// c
	);

	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
			".att_syntax\n\t"
			"movups	(%r8), %xmm0\n\t"	// load the first sse register
			"movups	(%rcx), %xmm1\n\t"	// load the second sse register
			"mulps	%xmm1, %xmm0\n\t"	// do the multiply
			"movups	%xmm0, (%rdx)\n\t"	// store the result
			"addq $16, %r8\n\t"
			"addq $16, %rcx\n\t"
			"addq $16, %rdx\n\t"
		);
	}

	for( int i = limit; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}

int main (int argc, char *argv[])
{
    #ifndef _OPENMP
        fprintf(stderr, "No OpenMP Support!\n");
        return 1;
    #endif

    // initialize arrays
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        A[i] = 1;
        B[i] = 2;
    }


    // max performance with NUMT and SIMD
    omp_set_num_threads(NUMT);
    double maxPerformance = 0.;

    for (int t = 0; t < NUMTRIES; t++)
    {   
        double time0 = omp_get_wtime();
        #pragma omp parallel for
        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            // int first = omp_get_thread_num() * NUM_ELEMENTS_PER_CORE;
            // SimdMul(&A[first], &B[first], &C[first], NUM_ELEMENTS_PER_CORE);
            C[i] = A[i] * B[i];
        }
        double time1 = omp_get_wtime();

        double megaMults = (float)ARRAY_SIZE / (time1 - time0) / 1000000.;
        if (megaMults > maxPerformance)
            maxPerformance = megaMults;
    }
    fprintf(stdout, "array: %d, SIMD & multithread megaMults/sec: %6.2f\n", ARRAY_SIZE, maxPerformance);
    return 0;
}

