/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/*
 * TODO: define your helper functions here.
 * Don't forget to use the .globl directive to make
 * them visible to the test code.
 */

/*
 * uint32_t get_pixel(struct Image *img, int32_t x, int32_t y);
 * 
 * Register Use:
 *   %rdi - pointer to struct Image
 *   %esi - x coordinate (int32_t)
 *   %edx - y coordinate (int32_t)
 * 	 %r10 - Used to compute the pixel index
 *   %eax - pixel value to return
 */
.globl get_pixel

get_pixel:

    # Calculate y * img->width
    movq %rdi, %r12
    movl IMAGE_WIDTH_OFFSET(%r12), %r14d
    movl %r14d, %r10d   # Load img->width into r10d
    imull %edx, %r10d                      # Multiply y (edx) * width (r10d)

    # Add x to the result (y * img->width + x)
    addl %esi, %r10d                       # r10d = y * width + x

    # Load the base address of img->data
    mov IMAGE_DATA_OFFSET(%rdi), %rdi       # Load img->data (address)

    # Retrieve the pixel at img->data[y * width + x]
    movl (%rdi, %r10, 4), %eax              # Load the pixel at img->data[index]

    movq %r12, %rdi

    ret


/*
 * void set_pixel(struct Image *img, int32_t x, int32_t y, uint32_t pixel) 
 *   img->data[y * img->width + x] = pixel;
 *
 * Register Use:
 *  %rdi - pointer to struct Image (input_img)
 *  %esi - x coordinate (int32_t)
 *  %edx - y coordinate (int32_t)
 *  %ecx - pixel parameter
 *  %eax - hold pixel value
 *  %r10 - used for index computation (y * width + x)
 */
.globl set_pixel
set_pixel:

   # Move the pixel value from %ecx to %eax
    movl %ecx, %eax                          # Move pixel value into eax

    # Calculate y * img->width
    movq %rdi, %r12
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d
    movl %r14d, %r13d   # Load img->width into r10d

    imull %edx, %r10d                         # Multiply y (edx) * width (r10)

    # Add x to the result (y * img->width + x)
    addl %esi, %r10d                          # r10 = y * width + x

    # Load the base address of img->data
    mov IMAGE_DATA_OFFSET(%rdi), %rdi       # Load img->data (address)

    # Store the pixel value at img->data[y * width + x]
    movl %eax, (%rdi, %r10, 4)               # Store pixel at img->data[index]

    ret

/*
 * uint32_t get_r(uint32_t pixel) 
 *  return (pixel >> 24) & 0xFF;
 *
 * Register Use:
 *  %rdi - uint32_t pixel parameter
 *  %eax - return value (R)
 *  
 */
.globl get_r
get_r:

    # Shift pixel right by 24 bits and mask with 0xFF to get the R value
    movl %edi, %eax            # Move pixel to eax
    shrl $24, %eax             # Shift right by 24
    andl $0xFF, %eax           # Mask with 0xFF
    ret


/*
 * uint32_t get_g(uint32_t pixel) 
 *  return (pixel >> 16) & 0xFF;
 *
 * Register Use:
 *  %rdi - uint32_t pixel parameter
 *  %eax - return value (G)
 *  
 */
.globl get_g
get_g:

    # Shift pixel right by 16 bits and mask with 0xFF to get the G value
    movl %edi, %eax            # Move pixel to eax
    shrl $16, %eax             # Shift right by 16
    andl $0xFF, %eax           # Mask with 0xFF
    ret

/*
 * uint32_t get_b(uint32_t pixel) 
 *  return (pixel >> 8) & 0xFF;
 *
 * Register Use:
 *  %rdi - uint32_t pixel parameter
 *  %eax - return value (B)
 *  
 */
.globl get_b
get_b:

    # Shift pixel right by 8 bits and mask with 0xFF to get the B value
    movl %edi, %eax            # Move pixel to eax
    shrl $8, %eax              # Shift right by 8
    andl $0xFF, %eax           # Mask with 0xFF
    ret

/*
 * uint32_t get_a(uint32_t pixel) 
 *  return pixel & 0xFF;
 *
 * Register Use:
 *  %rdi - uint32_t pixel parameter
 *  %eax - return value (A)
 *  
 */
.globl get_a
get_a:

    # Mask pixel with 0xFF to get the A value
    movl %edi, %eax            # Move pixel to eax
    andl $0xFF, %eax           # Mask with 0xFF
    ret

/*
* uint32_t to_grayscale( uint32_t pixel ) 
*  uint32_t gray_value = ((79 * get_r(pixel)) + (128 * get_g(pixel)) + (49 * get_b(pixel))) / 256;
*  return make_pixel(gray_value, gray_value, gray_value, get_a(pixel));
*  
*  Register Use:
*  %rdi - uint32_t pixel parameter
*  
*  
*/
.globl to_grayscale
to_grayscale:

# Get R, G, B, and A components from the pixel

call get_r                  # Call get_r(pixel), result in %eax
imull $79, %eax             # Multiply R value by 79
movl %eax, %r10d             # Store result in %r10 (for later use)

call get_g                  # Call get_g(pixel), result in %eax
imull $128, %eax            # Multiply G value by 128
addl %eax, %r10d             # Add to previous result in %r10

call get_b                  # Call get_b(pixel), result in %eax
imull $49, %eax             # Multiply B value by 49
addl %eax, %r10d             # Add to result in %r10

# Now divide the total by 256 to get the gray value

movl %r10d, %eax             # Move sum into %eax
sarl $8, %eax               # Arithmetic right shift by 8 (divide by 2^8=256)

# Get the A value from the pixel
call get_a                  # Call get_a(pixel), result in %ebx

# Create the grayscale pixel using make_pixel(gray_value, gray_value, gray_value, a)
movl %eax, %r10d             # Move gray_value to %r10 (for pixel creation)
movl %r10d, %ecx             # Copy gray_value for green channel
movl %r10d, %edx             # Copy gray_value for blue channel

# Now call make_pixel
movq %r10, %rdi             # Move gray_value to %rdi (R)
movq %rcx, %rsi             # Move gray_value to %rsi (G)
movq %rdx, %rdx             # Move gray_value to %rdx (B)
movq %rbx, %rcx             # Move A value to %rcx (A)
    
# call make_pixel             # Call make_pixel(R, G, B, A), stored in %eax
ret                         # return %eax

/*
 * Implementations of API functions
 */

/*
 * void imgproc_mirror_h( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by mirroring its pixels horizontally.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_mirror_h
imgproc_mirror_h:
    movq %rdi, %r12
    movq %rsi, %r15
    # Initialize y counter variable
    movl $0, %edx

h_outer_y_loop:
    
    movl IMAGE_HEIGHT_OFFSET(%r12), %r13d
    # Compare y with the total height
    cmpl %r13d, %edx  

    # If y >= Image height, end the outer loop
    jae h_y_loop_end

    # Intiailze x counter variable
    movl $0, %esi

h_inner_x_loop:
    # Compare x with total variable
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d
    cmpl %r13d, %esi

    # If x ?= Image width, end the inner loop
    jae h_x_loop_end

    # Get pixel of the input image at the input image 
    call get_pixel

    # Move the pixel into the pixel parameter for set_pixel
    movl %eax, %ecx

    # Store the original width value and original input image pointer
    movl %esi, %r14d
    movq %rdi, %r11

    # Update the %esi register to hold the inversed width
    movl $0, %esi
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d
    addl %r13d, %esi
    subl $1, %esi
    subl %r14d, %esi

    # Add the output image pointer into the input image pointer for set_pixel
    movq %r15, %rdi

    # Set the pixel at the provided position
    call set_pixel

    # Restore the original width value and original input image pointer
    movl %r14d, %esi
    movq %r11, %rdi

    # Increase the x counter and jump in the loop again
    incl %esi
    jmp h_inner_x_loop

h_x_loop_end:
    incl %edx
    jmp h_outer_y_loop

h_y_loop_end:
    movq %r15, %rsi
    ret
    

	/*
     * Register Use:
 * uint32_t get_pixel(struct Image *img, int32_t x, int32_t y);
 * 
 * Register Use:
 *   %rdi - pointer to struct Image
 *   %esi - x coordinate (int32_t)
 *   %edx - y coordinate (int32_t)
 * 	 %r10 - Used to compute the pixel index
 *   %eax - pixel value to return
 *

  * void set_pixel(struct Image *img, int32_t x, int32_t y, uint32_t pixel) 
 *   img->data[y * img->width + x] = pixel;
 *
 * Register Use:
 *  %rdi - pointer to struct Image (input_img)
 *  %esi - x coordinate (int32_t)
 *  %edx - y coordinate (int32_t)
 *  %ecx - pixel parameter
 *  %eax - hold pixel value
 *  %r10 - used for index computation (y * width + x)
    */

/*
 * void imgproc_mirror_v( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by mirroring its pixels vertically.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_mirror_v
imgproc_mirror_v:
    # Initialize y counter variable
    movl $0, %edx

v_outer_y_loop:
    # Load Image height into %r10
    movl IMAGE_HEIGHT_OFFSET(%rdi), %r10d

    # Compare y with the total width
    cmpl %edx, %r10d   

    # If y >= Image height, end the outer loop
    jae v_y_loop_end

    # Intiailze x counter variable
    movl $0, %esi

v_inner_x_loop:
    # Load Image width into %r10
    movl IMAGE_WIDTH_OFFSET(%rdi), %r10d

    # Compare x with total variable
    cmpl %esi, %r10d

    # If x ?= Image width, end the inner loop
    jae v_x_loop_end

    # Get pixel of the input image at the input image 
    call get_pixel

    # Move the pixel into the pixel parameter for set_pixel
    movl %eax, %ecx

    # Store the original height value and original input image pointer
    movl %edx, %r10d
    movq %rdi, %r11

    # Update the %edx register to hold the inversed height
    movl $0, %edx
    addl IMAGE_WIDTH_OFFSET(%rdi), %edx
    subl $1, %edx
    subl %r10d, %edx

    # Add the output image pointer into the input image pointer for set_pixel
    movq %r15, %rdi

    # Set the pixel at the provided position
    call set_pixel

    # Restore the original width value and original input image pointer
    movl %r10d, %esi
    movq %r11, %rdi

    # Increase the x counter and jump in the loop again
    incl %esi
    jmp v_inner_x_loop

v_x_loop_end:
    incl %edx
    jmp v_outer_y_loop

v_y_loop_end:
    ret

/*
 * int imgproc_tile( struct Image *input_img, int n, struct Image *output_img );
 *
 * Transform image by generating a grid of n x n smaller tiles created by
 * sampling every n'th pixel from the original image.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %esi - tiling factor (how many rows and columns of tiles to generate)
 *   %rdx - pointer to the output Image (in which the transformed
 *          pixels should be stored)
 *
 * Returns (in %eax):
 *   1 if successful, or 0 if either
 *     - n is less than 1, or
 *     - the output can't be generated because at least one tile would
 *       be empty (i.e., have 0 width or height)
 */
	.globl imgproc_tile
imgproc_tile:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
 * void imgproc_grayscale( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by converting each pixel to grayscale.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_grayscale
imgproc_grayscale:
    # Initialize y counter variable
    movl $0, %edx

gray_outer_y_loop:
    # Load Image height into %r10
    movl IMAGE_HEIGHT_OFFSET(%rdi), %r10d

    # Compare y with the total width
    cmpl %edx, %r10d    

    # If y >= Image height, end the outer loop
    jae gray_y_loop_end

    # Intiailze x counter variable
    movl $0, %esi

gray_inner_x_loop:
    # Load Image width into %r10
    movl IMAGE_WIDTH_OFFSET(%rdi), %r10d

    # Compare x with total variable
    cmpl %esi, %r10d

    # If x ?= Image width, end the inner loop
    jae gray_x_loop_end

    call get_pixel


    movq %rdi, %r11
    movq %rax, %rdi

    # Get pixel of the input image at the input image 
    call to_grayscale

    # Move the pixel into the pixel parameter for set_pixel
    movl %eax, %ecx

    # Add the output image pointer into the input image pointer for set_pixel
    movq %rsi, %rdi

    # Set the pixel at the provided position
    call set_pixel

    # Restore the original input image pointer
    movq %r11, %rdi

    # Increase the x counter and jump in the loop again
    incl %esi
    jmp gray_inner_x_loop

gray_x_loop_end:
    incl %edx
    jmp gray_outer_y_loop

gray_y_loop_end:
    ret

/*
 * int imgproc_composite( struct Image *base_img, struct Image *overlay_img );
 *
 * Overlay a foreground image on a background image, using each foreground
 * pixel's alpha value to determine its degree of opacity in order to blend
 * it with the corresponding background pixel.
 *
 * Parameters:
 *   %rdi - pointer to base (background) image
 *   %rsi - pointer to overlaid (foreground) image
 *   %rdx - pointer to output Image
 *
 * Returns:
 *   1 if successful, or 0 if the transformation fails because the base
 *   and overlay image do not have the same dimensions
 */
	.globl imgproc_composite
imgproc_composite:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
vim:ft=gas:
*/
