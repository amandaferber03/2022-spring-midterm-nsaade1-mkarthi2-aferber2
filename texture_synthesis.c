#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "texture_synthesis.h"

Pixel * create_TBS_pixel_window(int r, TBSPixel TBSPixel, Pixel * pixels, int width, int height) {
	int window_width = 2 * r + 1;
	int window_height = 2 * r + 1;

	Pixel * pixel_window = (Pixel *) malloc(sizeof(Pixel) * window_width * window_height);
	int x = TBSPixel.idx.x;
	int y = TBSPixel.idx.y;

	int counter = 0;

	int x_top_left = x - r;
	int y_top_left = y - r;

	for(int i = x_top_left; i <= (x + r); i++) {
	  for(int j = y_top_left; j <= (y + r); j++) {
	    if ((i <= 0 && i >= width) || (j <= 0 && j >= height)) {
	      Pixel pixel_out_bounds = {0, 0, 0, 0};
	      pixel_window[counter] = pixel_out_bounds;
	      counter++;
	    }
	    else { 
	      int position = (width * j) + i;
	      printf("%d: %d %d\n", counter, i, j); 
	      pixel_window[counter] = pixels[i];
              counter++;
	    }
	  }
	}
	      
	return pixel_window;

}


Pixel * create_exemplar_window(int r, int index, int width, int height, Pixel * pixels) { //to be used in a for loop iterating through the exemplar image pixels

	int window_width = 2 * r + 1;
        int window_height = 2 * r + 1;

        Pixel * pixel_window = (Pixel *) malloc(sizeof(Pixel) * window_width * window_height);

	int exemp_x  = index  % width;
        int exemp_y  = index / width;

	int counter = 0;

	int x_top_left = exemp_x - r;
        int y_top_left = exemp_y - r;

        for(int i = x_top_left; i <= (exemp_x + r); i++) {
          for(int j = y_top_left; j <= (exemp_y + r); j++) {
            if ((i <= 0 && i >= width) || (j <= 0 && j >= height)) {
              Pixel pixel_out_bounds = {0, 0, 0, 0};
              pixel_window[counter] = pixel_out_bounds;
                counter++;
            }
	    else {
            int position = (width * j) + i;
            pixel_window[counter] = pixels[i];
	    printf("%d: %d %d\n", counter, i, j);
            counter++;
	    }

	    }
        }

        return pixel_window;
}
	


void compare_windows(TBSPixel * tbs_pixels, Image * img, Image * exemp, int r, int TBSPixel_arr_size) {
  //what should compare_windows return
  Pixel * pixels_array = img->pixels;
  int width = img->width;
  int height = img->height;
  int exemp_width = exemp->width;
  int exemp_height = exemp->height;
  int exemp_count = 0; //This is the index to help us traverse through the exemplar image using a 1-D for loop
  int exemp_counter = 0; //Counts every exemplar pixel 

  
  for (int i = 0; i < TBSPixel_arr_size; i++) { //For every TBS Pixel
    Pixel * tbs_pixel_window = create_TBS_pixel_window(r, tbs_pixels[i], pixels_array, width, height); //We create a window
    for(int j = 0; j < width * height; j++) { //We compare it to every exemplar pixel
      if(j >= (exemp_count*width) && j <= (exemp_count*width + exemp_width -1) && !(exemp_count >= exemp_height)) {
	  exemp_counter++;
	  Pixel * exemp_pixel_window = create_exemplar_window(r, j, width, height, pixels_array);
          if (exemp_counter % exemp->width == 0) {
	     exemp_count++; 
	  }
	  //TO DO - implement helper function to do comparison between corresponding pixels in both windows
      }
    }
  }
}

TBSPixel * count_neighbors(Image * new_img, const Image * exemplar) {

	// determining the size of the TBSPixel array
	unsigned int total_exemplar_pixels = exemplar->width * exemplar->height;
	unsigned int total_new_img_pixels = new_img->width * new_img->height;
	unsigned int total_tbs_pixels = total_new_img_pixels - total_exemplar_pixels;

	// variables for Pixel array and TBSPixels array
	Pixel * pixels = new_img->pixels;
	TBSPixel * tbs_pixels = (TBSPixel*) malloc(sizeof(TBSPixel) * total_tbs_pixels);

	// other important variables
	int TBSPixel_index = 0;
	int pixel_col = 0;
	int pixel_row = 0;
	int position = 0;
	int neighbor_count = 0;

	// loop that will iterate through all of the pixels in new_img
	for (unsigned int i = 0; i < total_new_img_pixels; i++) {
		if (pixels[i].a == 0) {
			pixel_col = i % new_img->width;
			pixel_row = i / new_img->width;
			position = determine_position(i, new_img->width, new_img->height);
			switch (position) {
				case 0:
					count_for_top(i, pixels, &neighbor_count, new_img->width);
					break;
				case 1:
					count_for_bottom_right(i, pixels, &neighbor_count, new_img->width);
					break;
				case 2:
					count_for_bottom_left(i, pixels, &neighbor_count, new_img->width);
					break;
				case 3:
					count_for_bottom(i, pixels, &neighbor_count, new_img->width);
					break;
				case 4:
					count_for_top_right(i, pixels, &neighbor_count, new_img->width);
					break;
				case 5:
					count_for_right(i, pixels, &neighbor_count, new_img->width);
					break;
				case 6:
					count_for_left(i, pixels, &neighbor_count, new_img->width);
					break;
				case 7:
					count_for_other(i, pixels, &neighbor_count, new_img->width);
					break;
			}

			tbs_pixels[TBSPixel_index].idx.x = pixel_col;
			tbs_pixels[TBSPixel_index].idx.y = pixel_row;
			tbs_pixels[TBSPixel_index].neighborCount = neighbor_count;

			neighbor_count = 0;
			TBSPixel_index++;

		}
	}

	return tbs_pixels;
}

int determine_position(unsigned int i, unsigned int width, unsigned int height) {
	if (i + width >= width * height) {
		// bottom right
		if (i + 1 == width * height) {
			return 1;
		}
		// bottom left
		else if (i == width * height - width) {
			return 2;
		}
		// bottom
		else {
			return 3;
		}
	}
	else if ((i + 1) % width == 0) {
		// top right
		if (i + 1 == width) {
			return 4;
		}
		// right
		else {
			return 5;
		}
	}
	else if (i < width) {
		// top
		return 0;
	}
	else if (i % width == 0) {
		// left
		return 6;
	}
	else {
		// other
		return 7;
	}
}

void count_for_top(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - 1].a == 255) { // left pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width - 1].a == 255) { // bottom left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width + 1].a == 255) { // bottom right
		(*tbs_neighbor_tracker)++;
	}
}

void count_for_bottom_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width - 1].a == 255) { // top left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - 1].a == 255) { // left
		(*tbs_neighbor_tracker)++;
	}
}

void count_for_bottom_left(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width + 1].a == 255) { // top right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
}

void count_for_bottom(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - width].a == 255) { // top
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width + 1].a == 255) { // top right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width - 1].a == 255) { // top left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - 1].a == 255) { // left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
}

void count_for_top_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - 1].a == 255) { // left pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width - 1].a == 255) { // bottom left
		(*tbs_neighbor_tracker)++;
	}
}

void count_for_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width - 1].a == 255) { // top left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - 1].a == 255) { // left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width - 1].a == 255) { //bottom left
		(*tbs_neighbor_tracker)++;
	}
}

void count_for_left(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width + 1].a == 255) { // top right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width + 1].a == 255) { // bottom right
		(*tbs_neighbor_tracker)++;
	}
}

void count_for_other(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	if (pixel[i - width].a == 255) { // top pixel
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width + 1].a == 255) { // top right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - width - 1].a == 255) { //top left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i - 1].a == 255) { // left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + 1].a == 255) { // right
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width].a == 255) { // bottom
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width - 1].a == 255) { // bottom left
		(*tbs_neighbor_tracker)++;
	}
	if (pixel[i + width + 1].a == 255) { // bottom right
		(*tbs_neighbor_tracker)++;
	}
}


Image * place_image(unsigned int width, unsigned int height, const Image * exemplar_image) {

	// checking if the width and height of the new image are greater
	// than the width and the height of the exemplar image
	if ((width < exemplar_image->width) || (height < exemplar_image->height)) {
    	return NULL;
  	}
  
	// setting the width and the height of the new image
	Image * new_img = (Image *) malloc(sizeof(Image));
	new_img->width = width;
	new_img->height = height;
	new_img->pixels = (Pixel *) malloc(sizeof(Pixel) * width * height);

	// creating pointers to Pixel arrays in the new image and exemplar image
	Pixel * new_pixels = new_img->pixels;
	Pixel * exemplar_pixels = exemplar_image->pixels;

	// ASK AMANDA TO CHANGE VARIABLE NAMES
  	unsigned int count = 0;
  	unsigned int counter = 0; 

	// "copies" the exemplar image into the new image
	// the remaining pixels will be black
	for(unsigned int i = 0; i < width * height; i++) {
	
		if (i >= (count*width) && i <= (count * width + exemplar_image->width - 1) && !(count >= exemplar_image->height)) {
			int exemp_pos = i - count*(((count+1)*(width) -1) - ((count)*(width) + exemplar_image->width -1));
			(new_pixels[i]).r = (exemplar_pixels[exemp_pos]).r;
			(new_pixels[i]).g = (exemplar_pixels[exemp_pos]).g;
			(new_pixels[i]).b = (exemplar_pixels[exemp_pos]).b;
			(new_pixels[i]).a = 255; 
	  	}
		else {
	    	(new_pixels[i]).r = 0;
        	(new_pixels[i]).g = 0;
        	(new_pixels[i]).b = 0;
			(new_pixels[i]).a = 0;
	  	}
		counter++;
		if (counter % width == 0) {
			count++;
	  	}
  }
  return new_img;
  
}

// compares tbs pixels 
int CompareTBSPixels( const void *v1 , const void *v2 )
{
	const TBSPixel *tp1 = (const TBSPixel *)v1;
	const TBSPixel *tp2 = (const TBSPixel *)v2;
	int d = tp1->neighborCount - tp2->neighborCount;
	if( d ) return -d;
	d = tp1->idx.y - tp2->idx.y;
	if( d ) return d;
	d = tp1->idx.x - tp2->idx.x;
	if( d ) return d;
	return tp2->r - tp1->r;
}

// sorts tbs pixels, returns zero if succeeded
int SortTBSPixels( TBSPixel *tbsPixels , unsigned int sz )
{
	unsigned int *permutation = (unsigned int*)malloc( sizeof(unsigned int)*sz );
	if( !permutation )
	{
		fprintf( stderr , "[ERROR] Failed to allocate memory for permutation: %d\n" , sz );
		return 1;
		exit(1);
	}
	for( unsigned int i=0 ; i<sz ; i++ ) permutation[i] = i;
	for( unsigned int i=0 ; i<sz ; i++ )
	{
		unsigned int i1 = rand() % sz;
		unsigned int i2 = rand() % sz;
		unsigned int tmp = permutation[i1];
		permutation[i1] = permutation[i2];
		permutation[i2] = tmp;
	}

	for( unsigned int i=0 ; i<sz ; i++ ) tbsPixels[i].r = permutation[i];
	free( permutation );

	qsort( tbsPixels , sz , sizeof( TBSPixel ) , CompareTBSPixels );

	return 0;
}


Image *SynthesizeFromExemplar( const Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius) // bool verbose 
{
	Image * new_image = place_image(outWidth, outHeight, exemplar);
	
	if (new_image == NULL) {
		printf("Error: Unable to generate a pointer to the new image.\n");
		return NULL;
	}

	TBSPixel * neighbor_counts = count_neighbors(new_image, exemplar);

	if (neighbor_counts == NULL) {
		printf("Error: Unable to generate a pointer to a TBSPixel array.\n");
		return NULL;
	}
	
	// SortTBSPixels successful
	int exemplar_pixels = exemplar->width * exemplar->height;
	int new_pixels = new_image->width * new_image->height;
	unsigned int TBSPixel_arr_size = new_pixels - exemplar_pixels;
	
	if (SortTBSPixels(neighbor_counts, TBSPixel_arr_size) == 0) {
		printf("Success!\n");
	}
	else {
		printf("Failure :(\n");
	}

	//Pixel * tbs_window = create_TBS_pixel_window(windowRadius, neighbor_counts[10], new_image->pixels, outWidth, outHeight);
	Pixel * exemplar_window = create_exemplar_window(windowRadius, 275, outWidth, outHeight, new_image->pixels);                              

	return new_image;
}
