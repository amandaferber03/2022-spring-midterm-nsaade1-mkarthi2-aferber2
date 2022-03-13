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
	//Chech number of command line arguments
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
	int width = argv[3];
	int height = argv[4]; 
	new_image = placeImage(width, height, img);

	if(!new_image) {
		printf("Error: dimensions of exemplar exceed those of new image");
		return 1;
	}

	

	//for(int i = 0; i < (width * height); i++) {
	//Pixel * pixels = img->pixels;//assigning to an array of Pixel structs img->pixels                                      
	//unsigned int red = (pixels[i]).r;
	//unsigned int green = (pixels[i]).g;
	//unsigned int blue = (pixels[i]).b;
          //printf("%u %u %u/n", red, green, blue);
	//}
	
	FILE *fp_write = fopen(argv[2], "w+");

	if (fp_write == NULL) {
	  printf("Error: Unable to write file");
	  return 4;
	}
	
	int pixel_count = WritePPM(fp_write, new_image);
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

