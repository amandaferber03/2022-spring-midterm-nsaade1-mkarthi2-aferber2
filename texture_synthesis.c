#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "texture_synthesis.h"

Image * placeImage(int width, int height, const Image * image) {
  if ((width < image->width) || (height < image->height)) {
    return NULL;
  }
  
  Image * img = (Image *) malloc(sizeof(Image));
  
  img->width = width;
  
  img->height = height;

  
  img->pixels = (Pixel *) malloc(sizeof(Pixel) * width * height);

  Pixel * pixels = img->pixels;
  Pixel * exemplar_pixels = image->pixels;
  
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int position = (width * i) + j; 
      if (i <= 50 && j <= 66) {
		(pixels[position]).r = (exemplar_pixels[position]).r;
		(pixels[position]).g = (exemplar_pixels[position]).g;
		(pixels[position]).b = (exemplar_pixels[position]).b;
		(pixels[position]).a = 255;
      }
      else {
		(pixels[position]).r = 0;
        (pixels[position]).g = 0;
        (pixels[position]).b = 0;
		(pixels[position]).a = 0;
      }
    }
  } 

//printf("Hey\n");
  
  /*for (int i = 0; i < (img->width * img->height); i++) {
	  Pixel * pixels = img->pixels;
      Pixel * exemplar_pixels = image->pixels;
	  //printf("Hey1\n");
	  if ((i / width) % (height-1) < img->height) { //Evaluating we are in an applicable row
		  if ((i % (width - 1) <= img->width) ) { //Evaluating if we are in an applicable column
			(pixels[i]).r = (exemplar_pixels[i/]).r;
			(pixels[i]).g = (exemplar_pixels[i]).g;
			(pixels[i]).b = (exemplar_pixels[i]).b;
			(pixels[i]).a = 255;
		  }
		  //printf("Hey2\n");
		  //printf("%d %d %d", width, height, width * height);
		  //printf("%d\n", i);
	  }
	  
	  else {
		(pixels[i]).r = 0;
        (pixels[i]).g = 0;
        (pixels[i]).b = 0;
		(pixels[i]).a = 0;
		//printf("Hey3\n");
	  }
  } */
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


Image *SynthesizeFromExemplar( const Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius , bool verbose )
{
	Image *synthesized = NULL;

	// TODO: IMPLEMENT THIS FUNCTION
	return synthesized;
}
