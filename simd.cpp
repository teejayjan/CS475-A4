#include <stdio.h>
#include <xmmintrin.h>
#include "simd.h"
#include <omp.h>
#define SSE_WIDTH 4

// tries for performance
#ifndef NUMTRIES
#define NUMTRIES 500
#endif

// array size
#ifndef ARRAY_SIZE
#define ARRAY_SIZE 100000
#endif

// initialize arrays
float cA[ARRAY_SIZE];
float cB[ARRAY_SIZE];
float cC[ARRAY_SIZE];

float sA[ARRAY_SIZE];
float sB[ARRAY_SIZE];
float sC[ARRAY_SIZE];

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
        cA[i] = 1;
        cB[i] = 2;
    }

    double maxPerformance = 0;

    // max performance with C/C++
    for (int tries = 0; tries < NUMTRIES; tries++)
    {
        double time0 = omp_get_wtime();

        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            cC[i] = cA[i] * cB[i];
        }

        double time1 = omp_get_wtime();
        double megaMultsPerSecond = (double)ARRAY_SIZE / (time1 - time0) / 1000000;
        if (megaMultsPerSecond > maxPerformance)
        {
            maxPerformance = megaMultsPerSecond;
        }
    }
    fprintf(stdout, "array: %d, vanilla megaMults/sec: %6.2f\n", ARRAY_SIZE, maxPerformance);

    // initialize arrays
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        sA[i] = 1;
        sB[i] = 2;
    }

    maxPerformance = 0;

    // max performance with SIMD

    for (int tries = 0; tries < NUMTRIES; tries++)
    {
        double time0 = omp_get_wtime();

        SimdMul(sA, sB, sC, ARRAY_SIZE);

        double time1 = omp_get_wtime();
        double megaMultsPerSecond = (double)ARRAY_SIZE / (time1 - time0) / 1000000;
        if (megaMultsPerSecond > maxPerformance)
        {
            maxPerformance = megaMultsPerSecond;
        }
    }
    fprintf(stdout, "array: %d, SIMD megaMults/sec: %6.2f\n", ARRAY_SIZE, maxPerformance);
    return 0;
}

