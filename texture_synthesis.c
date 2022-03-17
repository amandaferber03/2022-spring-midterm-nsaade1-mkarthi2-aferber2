#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <float.h>
#include "texture_synthesis.h"
#include "image.h"


/* This function creates a pixel window for TBS Pixel evaluation */
Pixel * create_TBS_pixel_window(int r, TBSPixel TBSPixel, Pixel * pixels, int width, int height) {
	printf("create_TBS_pixel_window\n");
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
	      pixel_window[counter] = pixels[i];
          counter++;
	    }
	  }
	}
	      
	return pixel_window;

}


/*This function creates a window to evaluate the exemplar pixels*/
Pixel * create_exemplar_window(int r, int index, int width, int height, Pixel * pixels) { //to be used in a for loop iterating through the exemplar image pixels
	printf("create_exemplar_window\n");
	int window_width = 2 * r + 1; //Calculating window width
	int window_height = 2 * r + 1;//Calculating window height

    Pixel * pixel_window = (Pixel *) malloc(sizeof(Pixel) * window_width * window_height);

    int exemp_x  = index  % width; //Calculating x position
    int exemp_y  = index / width; //Calculating y position

    int counter = 0; //index within window

	int x_top_left = exemp_x - r;
    int y_top_left = exemp_y - r;

    for(int i = x_top_left; i <= (exemp_x + r); i++) {
        for(int j = y_top_left; j <= (exemp_y + r); j++) {
            if ((i <= 0 && i >= width) || (j <= 0 && j >= height)) {
				// alpha = -1 to indicate that the pixel is out of bounds
            	Pixel pixel_out_bounds = {0, 0, 0, 50}; 
            	pixel_window[counter] = pixel_out_bounds;
            	counter++;
            }
	    	else {
		  pixel_window[counter] = pixels[i];
		  counter++; 
	    	}
	    }
    }

    return pixel_window; //return generated window
}


double find_difference(Pixel * tbs_pixel_window, Pixel * exemp_pixel_window, int r) {
	printf("find_difference\n");
	//Initialization:
	printf("find_difference\n");
	double diff = 0;
	double d = 0;
	double sigma = (2 * r + 1) / 6.4;
	double s = 0;


	int counter = 0;

	for (int i = -r; i <= r; i++) {
		for (int j = -r; j <= r; j++) {
		  printf("%d %d counter: %d\n", i, j, counter);
			Pixel tbs_pixel = tbs_pixel_window[counter];
			Pixel exemp_pixel = exemp_pixel_window[counter];

			// checks if the tbs pixel and exemp pixel is set
			if (tbs_pixel.a == 255 && exemp_pixel.a == 255) {
				d = PixelSquaredDifference(tbs_pixel, exemp_pixel);
				s = exp( -(i*i + j*j)/(2*sigma*sigma)); 
				diff += d*s;
			}
			// the exemp pixel is out of bounds so we don't compare the tbs_pixel_window to exemp_pixel_window
			else if (tbs_pixel.a == 255 && exemp_pixel.a == 50) {
				return -1.0;
			}
			counter++;

		}
	}
	printf("success!\n");
	if(diff == 0) {
	  for (int i = 0; i < 25; i++) {
	    printf("TBS Pixel: %u %u %u %u\n", tbs_pixel_window[i].r, tbs_pixel_window[i].g, tbs_pixel_window[i].b, tbs_pixel_window[i].a);
	    printf("Exemp Pixel: %u %u %u %u\n", exemp_pixel_window[i].r, exemp_pixel_window[i].g, exemp_pixel_window[i].b, exemp_pixel_window[i].a);
	  }
	}
	assert(diff != 0);
	return diff;

}
	


PixelDiff * compare_windows(Pixel * tbs_pixel_window, Image * img, const Image * exemp, int r) {
//what should compare_windows return
  Pixel * pixels_array = img->pixels;
  int width = img->width;
  int height = img->height;
  int exemp_width = exemp->width;
  int exemp_height = exemp->height;
  int exemp_count = 0; //This is the index to help us traverse through the exemplar image using a 1-D for loop
  int exemp_counter = 0; //Counts every exemplar pixel
  int diff_array_index = 0; // current index of diff_array

  // difference between a TBS window and exemplar window
  double diff_of_windows = 0;

  // array of differences between TBS window and all exemplar windows
  PixelDiff * diff_array = (PixelDiff *) malloc(sizeof(PixelDiff) * exemp->width * exemp->height);

  
  //for (int i = 0; i < TBSPixel_arr_size; i++) { //For every TBS Pixel
    //Pixel * tbs_pixel_window = create_TBS_pixel_window(r, tbs_pixels[i], pixels_array, width, height); //We create a window
  //for(int j = 0; j < width * height; j++) { //We compare it to every exemplar pixel
    //if(j >= (exemp_count*width) && j <= (exemp_count*width + exemp_width -1) && !(exemp_count >= exemp_height)) {
      //exemp_counter++;
      //Pixel * exemp_pixel_window = create_exemplar_window(r, j, width, height, pixels_array);
      //diff_of_windows = find_difference(tbs_pixel_window, exemp_pixel_window, r);
      //PixelDiff exemp = {pixels_array[j], diff_of_windows};
      //diff_array[j] = exemp;
      //}
    // }

  for(int i = 0; i < width; i++) {
    for(int j = 0; j < height; j++) {
      int pos = (width * j) + i; 
      if(pos >= (j*width) && pos <= (j*width + exemp_width -1) && !(j >= exemp_height)) {
	Pixel * exemp_pixel_window = create_exemplar_window(r, pos, width, height, pixels_array);
	diff_of_windows = find_difference(tbs_pixel_window, exemp_pixel_window, r);
	PixelDiff exemp = {pixels_array[pos], diff_of_windows};
	diff_array[diff_array_index] = exemp;
	diff_array_index++;
	exemp_counter++;
      }
    }
  }
    printf("Exemp counter: %d\n", exemp_counter);
    return diff_array;
}

/**Function to find the minimum difference between unset and pixel from exemplar*/
PixelDiff find_minimum_difference(PixelDiff * diff_array, int exemp_width, int exemp_height) {
	printf("find min dif");
	double min = diff_array[0].diff;
	int num_elements = exemp_width * exemp_height;

	//printf("before first loop\n");
	for (int i = 1; i < num_elements; i++) {//CHANGE THIS
		if (diff_array[i].diff < min) {
		  if(diff_array[i].diff >= 0) {
			min = diff_array[i].diff;
		  }
		}
	}
        //printf("before thresh\n");
	//printf("Min: %f\n", min);
	double threshold = min * 1.1;
        //printf("before cap\n");
	int capacity = 1;
	//printf("before thresh_arr malloc\n");
	PixelDiff * threshold_arr = malloc(sizeof(PixelDiff) * capacity);
	int counter = 0;

	//printf("before second loop\n");
	for (int i = 0; i < num_elements; i++) {
	  //printf("before first if\n");
		if (diff_array[i].diff < threshold) {
			threshold_arr[counter].pixel = diff_array[i].pixel; 
			threshold_arr[counter].diff = diff_array[i].diff;

			counter++; 
		}
		//printf("before second if\n");
		if (counter == capacity && i != num_elements - 1) {
			capacity++;
			threshold_arr = realloc(threshold_arr, sizeof(PixelDiff) * capacity);
		}
	}

	printf("Min: %f\n", min);
	printf("Diff of first: %f\n", diff_array[0].diff);
	printf("Counter: %d\n", counter);
	printf("before random\n");
	int random_index = rand() % counter; // 0 to counter - 1
	printf("%d\n", random_index);
	printf("before return\n");
	return threshold_arr[random_index];

}

/*Function for setting up TBS Pixels*/
Image * set_TBS_Pixels(int TBSPixel_arr_size, TBSPixel * tbs_pixels, int r, Image * img, const Image * exemp){
	printf("set_TBS_Pixels\n");
	Pixel * pixels = img->pixels;
	for (int i = 0; i < TBSPixel_arr_size; i++) {
	        //Creating pertinent variables and initializing them
		Pixel * tbs_pixel_window = create_TBS_pixel_window(r, tbs_pixels[i], pixels, img->width, img->height);
		PixelDiff * pixel_diffs = compare_windows(tbs_pixel_window, img, exemp, r);
		PixelDiff rand_exemp = find_minimum_difference(pixel_diffs, exemp->width, exemp->height);
		Pixel for_tbs_pixel = rand_exemp.pixel;

		//Calculating index for setting
		int x = tbs_pixels[i].idx.x;
		int y = tbs_pixels[i].idx.y;
		int pos = (img->width * y) + x;

		//Finally, we set up the pixels
		pixels[pos].r = for_tbs_pixel.r;
		pixels[pos].g = for_tbs_pixel.g;
		pixels[pos].b = for_tbs_pixel.b;
		pixels[pos].a = for_tbs_pixel.a;
	}

	//After exiting the loop, we return the generated image
	return img;

}

	

/*Function that counts the amount of set neighboring pixel for each new image pixel, and stores the amounts in arrays*/
TBSPixel * count_neighbors(Image * new_img, const Image * exemplar) {
	printf("count_neighbors\n");
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

/*Helper function that determines the pixel position within the image*/
int determine_position(unsigned int i, unsigned int width, unsigned int height) {
	printf("determine_position\n");
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


/*Helper function that counts neighbors for top pixels*/
void count_for_top(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	printf("count_for_top\n");
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

/*Helper function that counts neighbors for bottom pixels*/
void count_for_bottom_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	printf("count_for_bottom\n");
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

/*Helper function that counts neighbors for bottom left pixels*/
void count_for_bottom_left(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	printf("count_for_bottom_left\n");
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

/*Helper function that counts neighbors for bottom pixels*/
void count_for_bottom(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	printf("count_for_bottom\n");
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


/*Helper function that counts neighbors for top right pixels*/
void count_for_top_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	printf("count_for_top_right\n");
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

/*Helper function that counts neighbors for right pixels*/
void count_for_right(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	printf("count_for_right\n");
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

/*Helper function that counts neighbors for left pixels*/
void count_for_left(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	printf("count_for_left\n");
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

/*Helper function that counts neighbors for center pixels*/
void count_for_other(unsigned int i, Pixel * pixel, int * tbs_neighbor_tracker, unsigned int width) {
	printf("count_for_other\n");
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
	printf("place_image\n");
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
	printf("CompareTBSPixels\n");
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
	printf("SortTBSPixels\n");
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
	printf("SynthesizeFromExemplar\n");
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
	
	SortTBSPixels(neighbor_counts, TBSPixel_arr_size);

	new_image = set_TBS_Pixels(TBSPixel_arr_size, neighbor_counts, windowRadius, new_image, exemplar); 


	return new_image;
}
