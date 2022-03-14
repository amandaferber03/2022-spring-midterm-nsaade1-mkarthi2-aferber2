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
	printf("first");
	// Check number of command line arguments
	if (argc != 6) {
		printf("Error: not enough arguments provided");
		return 1;
	}

	// Seed the random number generator so that the code produces the same results on the same input.
    Image * img = NULL;
	srand(0);
    FILE *fp = fopen(argv[1], "r");

	if (fp == NULL) {
	  printf("Error: Unable to read file");
	  return 2;
	}
	
	img = ReadPPM(fp);

	fclose(fp);

	if(!img) {
	  //fclose(fp);
	  printf("Error: could not read file");
	  return 3;
	}

	Image * new_image = NULL;
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	int window_radius = atoi(argv[5]);
	printf("main width and height: %d %d", width, height);
	new_image = place_image(width, height, img);

	if(!new_image) {
		printf("Error: dimensions of exemplar exceed those of new image");
		return 1;
	}
	
	FILE *fp_write = fopen(argv[2], "w+");

	if (fp_write == NULL) {
	  printf("Error: Unable to write file");
	  return 4;
	}

	int pixel_count = WritePPM(fp_write, new_image);
	new_image = SynthesizeFromExemplar(img, width, height, window_radius);
	fclose(fp_write);

	// Get the time at the start of execution
	clock_t start_clock = clock();

	// TODO: IMPLEMENT THIS FUNCTION

	// Get the time at the end of the execution
	clock_t clock_difference = clock() - start_clock;

	// Convert the time difference into seconds and print
	printf( "Synthesized texture in %.2f(s)\n" , (double)clock_difference/CLOCKS_PER_SEC );
	return 0;
}

