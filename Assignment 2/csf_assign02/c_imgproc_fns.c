// C implementations of image processing functions

#include <stdlib.h>
#include <assert.h>
#include "imgproc.h"

// TODO: define your helper functions here
int all_tiles_nonempty( int width, int height, int n );
int determine_tile_w( int width, int n, int tile_col );
int determine_tile_x_offset( int width, int n, int tile_col );
int determine_tile_h( int height, int n, int tile_row );
int determine_tile_y_offset( int height, int n, int tile_row );
void copy_tile( struct Image *out_img, struct Image *img, int tile_row, int tile_col, int n );

uint32_t get_r(uint32_t pixel) { //shift 
    return (pixel >> 24) & 0xFF;
}

uint32_t get_g(uint32_t pixel) {
    return (pixel >> 16) & 0xFF;
}

uint32_t get_b(uint32_t pixel) {
    return (pixel >> 8) & 0xFF;
}

uint32_t get_a(uint32_t pixel) {
    return pixel & 0xFF;
}

uint32_t make_pixel(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    return (r << 24) | (g << 16) | (b << 8) | a;
}

uint32_t get_pixel(struct Image *img, int32_t x, int32_t y) {
    return img->data[y * img->width + x];
}

void set_pixel(struct Image *img, int32_t x, int32_t y, uint32_t pixel) {
    img->data[y * img->width + x] = pixel;
}

uint32_t to_grayscale( uint32_t pixel ) {
  uint32_t gray_value = ((79 * get_r(pixel)) + (128 * get_g(pixel)) + (49 * get_b(pixel))) / 256;
  return make_pixel(gray_value, gray_value, gray_value, get_a(pixel));
}


uint32_t blend_components( uint32_t fg, uint32_t bg, uint32_t alpha ) {
  return (alpha * fg + (255 - alpha) * bg) / 255; //computation for new color component
}

uint32_t blend_colors( uint32_t fg, uint32_t bg ) {
  uint32_t fg_r = get_r(fg);
  uint32_t fg_g = get_g(fg);
  uint32_t fg_b = get_b(fg);
  uint32_t fg_a = get_a(fg);  // alpha value for blending

  uint32_t bg_r = get_r(bg);
  uint32_t bg_g = get_g(bg);
  uint32_t bg_b = get_b(bg);

  // Blend each color component using the foreground alpha
  uint32_t blended_r = blend_components(fg_r, bg_r, fg_a);
  uint32_t blended_g = blend_components(fg_g, bg_g, fg_a);
  uint32_t blended_b = blend_components(fg_b, bg_b, fg_a);

  // return new pixel w/ alpha 255 (fully opaque)
  return make_pixel(blended_r, blended_g, blended_b, 255);
}

// Mirror input image horizontally.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_h( struct Image *input_img, struct Image *output_img ) {
  
  //iterate over each row, swap the pixels to produce mirror image
  for(int32_t y = 0; y < input_img->height; y++) {
    for(int32_t x = 0; x < input_img->width; x++) {

      int32_t mirrored_width = input_img->width - 1 - x; // width index of tile to swap with

      // Swap current pixel to mirrored position in output image
      uint32_t pixel = get_pixel(input_img, x, y);
      set_pixel(output_img, mirrored_width, y, pixel);
    }
  }

}

// Mirror input image vertically.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_mirror_v( struct Image *input_img, struct Image *output_img ) {
  
  //iterate over each row, swap the pixels to produce mirror image
  for(int32_t y = 0; y < input_img->height; y++) {
    for(int32_t x = 0; x < input_img->width; x++) {

      int32_t mirrored_height = input_img->height - 1 - y; //height of tile to swap with

      // Swap current pixel to mirrored pixel in output image
      uint32_t pixel = get_pixel(input_img, x, y);
      set_pixel(output_img, x, mirrored_height, pixel);
    }
  }

}

// Transform image by generating a grid of n x n smaller tiles created by
// sampling every n'th pixel from the original image.
//
// Parameters:
//   input_img  - pointer to original struct Image
//   n          - tiling factor (how many rows and columns of tiles to generate)
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
// Returns:
//   1 if successful, or 0 if either
//     - n is less than 1, or
//     - the output can't be generated because at least one tile would
//       be empty (i.e., have 0 width or height)
int imgproc_tile( struct Image *input_img, int n, struct Image *output_img ) {
  // TODO: implement
  return 0;
}

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  
  for(int x = 0; x < input_img->width; x++) {
    for(int y = 0; y < input_img->height; y++) {

      uint32_t pixel = get_pixel(input_img, x, y);

      //convert current pixel to grayscale version
      uint32_t grayed_pixel = to_grayscale(pixel);

      //set current pixel to grayed version in output
      set_pixel(output_img, x, y, grayed_pixel);

    }
  }

}

// Overlay a foreground image on a background image, using each foreground
// pixel's alpha value to determine its degree of opacity in order to blend
// it with the corresponding background pixel.
//
// Parameters:
//   base_img - pointer to base (background) image
//   overlay_img - pointer to overlaid (foreground) image
//   output_img - pointer to output Image
//
// Returns:
//   1 if successful, or 0 if the transformation fails because the base
//   and overlay image do not have the same dimensions
int imgproc_composite( struct Image *base_img, struct Image *overlay_img, struct Image *output_img ) {
  // TODO: implement
  return 0;
}
