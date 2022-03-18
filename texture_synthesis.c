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

    
 
Pixel * create_TBS_pixel_window(int r, TBSPixel TBSPixel, Pixel * pixels, int width, int height) {
	
	int window_width = 2 * r + 1;
	int window_height = 2 * r + 1;

	Pixel * pixel_window = (Pixel *) malloc(sizeof(Pixel) * window_width * window_height); //freed in set tbs pixel
	int x = TBSPixel.idx.x;
	int y = TBSPixel.idx.y;

	//if(y==2) {
	//printf("yes, third row");
	//}

	//printf("coords: %d %d\n", x, y);

	int counter = 0;

	int x_top_left = x - r;
	int y_top_left = y - r;

	for(int i = x_top_left; i <= (x + r); i++) {
	  for(int j = y_top_left; j <= (y + r); j++) {
	    if (i < 0) {
	      Pixel pixel_out_bounds = {0, 0, 0, 0};
	      pixel_window[counter] = pixel_out_bounds;
	      counter++;
	    }
	    else if ( i >= width) {
	      Pixel pixel_out_bounds = {0, 0, 0, 0};
              pixel_window[counter] = pixel_out_bounds;
              counter++;
	    }
	    else if (j < 0) {
	      Pixel pixel_out_bounds = {0, 0, 0, 0};
              pixel_window[counter] = pixel_out_bounds;
              counter++;
	    }
	    else if ( j >= height) {
	      Pixel pixel_out_bounds = {0, 0, 0, 0};
              pixel_window[counter] = pixel_out_bounds;
              counter++;
	    }
	    else {
	      int pos = (width * j) + i;
	      pixel_window[counter] = pixels[pos];
              counter++;
	    }
	  }
	}
	      
	return pixel_window;

}


Pixel * create_exemplar_window(int r, int index, int width, int height, Pixel * pixels, Image * exemplar) { //to be used in a for loop iterating through the exemplar image pixels
  int window_width = 2*r+1;
  int window_height = 2*r+1;

    int exemp_width = exemplar->width;
    int exemp_height = exemplar->height;
 
    Pixel * pixel_window = (Pixel *) malloc(sizeof(Pixel) * window_width * window_height);//freed this in compare windows

    int exemp_x  = index  % width;
    int exemp_y  = index / width;

    // printf("coords: %d %d\n", exemp_x, exemp_y);

    
    int counter = 0;

    int x_top_left = exemp_x - r;
    int y_top_left = exemp_y - r;

    for(int i = x_top_left; i <= (exemp_x + r); i++) {
        for(int j = y_top_left; j <= (exemp_y + r); j++) {
	  int pos = (width * j) + i;
            if (i < 0) {
              Pixel pixel_out_bounds = {0, 0, 0, 0};
              pixel_window[counter] = pixel_out_bounds;
              counter++;
	      if(pixels[index].r == 255) {
		//printf("i is %d. i < 0: yes\n", i);
              }
            }
            else if ( i >= width) {
              Pixel pixel_out_bounds = {0, 0, 0, 0};
              pixel_window[counter] = pixel_out_bounds;
              counter++;
	      if(pixels[index].r == 255) {
		//printf("i is %d. i > width: yes\n", i);
              }
            }
            else if (j < 0) {
              Pixel pixel_out_bounds = {0, 0, 0, 0};
              pixel_window[counter] = pixel_out_bounds;
              counter++;
	    }

            else if ( j >= height) {
              Pixel pixel_out_bounds = {0, 0, 0, 0};
              pixel_window[counter] = pixel_out_bounds;
              counter++;
	    }

            else {
              int pos = (width * j) + i;
	      if (pixels[index].r == 255) {
		//printf("YES: %d %d\n", i, j);
	      }
              pixel_window[counter] = pixels[pos];
              counter++;
	    }
	}
    }

    return pixel_window;
}

//the problem is between find_difference. for some reason, valid exemplar pixels are marked as not set to 255. additionally some pixels in the second row are returning a difference greater than zero

double find_difference(Pixel * tbs_pixel_window, Pixel * exemp_pixel_window, int r, Pixel  * pixels, int index) {
  //printf("find_difference\n");
	double diff = 0;
	double d = 0;
	double sigma = (2 * r + 1) / 6.4;
	double s = 0;


	int counter = 0;

	for (int i = -r; i <= r; i++) {
		for (int j = -r; j <= r; j++) {
		  //printf("%d %d counter: %d\n", i, j, counter);
			Pixel tbs_pixel = tbs_pixel_window[counter];
			Pixel exemp_pixel = exemp_pixel_window[counter];

			// checks if the tbs pixel and exemp pixel is set
			if (tbs_pixel.a == 255 && exemp_pixel.a == 255) {
			        d = PixelSquaredDifference(tbs_pixel, exemp_pixel);
				s = exp( -(i*i + j*j)/(2*sigma*sigma)); 
				diff += d*s;
				if(pixels[index].r == 255 && d != 0) {
				  //printf("index of faulty pixel: %d\n", index);
				}
				}
		       

			else if(exemp_pixel.a != 255) {
			  return -1;
			}
			counter++;

		}
	}
	//if (pixels[index].r == 255) {
	//printf("white center diff: %f\n", diff);
	//}
	return diff; 
}
	


PixelDiff * compare_windows(Pixel * tbs_pixel_window, Image * img, Image * exemp, int r) {
	
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
  PixelDiff * diff_array = (PixelDiff *) malloc(sizeof(PixelDiff) * exemp->width * exemp->height); //freed in find min difference
  Pixel * exemp_pixel_window = NULL;

  for(int i = 0; i < width; i++) {
    for(int j = 0; j < height; j++) {
      int pos = (width * j) + i; 
      if(pos >= (j*width) && pos <= (j*width + exemp_width -1) && !(j >= exemp_height)) {
	//printf("pos: %d\n", pos);
	exemp_pixel_window = create_exemplar_window(r, pos, width, height, pixels_array, exemp);
	diff_of_windows = find_difference(tbs_pixel_window, exemp_pixel_window, r, pixels_array, pos);
	PixelDiff exemp_pixel_diff = {pixels_array[pos], diff_of_windows};
	diff_array[diff_array_index] = exemp_pixel_diff;
	diff_array_index++;
	free(exemp_pixel_window);
      }
    }
  }
    return diff_array;
}

PixelDiff find_minimum_difference(PixelDiff * diff_array, int exemp_width, int exemp_height) {
  int x = 0;
  while(diff_array[x].diff < 0) {
  x++;
  }
	double min = diff_array[x].diff;
	int num_elements = exemp_width * exemp_height;

	//printf("before first loop\n");
	for (int i = 0; i < num_elements; i++) {
		if (diff_array[i].diff < min) {
		  if(diff_array[i].diff >= 0) {
			min = diff_array[i].diff;
		  }
		}
		//printf("diff: %f", diff_array[i].diff);
	}
        //printf("before thresh\n");
	//printf("Min: %f\n", min);
	double threshold = min * 1.1;
	//printf("threshold = %f\n", threshold);
        //printf("before cap\n");
	int capacity = 1;
	//printf("before thresh_arr malloc\n");
	PixelDiff * threshold_arr = malloc(sizeof(PixelDiff) * capacity);//freed in this function
	int counter = 0;

	//printf("before second loop\n");
	for (int i = 0; i < num_elements; i++) {
	  //printf("before first if\n");
		if (diff_array[i].diff <= threshold) {
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

	//printf("Min: %f\n", min);
	//printf("Diff of first: %f\n", diff_array[0].diff);
	//printf("Counter: %d\n", counter);
	//printf("before random\n");
	int random_index = rand() % counter; // 0 to counter - 1
	//printf("%d\n", random_index);
	//printf("before return\n");
	free(diff_array); //
	PixelDiff random_exemp = threshold_arr[random_index];
	free(threshold_arr);
	return random_exemp;

}

Image * set_TBS_Pixels(int TBSPixel_arr_size, TBSPixel * tbs_pixels, int r, Image * img, Image * exemp){
  //printf("set_TBS_Pixels\n");
	Pixel * pixels = img->pixels;
	int count = 0;
	int counter = 0;
	for (int i = 0; i < TBSPixel_arr_size; i++) {
		Pixel * tbs_pixel_window = create_TBS_pixel_window(r, tbs_pixels[i], pixels, img->width, img->height);
		PixelDiff * pixel_diffs = compare_windows(tbs_pixel_window, img, exemp, r);
		PixelDiff rand_exemp = find_minimum_difference(pixel_diffs, exemp->width, exemp->height);
		Pixel for_tbs_pixel = rand_exemp.pixel;
		int x = tbs_pixels[i].idx.x;
		int y = tbs_pixels[i].idx.y;
		//printf("HOLA!");
		int pos = (img->width * y) + x; 
		pixels[pos].r = for_tbs_pixel.r;
		pixels[pos].g = for_tbs_pixel.g;
		pixels[pos].b = for_tbs_pixel.b;
		pixels[pos].a = for_tbs_pixel.a;
		count++;
		if(pixels[pos].r == 255) {
		  counter++;
		}
		//printf("Count: %d", count);
		free(tbs_pixel_window); //
	}
	printf("counter: %d", counter);
	return img;

}

	

TBSPixel * count_neighbors(Image * new_img,  Image * exemplar, int * num_neighbor_counts) {
  //printf("count_neighbors\n");
	// determining the size of the TBSPixel array
	unsigned int total_exemplar_pixels = exemplar->width * exemplar->height;
	unsigned int total_new_img_pixels = new_img->width * new_img->height;
	unsigned int total_tbs_pixels = total_new_img_pixels - total_exemplar_pixels;

	// variables for Pixel array and TBSPixels array
	Pixel * pixels = new_img->pixels;
	int capacity = 1;
 	TBSPixel * tbs_pixels = (TBSPixel*) malloc(sizeof(TBSPixel) * capacity);//freed in synthesize from exemplar

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
                        if(neighbor_count > 0){
			  tbs_pixels[TBSPixel_index].idx.x = pixel_col;
                          tbs_pixels[TBSPixel_index].idx.y = pixel_row;
                          tbs_pixels[TBSPixel_index].neighborCount = neighbor_count;
			  TBSPixel_index++;
			}

			if (TBSPixel_index == capacity && i != total_new_img_pixels - 1) {
			  capacity++;
			  //printf("before realloc\n");
			  tbs_pixels = (TBSPixel *) realloc(tbs_pixels, sizeof(TBSPixel) * capacity);
			}

			neighbor_count = 0;


		}
	}
	*num_neighbor_counts = TBSPixel_index; 
        //printf("before return\n");
	return tbs_pixels;
}

int determine_position(unsigned int i, unsigned int width, unsigned int height) {
  //printf("determine_position\n");
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
  //printf("count_for_top\n");
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
  //printf("count_for_bottom\n");
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
  //printf("count_for_bottom_left\n");
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
  //printf("count_for_bottom\n");
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
  //printf("count_for_top_right\n");
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
  //printf("count_for_right\n");
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
  //printf("count_for_left\n");
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
  //printf("count_for_other\n");
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


Image * place_image(unsigned int width, unsigned int height, Image * exemplar_image) {
  //printf("place_image\n");
	// checking if the width and height of the new image are greater
	// than the width and the height of the exemplar image
	if ((width < exemplar_image->width) || (height < exemplar_image->height)) {
    	return NULL;
  	}
  
	// setting the width and the height of the new image
	Image * new_img = (Image *) malloc(sizeof(Image)); //freed in main
	new_img->width = width;
	new_img->height = height;
	new_img->pixels = (Pixel *) malloc(sizeof(Pixel) * width * height); //freed in main

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
			(new_pixels[i]).a = (unsigned char)255; 
	  	}
		else {
		  (new_pixels[i]).r = (unsigned char)0;
        	(new_pixels[i]).g = (unsigned char)0;
        	(new_pixels[i]).b = (unsigned char)0;
		(new_pixels[i]).a = (unsigned char)0;
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
  //printf("CompareTBSPixels\n");
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
  //printf("SortTBSPixels\n");
  //printf("num neighbor: %u\n", sz);
  unsigned int * permutation = (unsigned int*)malloc( sizeof(unsigned int)*sz);
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


Image *SynthesizeFromExemplar(Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius) // bool verbose 
{
	printf("SynthesizeFromExemplar\n");
	Image * new_image = place_image(outWidth, outHeight, exemplar);
	
	if (new_image == NULL) {
		printf("Error: Unable to generate a pointer to the new image.\n");
		return NULL;
	}
	
	// TBSPixel * neighbor_counts = count_neighbors(new_image, exemplar);
	//TBSPixel * tbs_pixels = (TBSPixel *) malloc(sizeof(TBSPixel)); currently, an array that will hold one TBSPixel
	int num_neighbor_counts = 0;
	TBSPixel * tbs_pixels  = count_neighbors(new_image, exemplar, &num_neighbor_counts);
	//SortTBSPixels(tbs_pixels, num_neighbor_counts);
	//new_image = set_TBS_Pixels(num_neighbor_counts, tbs_pixels, windowRadius, new_image, exemplar);
	//unsigned int num_neighbor_counts = 0;
	//if (tbs_pixels == NULL) {
	//printf("Error: Unable to generate a pointer to a TBSPixel array.\n");
	//	return NULL;
	//}

	
	// SortTBSPixels successful
	int exemplar_pixels = exemplar->width * exemplar->height;
	int new_pixels = new_image->width * new_image->height;
	unsigned int TBSPixel_final_arr_size = new_pixels - exemplar_pixels;
        
	//int current_tbs_size = (sizeof(neighbor_counts))/(sizeof(TBSPixel));

	while (num_neighbor_counts > 0) { //while there are still unset pixels
	  SortTBSPixels(tbs_pixels, num_neighbor_counts);
	  new_image = set_TBS_Pixels(num_neighbor_counts, tbs_pixels, windowRadius, new_image, exemplar);
	  free(tbs_pixels);
	  tbs_pixels  = count_neighbors(new_image, exemplar, &num_neighbor_counts);
	}
        free(tbs_pixels);
	FreeImage(&exemplar);
	
	//SortTBSPixels(neighbor_counts, TBSPixel_arr_size); 

	//new_image = set_TBS_Pixels(TBSPixel_arr_size, neighbor_counts, windowRadius, new_image, exemplar); 


	return new_image;
}

