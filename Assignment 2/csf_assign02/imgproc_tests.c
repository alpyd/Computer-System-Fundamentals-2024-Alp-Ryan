#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tctest.h"
#include "imgproc.h"

// An expected color identified by a (non-zero) character code.
// Used in the "Picture" data type.
typedef struct {
  char c;
  uint32_t color;
} ExpectedColor;

// Type representing a "picture" of an expected image.
// Useful for creating a very simple Image to be accessed
// by test functions.
typedef struct {
  ExpectedColor colors[20];
  int width, height;
  const char *data;
} Picture;

// Some "basic" colors to use in test Pictures
#define TEST_COLORS \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0xFF0000FF }, \
      { 'g', 0x00FF00FF }, \
      { 'b', 0x0000FFFF }, \
      { 'c', 0x00FFFFFF }, \
      { 'm', 0xFF00FFFF }, \
    }

// Expected "basic" colors after grayscale transformation
#define TEST_COLORS_GRAYSCALE \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0x4E4E4EFF }, \
      { 'g', 0x7F7F7FFF }, \
      { 'b', 0x303030FF }, \
      { 'c', 0xB0B0B0FF }, \
      { 'm', 0x7F7F7FFF }, \
    }

// Colors for test overlay image (for testing the composite
// transformation). Has some fully-opaque colors,
// some partially-transparent colors, and a complete
// transparent color.
#define OVERLAY_COLORS \
  { \
    { 'r', 0xFF0000FF }, \
    { 'R', 0xFF000080 }, \
    { 'g', 0x00FF00FF }, \
    { 'G', 0x00FF0080 }, \
    { 'b', 0x0000FFFF }, \
    { 'B', 0x0000FF80 }, \
    { ' ', 0x00000000 }, \
  }

// Data type for the test fixture object.
// This contains data (including Image objects) that
// can be accessed by test functions. This is useful
// because multiple test functions can access the same
// data (so you don't need to create/initialize that
// data multiple times in different test functions.)
typedef struct {
  // smiley-face picture
  Picture smiley_pic;

  // original smiley-face Image object
  struct Image *smiley;

  // empty Image object to use for output of
  // transformation on smiley-face image
  struct Image *smiley_out;

  // Picture for overlay image (for basic imgproc_composite test)
  Picture overlay_pic;

  // overlay image object
  struct Image *overlay;
} TestObjs;

// Functions to create and clean up a test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Helper functions used by the test code
struct Image *picture_to_img( const Picture *pic );
uint32_t lookup_color(char c, const ExpectedColor *colors);
bool images_equal( struct Image *a, struct Image *b );
void destroy_img( struct Image *img );

// Test functions
void test_mirror_h_basic( TestObjs *objs );
void test_mirror_v_basic( TestObjs *objs );
void test_tile_basic( TestObjs *objs );
void test_grayscale_basic( TestObjs *objs );
void test_composite_basic( TestObjs *objs );
// TODO: add prototypes for additional test functions

void test_all_tiles_nonempty( TestObjs *objs);
void test_determine_tile_x_offset( TestObjs *objs);
void test_determine_tile_y_offset( TestObjs *objs);
void test_determine_tile_w( TestObjs *objs);
void test_determine_tile_h( TestObjs *objs);
void test_make_pixel( TestObjs *objs);
void test_get_pixel( TestObjs *objs);
void test_set_pixel( TestObjs *objs);
void test_copy_tile( TestObjs *objs);
void test_get_r( TestObjs *objs);
void test_get_g( TestObjs *objs);
void test_get_b( TestObjs *objs);
void test_get_a( TestObjs *objs);
void test_to_grayscale( TestObjs *objs);
void test_blend_components( TestObjs *objs);
void test_blend_colors( TestObjs *objs);

int main( int argc, char **argv ) {
  // allow the specific test to execute to be specified as the
  // first command line argument
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  // Run tests.
  // Make sure you add additional TEST() macro invocations
  // for any additional test functions you add.
  
  TEST( test_mirror_h_basic );
  TEST( test_mirror_v_basic );
  TEST( test_tile_basic );
  TEST( test_grayscale_basic );
  TEST( test_composite_basic );
  TEST( test_all_tiles_nonempty );
  TEST( test_determine_tile_x_offset);
  TEST( test_determine_tile_y_offset);
  TEST( test_determine_tile_w);
  TEST( test_determine_tile_h);
  TEST( test_make_pixel);
  TEST( test_get_pixel);
  TEST( test_set_pixel);
  TEST( test_copy_tile);
  TEST( test_get_r);
  TEST( test_get_g);
  TEST( test_get_b);
  TEST( test_get_a);
  TEST( test_to_grayscale);

  TEST_FINI();
}

////////////////////////////////////////////////////////////////////////
// Test fixture setup/cleanup functions
////////////////////////////////////////////////////////////////////////

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs) );

  Picture smiley_pic = {
    TEST_COLORS,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };
  objs->smiley_pic = smiley_pic;
  objs->smiley = picture_to_img( &smiley_pic );

  objs->smiley_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->smiley_out, objs->smiley->width, objs->smiley->height );

  Picture overlay_pic = {
    OVERLAY_COLORS,
    16, 10,
   "                "
   "                "
   "                "
   "                "
   "                "
   "  rRgGbB        "
   "                "
   "                "
   "                "
   "                "
  };
  objs->overlay_pic = overlay_pic;
  objs->overlay = picture_to_img( &overlay_pic );

  return objs;
}

void cleanup( TestObjs *objs ) {
  destroy_img( objs->smiley );
  destroy_img( objs->smiley_out );
  destroy_img( objs->overlay );

  free( objs );
}

////////////////////////////////////////////////////////////////////////
// Test code helper functions
////////////////////////////////////////////////////////////////////////

struct Image *picture_to_img( const Picture *pic ) {
  struct Image *img;

  img = (struct Image *) malloc( sizeof(struct Image) );
  img_init( img, pic->width, pic->height );

  for ( int i = 0; i < pic->height; ++i ) {
    for ( int j = 0; j < pic->width; ++j ) {
      int index = i * img->width + j;
      uint32_t color = lookup_color( pic->data[index], pic->colors );
      img->data[index] = color;
    }
  }

  return img;
}

uint32_t lookup_color(char c, const ExpectedColor *colors) {
  for (int i = 0; ; i++) {
    assert(colors[i].c != 0);
    if (colors[i].c == c) {
      return colors[i].color;
    }
  }
}

// Returns true IFF both Image objects are identical
bool images_equal( struct Image *a, struct Image *b ) {
  if ( a->width != b->width || a->height != b->height )
    return false;

  int num_pixels = a->width * a->height;
  for ( int i = 0; i < num_pixels; ++i ) {
    if ( a->data[i] != b->data[i] )
      return false;
  }

  return true;
}

void destroy_img( struct Image *img ) {
  if ( img != NULL )
    img_cleanup( img );
  free( img );
}

////////////////////////////////////////////////////////////////////////
// Test functions
////////////////////////////////////////////////////////////////////////

void test_mirror_h_basic( TestObjs *objs ) {
  Picture smiley_mirror_h_pic = {
    TEST_COLORS,
    16, 10,
    "    cbggrrrm    "
    "   b        c   "
    "  c   b  r   r  "
    " b            b "
    " r            b "
    " r   c    b   g "
    "  b   brgg   c  "
    "   c        m   "
    "    cmbrrggg    "
    "                "
  };
  struct Image *smiley_mirror_h_expected = picture_to_img( &smiley_mirror_h_pic );

  imgproc_mirror_h( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_mirror_h_expected, objs->smiley_out ) );

  destroy_img( smiley_mirror_h_expected );
}

void test_mirror_v_basic( TestObjs *objs ) {
  Picture smiley_mirror_v_pic = {
    TEST_COLORS,
    16, 10,
    "                "
    "    gggrrbmc    "
    "   m        c   "
    "  c   ggrb   b  "
    " g   b    c   r "
    " b            r "
    " b            b "
    "  r   r  b   c  "
    "   c        b   "
    "    mrrrggbc    "
  };
  struct Image *smiley_mirror_v_expected = picture_to_img( &smiley_mirror_v_pic );

  imgproc_mirror_v( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_mirror_v_expected, objs->smiley_out ) );

  destroy_img( smiley_mirror_v_expected );
}

void test_tile_basic( TestObjs *objs ) {
  Picture smiley_tile_3_pic = {
    TEST_COLORS,
    16, 10,
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
    "                "
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
    "  rg    rg   rg "
    "                "
    "  gb    gb   gb "
  };
  struct Image *smiley_tile_3_expected = picture_to_img( &smiley_tile_3_pic );

  int success = imgproc_tile( objs->smiley, 3, objs->smiley_out );
  ASSERT( success );
  ASSERT( images_equal( smiley_tile_3_expected, objs->smiley_out ) );

  destroy_img( smiley_tile_3_expected );
}

void test_grayscale_basic( TestObjs *objs ) {
  Picture smiley_grayscale_pic = {
    TEST_COLORS_GRAYSCALE,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };

  struct Image *smiley_grayscale_expected = picture_to_img( &smiley_grayscale_pic );

  imgproc_grayscale( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_grayscale_expected, objs->smiley_out ) );

  destroy_img( smiley_grayscale_expected );
}

void test_composite_basic( TestObjs *objs ) {
  imgproc_composite( objs->smiley, objs->overlay, objs->smiley_out );

  // for all of the fully-transparent pixels in the overlay image,
  // the result image should have a pixel identical to the corresponding
  // pixel in the base image
  for ( int i = 0; i < 160; ++i ) {
    if ( objs->overlay->data[i] == 0x00000000 )
      ASSERT( objs->smiley->data[i] == objs->smiley_out->data[i] );
  }

  // check the computed colors for the partially transparent or
  // fully opaque colors in the overlay image
  ASSERT( 0xFF0000FF == objs->smiley_out->data[82] );
  ASSERT( 0x800000FF == objs->smiley_out->data[83] );
  ASSERT( 0x00FF00FF == objs->smiley_out->data[84] );
  ASSERT( 0x00807FFF == objs->smiley_out->data[85] );
  ASSERT( 0x0000FFFF == objs->smiley_out->data[86] );
  ASSERT( 0x000080FF == objs->smiley_out->data[87] );
}

//TESTS FOR HELPER FUNCTIONS

void test_all_tiles_nonempty(TestObjs *objs) {

//same width and height, valid tiling factor
ASSERT(all_tiles_nonempty(16, 16, 2) == 1);

//tiling factor fails for both height/width
ASSERT(all_tiles_nonempty(16, 16, 17) == 0);

//tiling factor fails for height only
ASSERT(all_tiles_nonempty(20, 10, 11) == 0);

//tiling factor fails for width only
ASSERT(all_tiles_nonempty(12, 15, 15) == 0);

}

void test_determine_tile_x_offset(TestObjs *objs) {

//width is perfectly divisible by n (no remainder)
ASSERT(determine_tile_x_offset(16, 4, 0) == 0); // 16 % 4 = 0, no remainder
ASSERT(determine_tile_x_offset(16, 4, 3) == 0); // 16 % 4 = 0, no remainder

//tile_col is less than the remainder
ASSERT(determine_tile_x_offset(17, 4, 0) == 1); // 17 % 4 = 1, tile_col < remainder
ASSERT(determine_tile_x_offset(18, 5, 1) == 1); // 18 % 5 = 3, tile_col < remainder

//tile_col is equal to or greater than the remainder
ASSERT(determine_tile_x_offset(17, 4, 1) == 0); // 17 % 4 = 1, tile_col == remainder
ASSERT(determine_tile_x_offset(18, 5, 3) == 0); // 18 % 5 = 3, tile_col == remainder

// Edge case: width smaller than n
ASSERT(determine_tile_x_offset(3, 4, 0) == 1); // 3 % 4 = 3, tile_col < remainder

// Special case: width is 0
ASSERT(determine_tile_x_offset(0, 4, 0) == 0); // 0 % 4 = 0, no remainder

int res_1 = determine_tile_x_offset(4, 2, 1);
ASSERT(res_1 == 0);
int res_2 = determine_tile_x_offset(5, 2, 0);
ASSERT(res_2 == 1);
int res_3 = determine_tile_x_offset(5, 2, 1);
ASSERT(res_3 == 0);
int res_4 = determine_tile_x_offset(0, 6, 0);
ASSERT(res_4 == 0);
int res_5 = determine_tile_x_offset(48, 7, 6);
ASSERT(res_5 == 0);
for(int i = 0; i < 6; i++){
  int res_6 = determine_tile_x_offset(48, 7, i);
  ASSERT(res_6 == 1);
  }

}

void  test_determine_tile_y_offset(TestObjs *objs) {

//height is perfectly divisible by n (no remainder)
ASSERT(determine_tile_y_offset(16, 4, 0) == 0); // 16 % 4 = 0, no remainder
ASSERT(determine_tile_y_offset(16, 4, 3) == 0); // 16 % 4 = 0, no remainder

//tile_row is less than the remainder
ASSERT(determine_tile_y_offset(17, 4, 0) == 1); // 17 % 4 = 1, tile_row < remainder
ASSERT(determine_tile_y_offset(18, 5, 1) == 1); // 18 % 5 = 3, tile_row < remainder

//tile_row is equal to or greater than the remainder
ASSERT(determine_tile_y_offset(17, 4, 1) == 0); // 17 % 4 = 1, tile_row == remainder
ASSERT(determine_tile_y_offset(18, 5, 3) == 0); // 18 % 5 = 3, tile_row == remainder

//Edge case: height smaller than n
ASSERT(determine_tile_y_offset(3, 4, 0) == 1); // 3 % 4 = 3, tile_row < remainder

//Special case: height is 0
ASSERT(determine_tile_y_offset(0, 4, 0) == 0); // 0 % 4 = 0, no remainder

}

void test_determine_tile_w(TestObjs *objs){
  //Test 0 width case; n = 0 is not tested because tile already returns 0 for this case
  ASSERT(determine_tile_w(0, 4, 2) == 0);
  ASSERT(determine_tile_w(0, 1, 0) == 0);

  //Test width = 1 case and not enough values in width
  ASSERT(determine_tile_w(1, 4, 0) == 1);
  ASSERT(determine_tile_w(1, 4, 2) == 0);

  //Test even width distributed across n
  for(int i = 0; i < 7; i++){
    ASSERT(determine_tile_w(42, 7, i) == 6);
  }
  for(int i = 0; i < 100; i++){
    ASSERT(determine_tile_w(10000, 100, i) == 100);
  }
  for(int i = 0; i < 10000; i++){
    ASSERT(determine_tile_w(10000, 10000, i) == 1);
  }

  //Test uneven width distribution across n
  for(int i = 0; i < 6; i++){
    ASSERT(determine_tile_w(48, 7, i) == 7);
  }
  ASSERT(determine_tile_w(48, 7, 6) == 6);

  for(int i = 0; i < 8; i++){
    ASSERT(determine_tile_w(177, 13, i) == 14);
  }
  for(int i = 8; i < 13; i++){
    ASSERT(determine_tile_w(177, 13, i) == 13);
  }
}

void test_determine_tile_h(TestObjs *objs){
    //Test 0 height case; n = 0 is not tested because tile already returns 0 for this case
  ASSERT(determine_tile_h(0, 4, 2) == 0);
  ASSERT(determine_tile_h(0, 1, 0) == 0);

  //Test height = 1 case and not enough values in height
  ASSERT(determine_tile_h(1, 4, 0) == 1);
  ASSERT(determine_tile_h(1, 4, 2) == 0);

  //Test even height distributed across n
  for(int i = 0; i < 7; i++){
    ASSERT(determine_tile_h(42, 7, i) == 6);
  }
  for(int i = 0; i < 100; i++){
    ASSERT(determine_tile_h(10000, 100, i) == 100);
  }
  for(int i = 0; i < 10000; i++){
    ASSERT(determine_tile_h(10000, 10000, i) == 1);
  }

  //Test uneven height distribution across n
  for(int i = 0; i < 6; i++){
    ASSERT(determine_tile_h(48, 7, i) == 7);
  }
  ASSERT(determine_tile_h(48, 7, 6) == 6);

  for(int i = 0; i < 8; i++){
    ASSERT(determine_tile_h(177, 13, i) == 14);
  }
  for(int i = 8; i < 13; i++){
    ASSERT(determine_tile_h(177, 13, i) == 13);
  }
}

void test_make_pixel( TestObjs *objs){
  //Test 0 Conditions and Max Conditions
  ASSERT(make_pixel(0, 0, 0, 0) == 0);
  ASSERT(make_pixel(0, 0, 0, 255) == 255);
  ASSERT(make_pixel(0, 0, 255, 255) == 65535);
  ASSERT(make_pixel(0, 255, 255, 255) == 16777215);
  ASSERT(make_pixel(255, 255, 255, 255) == 4294967295);
  ASSERT(make_pixel(255, 255, 255, 0) == 4294967295-255);
  ASSERT(make_pixel(255, 255, 0, 0) == 4294967295-65535);
  ASSERT(make_pixel(255, 0, 0, 0) == 4294967295-16777215);

  //Test arbitrary values
  ASSERT(make_pixel(32, 17, 143, 62) == 538021694);
  ASSERT(make_pixel(55, 12, 196, 202) == 923583690);
  ASSERT(make_pixel(25, 166, 43, 100) == 430320484);
  ASSERT(make_pixel(44, 218, 12, 255) == 752487679);
  ASSERT(make_pixel(23, 191, 173, 230) == 398437862);

  //Test Overflow
  ASSERT(make_pixel(256, 0, 0, 0 != 4294967296));
}

void test_get_pixel( TestObjs *objs){
  //Test on an image of 1x1 width
  Picture single_pixel = {
    TEST_COLORS,
    1,
    1,
    " "
  };
  struct Image *test_single_pixel = picture_to_img( &single_pixel);
  ASSERT(get_pixel(test_single_pixel, 0, 0) == 0x000000FF);

  //Test on a Larger Image
  Picture test_image_mosaic = {
    TEST_COLORS,
    3, // width
    3, // height
    "mrg"
    "c g"
    "rgb"
  };

  struct Image *test_get_mosaic = picture_to_img( &test_image_mosaic);
  ASSERT(get_pixel(test_get_mosaic, 0, 0) == 0xFF00FFFF);
  ASSERT(get_pixel(test_get_mosaic, 1, 0) == 0xFF0000FF);
  ASSERT(get_pixel(test_get_mosaic, 2, 0) == 0x00FF00FF);
  ASSERT(get_pixel(test_get_mosaic, 0, 1) == 0x00FFFFFF);
  ASSERT(get_pixel(test_get_mosaic, 1, 1) == 0x000000FF);
  ASSERT(get_pixel(test_get_mosaic, 2, 1) == 0x00FF00FF);
  ASSERT(get_pixel(test_get_mosaic, 0, 2) == 0xFF0000FF);
  ASSERT(get_pixel(test_get_mosaic, 1, 2) == 0x00FF00FF);
  ASSERT(get_pixel(test_get_mosaic, 2, 2) == 0x0000FFFF);
}
  
void test_set_pixel( TestObjs *objs){
    Picture single_pixel = {
    TEST_COLORS,
    1,
    1,
    " "
  };
  struct Image *test_single_pixel = picture_to_img( &single_pixel);
  ASSERT(get_pixel(test_single_pixel, 0, 0) == 0x000000FF);
  set_pixel(test_single_pixel, 0, 0, 0xFEEDBEEF);
  ASSERT(get_pixel(test_single_pixel, 0, 0) == 0xFEEDBEEF);
  set_pixel(test_single_pixel, 0, 0, 0xACABBACE);
  ASSERT(get_pixel(test_single_pixel, 0, 0) == 0xACABBACE);
  set_pixel(test_single_pixel, 0, 0, 0xDEAFACED);
  ASSERT(get_pixel(test_single_pixel, 0, 0) == 0xDEAFACED);
  set_pixel(test_single_pixel, 0, 0, 0xABADBEAD);
  ASSERT(get_pixel(test_single_pixel, 0, 0) == 0xABADBEAD);

  //Test on a Larger Image
  Picture test_image_mosaic = {
    TEST_COLORS,
    3, // width
    3, // height
    "   "
    "   "
    "   "
  };

  struct Image *test_set_mosaic = picture_to_img( &test_image_mosaic);
  set_pixel(test_set_mosaic, 0, 0, 0xFF00FFFF);
  set_pixel(test_set_mosaic, 0, 1, 0xFF0000FF);
  set_pixel(test_set_mosaic, 0, 2, 0x00FF00FF);
  set_pixel(test_set_mosaic, 1, 0, 0x00FFFFFF);
  set_pixel(test_set_mosaic, 1, 1, 0x0000FFFF);
  set_pixel(test_set_mosaic, 1, 2, 0x00FF00FF);
  set_pixel(test_set_mosaic, 2, 0, 0xFF0000FF);
  set_pixel(test_set_mosaic, 2, 1, 0x00FF00FF);
  set_pixel(test_set_mosaic, 2, 2, 0x0000FFFF);

  ASSERT(get_pixel(test_set_mosaic, 0, 0) == 0xFF00FFFF);
  ASSERT(get_pixel(test_set_mosaic, 0, 1) == 0xFF0000FF);
  ASSERT(get_pixel(test_set_mosaic, 0, 2) == 0x00FF00FF);
  ASSERT(get_pixel(test_set_mosaic, 1, 0) == 0x00FFFFFF);
  ASSERT(get_pixel(test_set_mosaic, 1, 1) == 0x0000FFFF);
  ASSERT(get_pixel(test_set_mosaic, 1, 2) == 0x00FF00FF);
  ASSERT(get_pixel(test_set_mosaic, 2, 0) == 0xFF0000FF);
  ASSERT(get_pixel(test_set_mosaic, 2, 1) == 0x00FF00FF);
  ASSERT(get_pixel(test_set_mosaic, 2, 2) == 0x0000FFFF);
} 

void test_copy_tile( TestObjs *objs){
  Picture input_tile_picture = {
    TEST_COLORS,
    8, // width
    4, // height
    "crgmbcrm"
    "mbcrmgrb"
    "grcmggrb"
    "rbbmmcmr"
  };

  struct Image *copy_tile_input = picture_to_img( &input_tile_picture);

  //Test tiling an even grid of numbers
  Picture output_image = {
    TEST_COLORS,
    8, // width
    4, // height
    "        "
    "        "
    "        "
    "        "
  };

  struct Image* copy_even_output_n4 = picture_to_img(&output_image);
  //Edge Case: Filling in (0,0)
  copy_tile(copy_even_output_n4, copy_tile_input, 0, 0, 4);
  ASSERT(get_pixel(copy_even_output_n4, 0, 0) == 0x00FFFFFF);
  ASSERT(get_pixel(copy_even_output_n4, 1, 0) == 0x0000FFFF);
  //Ensure no other values are changed
  ASSERT(get_pixel(copy_even_output_n4, 2, 0) == 0x000000FF);
  ASSERT(get_pixel(copy_even_output_n4, 0, 1) == 0x000000FF);

  //Try filling in (0, 1) and (1,0)
  copy_tile(copy_even_output_n4, copy_tile_input, 1, 0, 4);
  copy_tile(copy_even_output_n4, copy_tile_input, 0, 1, 4);
  ASSERT(get_pixel(copy_even_output_n4, 2, 0) == 0x00FFFFFF);
  ASSERT(get_pixel(copy_even_output_n4, 3, 0) == 0x0000FFFF);
  ASSERT(get_pixel(copy_even_output_n4, 0, 1) == 0x00FFFFFF);
  ASSERT(get_pixel(copy_even_output_n4, 1, 1) == 0x0000FFFF);
  ASSERT(get_pixel(copy_even_output_n4, 0, 0) == 0x00FFFFFF);
  ASSERT(get_pixel(copy_even_output_n4, 1, 0) == 0x0000FFFF);

  //Ensure no other values are changed
  ASSERT(get_pixel(copy_even_output_n4, 6, 2) == 0x000000FF);
  ASSERT(get_pixel(copy_even_output_n4, 7, 3) == 0x000000FF);

  struct Image* copy_odd_output_n3 = picture_to_img(&output_image);
  //Edge Case: Filling in (0,0)
  copy_tile(copy_odd_output_n3, copy_tile_input, 0, 0, 3);
  ASSERT(get_pixel(copy_odd_output_n3, 0, 0) == 0x00FFFFFF);
  ASSERT(get_pixel(copy_odd_output_n3, 1, 0) == 0xFF00FFFF);
  ASSERT(get_pixel(copy_odd_output_n3, 2, 0) == 0xFF0000FF);
  ASSERT(get_pixel(copy_odd_output_n3, 0, 1) == 0xFF0000FF);
  ASSERT(get_pixel(copy_odd_output_n3, 1, 1) == 0xFF00FFFF);
  ASSERT(get_pixel(copy_odd_output_n3, 2, 1) == 0xFF00FFFF);
  //Ensure no other values are changed
  ASSERT(get_pixel(copy_odd_output_n3, 3, 0) == 0x000000FF);
  ASSERT(get_pixel(copy_odd_output_n3, 0, 2) == 0x000000FF);

  //Try edge case - bottom edge of box with 0 offset in both x and y when others have offset
  copy_tile(copy_odd_output_n3, copy_tile_input, 2, 2, 3);
  ASSERT(get_pixel(copy_odd_output_n3, 6, 3) == 0x00FFFFFF);
  ASSERT(get_pixel(copy_odd_output_n3, 7, 3) == 0xFF00FFFF);
  //Ensure pixels next to the tile are unchanged
  ASSERT(get_pixel(copy_odd_output_n3, 5, 3) == 0x000000FF);
  ASSERT(get_pixel(copy_odd_output_n3, 6, 2) == 0x000000FF);
  
  //Try edge case - offset in one box but not the other
  copy_tile(copy_odd_output_n3, copy_tile_input, 2, 1, 3);
  ASSERT(get_pixel(copy_odd_output_n3, 3, 3) == 0x00FFFFFF);
  ASSERT(get_pixel(copy_odd_output_n3, 4, 3) == 0xFF00FFFF);
  ASSERT(get_pixel(copy_odd_output_n3, 5, 3) == 0xFF0000FF);

  copy_tile(copy_odd_output_n3, copy_tile_input, 0, 2, 3);
  ASSERT(get_pixel(copy_odd_output_n3, 6, 0) == 0x00FFFFFF);
  ASSERT(get_pixel(copy_odd_output_n3, 7, 0) == 0xFF00FFFF);
  ASSERT(get_pixel(copy_odd_output_n3, 6, 1) == 0xFF0000FF);
  ASSERT(get_pixel(copy_odd_output_n3, 7, 1) == 0xFF00FFFF);

  //Ensure no other zeros are changed
  ASSERT(get_pixel(copy_odd_output_n3, 0, 3) == 0x00000FF);
}

void test_get_r( TestObjs *objs) {

//Expected red value: 0xFF
uint32_t pixel = 0xFF336699;
ASSERT(get_r(pixel) == 0xFF);

//Test with pixel having red as 0xAA: 0xAA112233
pixel = 0xAA112233;
ASSERT(get_r(pixel) == 0xAA);

//Test with pixel where red part is 0x00: 0x00334455
pixel = 0x00334455;
ASSERT(get_r(pixel) == 0x00);

//Edge case: all parts set to 0: 0x00000000
pixel = 0x00000000;
ASSERT(get_r(pixel) == 0x00);

//Edge case: all 8 bit parts set to maximum: 0xFFFFFFFF
pixel = 0xFFFFFFFF;
ASSERT(get_r(pixel) == 0xFF);

}

void test_get_g( TestObjs *objs) {

uint32_t pixel = 0xFF336699;
ASSERT(get_g(pixel) == 0x33);

//Test with green set as 0xBB
pixel = 0x11BB2233;
ASSERT(get_g(pixel) == 0xBB);

//Test with pixel where green part is 0x00: 0xFF003355
pixel = 0xFF003355;
ASSERT(get_g(pixel) == 0x00);

//Edge case: all bits set to 0: 0x00000000
pixel = 0x00000000;
ASSERT(get_g(pixel) == 0x00);

//Edge case: all bits set to maximum: 0xFFFFFFFF
pixel = 0xFFFFFFFF;
ASSERT(get_g(pixel) == 0xFF);

}

void test_get_b( TestObjs *objs) {

uint32_t pixel = 0xFF336699;
ASSERT(get_b(pixel) == 0x66); //expected is 3rd pair (bits 8-15)

//Test with blue part as 0xCC: 0x11BBCC33
pixel = 0x11BBCC33;
ASSERT(get_b(pixel) == 0xCC);

//Edge case: all bits set to 0: 0x00000000
pixel = 0x00000000;
ASSERT(get_b(pixel) == 0x00);

//Edge case: all bits set to maximum: 0xFFFFFFFF
pixel = 0xFFFFFFFF;
ASSERT(get_b(pixel) == 0xFF);

}

void test_get_a( TestObjs *objs) {

uint32_t pixel = 0xFF336699;
ASSERT(get_a(pixel) == 0x99); //expected is bits (0-7) = 99 in hex

//Test with pixel having alpha as 0xDD: 0x11BB22DD
pixel = 0x11BB22DD;
ASSERT(get_a(pixel) == 0xDD);

// Edge case: all channels set to 0: 0x00000000
pixel = 0x00000000;
ASSERT(get_a(pixel) == 0x00);

// Edge case: all channels set to maximum: 0xFFFFFFFF
pixel = 0xFFFFFFFF;
ASSERT(get_a(pixel) == 0xFF);

}

//TODO: Add these tests to list of tests to run

void test_to_grayscale( TestObjs *objs) {

uint32_t pixel, gray_pixel, expected_gray;

//Red pixel (R=255, G=0, B=0, A=255)
pixel = 0xFF0000FF;
gray_pixel = to_grayscale(pixel);
expected_gray = ((79 * 255) + (128 * 0) + (49 * 0)) / 256; // expected grayscale value
ASSERT(get_r(gray_pixel) == expected_gray);
ASSERT(get_g(gray_pixel) == expected_gray);
ASSERT(get_b(gray_pixel) == expected_gray);
ASSERT(get_a(gray_pixel) == 0xFF);  // alpha should remain the same

//Green pixel 50% opacity (R=0, G=255, B=0, A=128)
pixel = 0x00FF0080; 
gray_pixel = to_grayscale(pixel);
expected_gray = ((79 * 0) + (128 * 255) + (49 * 0)) / 256; //expected grayscale value
ASSERT(get_r(gray_pixel) == expected_gray);
ASSERT(get_g(gray_pixel) == expected_gray);
ASSERT(get_b(gray_pixel) == expected_gray);
ASSERT(get_a(gray_pixel) == 0x80);  //alpha should remain the same

//Edge case (R=0, G=0, B=0, A=255): black with full opacity
pixel = 0x000000FF;
gray_pixel = to_grayscale(pixel);
ASSERT(get_r(gray_pixel) == 0x00);
ASSERT(get_g(gray_pixel) == 0x00);
ASSERT(get_b(gray_pixel) == 0x00);
ASSERT(get_a(gray_pixel) == 0xFF);  //alpha should remain the same

//Edge case (R=255, G=255, B=255, A=255): white with full opacity
pixel = 0xFFFFFFFF; 
gray_pixel = to_grayscale(pixel);
ASSERT(get_r(gray_pixel) == 0xFF); 
ASSERT(get_g(gray_pixel) == 0xFF);
ASSERT(get_b(gray_pixel) == 0xFF);
ASSERT(get_a(gray_pixel) == 0xFF);  //alpha should remain the same

//Edge case (R=100, G=150, B=200, A=0): mixed color fully transparent
pixel = 0x6496C800; 
gray_pixel = to_grayscale(pixel);
expected_gray = ((79 * 100) + (128 * 150) + (49 * 200)) / 256; //expected grayscale value
ASSERT(get_r(gray_pixel) == expected_gray);
ASSERT(get_g(gray_pixel) == expected_gray);
ASSERT(get_b(gray_pixel) == expected_gray);
ASSERT(get_a(gray_pixel) == 0x00);  //alpha should remain the same

}

void test_blend_components( TestObjs *objs) {

}

void test_blend_colors( TestObjs *objs) {

}