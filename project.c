#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "image.h"
#include "ppm.h"
#include "texture_synthesis.h"

int main( int argc , char *argv[] )
{
	// Seed the random number generator so that the code produces the same results on the same input.
	srand(0);

	// Get the time at the start of execution
	clock_t start_clock = clock();

	// TODO: IMPLEMENT THIS FUNCTION

	// Get the time at the end of the execution
	clock_t clock_difference = clock() - start_clock;

	// Convert the time difference into seconds and print
	printf( "Synthesized texture in %.2f(s)\n" , (double)clock_difference/CLOCKS_PER_SEC );
	return 0;
}

