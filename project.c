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
		printf("Error: Not enough arguments provided\n");
		return 1;
	}
	printf("second\n");
	
	// Seed the random number generator so that the code produces the same results on the same input.
	srand(0);
    FILE *fp = fopen(argv[1], "r");

	// Error reading the input file
	if (fp == NULL) {
	  printf("Error: Unable to read file of the exemplar image\n");
	  return 2;
	}
	
	// Setting fields in the Image struct representing the exemplar image
	Image * img = NULL;
	img = ReadPPM(fp);

	// Error creating the Image struct
	if(img == NULL) {
	  printf("Error: Unable to create a struct for the exemplar image\n");
	  return 3;
	}

	// Closing the input file
	fclose(fp);
	printf("third\n");
	
	// Storing the user inputted width, height, and window radius
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	int window_radius = atoi(argv[5]);
	printf("main width and height: %d %d", width, height);

	// Setting the fields of the struct representing the new image
	Image * new_image = NULL;

	 //Get the time at the start of execution
	clock_t start_clock = clock();

	// TODO: IMPLEMENT THIS FUNCTION

	new_image = SynthesizeFromExemplar(img, width, height, window_radius);

		// Get the time at the end of the execution
	clock_t clock_difference = clock() - start_clock;

	if(!new_image) {
		printf("Error: dimensions of exemplar exceed those of new image");
		return 1;
	}

	// Convert the time difference into seconds and print
	printf( "Synthesized texture in %.2f(s)\n" , (double)clock_difference/CLOCKS_PER_SEC );


	FILE *fp_write = fopen(argv[2], "w+");

	if (fp_write == NULL) {
		printf("Error: Unable to write file");
		return 4;
	}

	int pixel_count = WritePPM(fp_write, new_image);
	FreeImage(&new_image);

	fclose(fp_write);

	return 0;
}

