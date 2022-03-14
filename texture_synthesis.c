#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "texture_synthesis.h"

TBSPixel * count_neighbors(Image * img, const Image * exemplar) {
	int width = img->width;
	int height = img->height;
	int exemplarProd = (exemplar->width)*(exemplar->height);
	int imgProd = (img->width)*(img->height);
	int pixelAmount = imgProd - exemplarProd;
 
	// variables to represent alpha values of neighboring pixels
	
	Pixel * pixel = img->pixels;
	TBSPixel * tbs_pixels = (TBSPixel *) malloc(sizeof(TBSPixel) * pixelAmount);
	int tbs_counter = 0;
	for (int i = 0; i < (width * height); i++) {
		int tbs_neighbor_tracker = 0;
		int unset_pixel_x = i % width;
		int unset_pixel_y = i / width;

		// finding neighbor counts of unset pixels
		if (pixel[i].a == 0) {
			if ((i+width) >= (width * height)) { // BOTTOM-MOST PIXELS
				if ((i+1) == (width * height)) { // BOTTOM RIGHT
					if (pixel[i-width].a == 255) { // up
						tbs_neighbor_tracker++;
					}
					if (pixel[i - width - 1].a == 255) { // top-left
						tbs_neighbor_tracker++;
					}
					if (pixel[i - 1].a == 255) { // left
						tbs_neighbor_tracker++;
					}
				}
				else if (i == ((width * height) - (width - 1))) { // BOTTOM LEFT
					if (pixel[i - width].a == 255) { // up
						tbs_neighbor_tracker++;
					}
					if (pixel[i - width + 1].a == 255) { // top-right
						tbs_neighbor_tracker++;
					}
					if (pixel[i + 1].a == 255) { // right
						tbs_neighbor_tracker++;
					}
					}
				}
				else { // BOTTOM
					if (pixel[i-width].a == 255) { // top
						tbs_neighbor_tracker++;
					}
					if (pixel[i - width + 1].a == 255) { // top-right
						tbs_neighbor_tracker++;
					}
					if (pixel[i - width - 1].a == 255) { // top-left
						tbs_neighbor_tracker++;
					}
					if (pixel[i - 1].a == 255) { // left
						tbs_neighbor_tracker++;
					}
					if (pixel[i + 1].a == 255) { // right
						tbs_neighbor_tracker++;
					}
				}
			}
			else if ((i % width) == 0) { // LEFT-MOST PIXELS
				if (pixel[i-width].a == 255) { // up
					tbs_neighbor_tracker++;
				}
				if (pixel[i - width + 1].a == 255) { // top-right
					tbs_neighbor_tracker++;
				}
				if (pixel[i + 1].a == 255) { // right
					tbs_neighbor_tracker++;
				}
				if (pixel[i + width].a == 255) { // bottom
					tbs_neighbor_tracker++;
				}
				if (pixel[i + width + 1].a == 255) { // bottom-right
					tbs_neighbor_tracker++;
				}

			}
			else if (((i + 1) % width) == 0) { // RIGHT-MOST PIXELS
				if (i + 1 == width) { // TOP RIGHT
					if (pixel[i - 1].a == 255) { //left
						tbs_neighbor_tracker++;
					}
					if (pixel[i + width].a == 255) { //bottom
						tbs_neighbor_tracker++;
					}
					if (pixel[i + width - 1].a == 255) { //bottom-left
						tbs_neighbor_tracker++;
					}
				}
				else { // RIGHT
					if (pixel[i-width].a == 255) { // up
						tbs_neighbor_tracker++;
					}
					if (pixel[i - width - 1].a == 255) { // top-left
						tbs_neighbor_tracker++;
					}
					if (pixel[i - 1].a == 255) { // left
						tbs_neighbor_tracker++;
					}
					if (pixel[i + width].a == 255) { // bottom
						tbs_neighbor_tracker++;
					}
					if (pixel[i + width - 1].a == 255) { //bottom-left
						tbs_neighbor_tracker++;
					}
				}
			}
			else if ((i < width) == 0) { // TOP
				if (pixel[i - 1].a == 255) { //left
					tbs_neighbor_tracker++;
				}
				if (pixel[i + 1].a == 255) { //right
					tbs_neighbor_tracker++;
				}
				if (pixel[i + width].a == 255) { //bottom
					tbs_neighbor_tracker++;
				}
				if (pixel[i + width - 1].a == 255) { //bottom-left
					tbs_neighbor_tracker++;
				}
				if (pixel[i + width + 1].a == 255) { //bottom-right
					tbs_neighbor_tracker++;
				}
			}
			else { // none of the borders
				if (pixel[i-width].a == 255) { //up
					tbs_neighbor_tracker++;
				}
				if (pixel[i - width + 1].a == 255) { //top-right
					tbs_neighbor_tracker++;
				}
				if (pixel[i - width - 1].a == 255) { //top-left
					tbs_neighbor_tracker++;
				}
				if (pixel[i - 1].a == 255) { //left
					tbs_neighbor_tracker++;
				}
				if (pixel[i + 1].a == 255) { //right
					tbs_neighbor_tracker++;
				}
				if (pixel[i + width].a == 255) { //bottom
					tbs_neighbor_tracker++;
				}
				if (pixel[i + width - 1].a == 255) { //bottom-left
					tbs_neighbor_tracker++;
				}
				if (pixel[i + width + 1].a == 255) { //bottom-right
					tbs_neighbor_tracker++;
				}
			}

		// setting idx and neighborCount fields
		tbs_pixels[tbs_counter].idx.x = unset_pixel_x;
		tbs_pixels[tbs_counter].idx.y = unset_pixel_y;

		// next index in TBS pixel array
		tbs_counter++;
	}
	return tbs_pixels;
}


Image * place_image(int width, int height, const Image * image) {
  if ((width < image->width) || (height < image->height)) {
    return NULL;
  }
  
  Image * img = (Image *) malloc(sizeof(Image));
  img->width = width;
  img->height = height;
  img->pixels = (Pixel *) malloc(sizeof(Pixel) * width * height);

  Pixel * pixels = img->pixels;
  Pixel * exemplar_pixels = image->pixels;
  int count = 0;
  int counter = 0; 

  for(int i = 0; i < width * height; i++) {
	  if(i >= (count*width) && i <= (count*width + image->width -1) && !(count >= image->height)) {
		  int exemp_pos = i - count*(((count+1)*(width) -1) - ((count)*(width) + image->width -1));
		  (pixels[i]).r = (exemplar_pixels[exemp_pos]).r;
		  (pixels[i]).g = (exemplar_pixels[exemp_pos]).g;
		  (pixels[i]).b = (exemplar_pixels[exemp_pos]).b;
		  (pixels[i]).a = 255;
		  
	  }
	  else {
		(pixels[i]).r = 0;
        (pixels[i]).g = 0;
        (pixels[i]).b = 0;
		(pixels[i]).a = 0;
	  }
	  counter++;
	  if (counter % width == 0) {
		  count++;
	  }

  }
  return img;
  
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
	printf("Hello");
	Image * new_image = place_image(outWidth, outHeight, exemplar);
	printf("Hello");
	TBSPixel * neighbor_counts = count_neighbors(new_image, exemplar);
	
	// SortTBSPixels successful
	int exemplar_pixels = exemplar->width * exemplar->height;
	int new_pixels = new_image->width * new_image->height;
	int TBSPixel_arr_size = exemplar_pixels * new_pixels;

	printf("Hello");
	if (SortTBSPixels(neighbor_counts, TBSPixel_arr_size) == 0) {
		printf("Success!\n");
	}
	else {
		printf("Failure :(\n");
	}

	// TODO: IMPLEMENT THIS FUNCTION
	return new_image;
}
