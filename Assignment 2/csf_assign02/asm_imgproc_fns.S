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
 * uint32_t make_pixel(uint32_t r, uint32_t g, uint32_t b, uint32_t a) 
 * return (r << 24) | (g << 16) | (b << 8) | a;
 * 
 * Register Use:
 *   %edi - r
 *   %esi - g
 *   %edx - b
 *   %ecx - a
 *   %eax - Used as temp in computation, also pixel value to return
 */
.globl make_pixel

make_pixel:

# Adjust the stack to maintain 16-byte alignment
subq $8, %rsp

movl %edi, %eax  # place r into temp register
shll $24, %eax    # left shift r by 24
shll $16, %esi    # left shift g by 16
shll $8, %edx     # left shift b by 8

or %esi, %eax    # store (r << 24) | (g << 16) in %eax
or %edx, %eax    # store (r << 24) | (g << 16) | (b << 8) in %eax
or %ecx, %eax    # or with a to get final result

# Restore the stack pointer
addq $8, %rsp

ret

/*
 * uint32_t get_pixel(struct Image *img, int32_t x, int32_t y);
 * 
 * Register Use:
 *   %rdi - pointer to struct Image
 *   %esi - x coordinate (int32_t)
 *   %edx - y coordinate (int32_t)
 *   %r10 - Used to compute the pixel index
 *   %eax - pixel value to return
 *   %r12, %r14 - callee-saved registers (need to be pushed and popped)
 */
.globl get_pixel

get_pixel:
    # Adjust the stack to maintain 16-byte alignment
    subq $8, %rsp

    # Push callee-saved registers
    pushq %r12
    pushq %r14

    # Calculate y * img->width
    movq %rdi, %r12                         # Save img pointer in r12
    movl IMAGE_WIDTH_OFFSET(%r12), %r14d    # Load img->width into r14d
    movl %r14d, %r10d                       # Move img->width into r10d
    imull %edx, %r10d                       # Multiply y (edx) * width (r10d)

    # Add x to the result (y * img->width + x)
    addl %esi, %r10d                        # r10d = y * width + x

    # Load the base address of img->data
    mov IMAGE_DATA_OFFSET(%r12), %rdi       # Load img->data (address)

    # Retrieve the pixel at img->data[y * width + x]
    movl (%rdi, %r10, 4), %eax              # Load the pixel at img->data[index]

    movq %r12, %rdi

    # Restore callee-saved registers
    popq %r14
    popq %r12

    # Restore the stack pointer
    addq $8, %rsp

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
 *  %eax - holds pixel value
 *  %r10 - used for index computation (y * width + x)
 *  %r12, %r13 - callee-saved registers used for computations
 */
.globl set_pixel
set_pixel:

    # Adjust the stack to maintain 16-byte alignment
    subq $8, %rsp

    # Push callee-saved registers
    pushq %r12
    pushq %r13

    # Move the pixel value from %ecx to %eax
    movl %ecx, %eax                          # Move pixel value into eax

    # Calculate y * img->width
    movq %rdi, %r12                          # Save img pointer in %r12
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d     # Load img->width into %r13d
    imull %edx, %r13d                        # Multiply y (edx) * width (%r13d)

    # Add x to the result (y * img->width + x)
    addl %esi, %r13d                         # r13d = y * width + x

    # Load the base address of img->data
    movq IMAGE_DATA_OFFSET(%r12), %r12        # Load img->data (address) into %r12

    # Store the pixel value at img->data[y * width + x]
    movl %eax, (%r12, %r13, 4)               # Store pixel at img->data[index]

    # Pop callee-saved registers
    popq %r13
    popq %r12

    # Restore the stack pointer
    addq $8, %rsp

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
*  %r12d - callee-saved register for computation
*  
*/
.globl to_grayscale
to_grayscale:

pushq %r12                   # push callee-saved registers

# Get R, G, B, and A components from the pixel

call get_r                   # Call get_r(pixel), result in %eax
imull $79, %eax              # Multiply R value by 79
movl %eax, %r12d             # Store result in %r12 (for later use)

call get_g                   # Call get_g(pixel), result in %eax
imull $128, %eax             # Multiply G value by 128
addl %eax, %r12d             # Add to previous result in %r10

call get_b                   # Call get_b(pixel), result in %eax
imull $49, %eax              # Multiply B value by 49
addl %eax, %r12d             # Add to result in %r10

# Now divide the total by 256 to get the gray value

sarl $8, %r12d               # Arithmetic right shift by 8 (divide by 2^8=256)

# Get the A value from the pixel
call get_a                  # Call get_a(pixel), result in %ebx

# Create the grayscale pixel using make_pixel(gray_value, gray_value, gray_value, a)
movl %r12d, %esi             # Copy gray_value for green channel
movl %r12d, %edx             # Copy gray_value for blue channel
movl %r12d, %edi             # Move gray_value to %rdi (R)
movl %eax, %ecx             # Move A value to %rcx (A)
    
call make_pixel             # Call make_pixel(R, G, B, A), stored in %eax

popq %r12                   # Pop callee-saved register

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
    # Push callee-saved registers
    pushq %r12
    pushq %r15

    # Move pointers to the input and output images into callee-saved registers
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

    # Initialize x counter variable
    movl $0, %esi

h_inner_x_loop:
    # Compare x with total width
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d
    cmpl %r13d, %esi

    # If x >= Image width, end the inner loop
    jae h_x_loop_end

    # Get pixel of the input image at the current position
    call get_pixel

    # Move the pixel into the pixel parameter for set_pixel
    movl %eax, %ecx

    # Store the original x position
    movl %esi, %r14d
    movq %rdi, %r11

    # Update %esi to hold the inversed width
    movl $0, %esi
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d
    addl %r13d, %esi
    subl $1, %esi
    subl %r14d, %esi

    # Prepare for the set_pixel call
    movq %r15, %rdi  # output image pointer

    # Set the pixel at the computed position
    call set_pixel

    # Restore the original x position
    movl %r14d, %esi
    movq %r11, %rdi  # restore input image pointer

    # Increment x counter and loop again
    incl %esi
    jmp h_inner_x_loop

h_x_loop_end:
    incl %edx
    jmp h_outer_y_loop

h_y_loop_end:
    # Restore callee-saved registers
    popq %r15
    popq %r12

    ret  # return

    

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
    # Push callee-saved registers
    pushq %r12
    pushq %r13
    pushq %r15

    # Move pointers to the input and output images into callee-saved registers
    movq %rdi, %r12
    movq %rsi, %r15

    # Initialize y counter variable
    movl $0, %edx

v_outer_y_loop:
    movl IMAGE_HEIGHT_OFFSET(%r12), %r13d  # Store height in %r13
    # Compare y with the total height
    cmpl %r13d, %edx  

    # If y >= Image height, end the outer loop
    jae v_y_loop_end

    # Initialize x counter variable
    movl $0, %esi

v_inner_x_loop:
    # Compare x with total width
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d  # Update width in %r13
    cmpl %r13d, %esi

    # If x >= Image width, end the inner loop
    jae v_x_loop_end

    # Get pixel of the input image at the current position
    call get_pixel

    # Move the pixel into the pixel parameter for set_pixel
    movl %eax, %ecx

    # Store the original y value and original input image pointer
    movl %edx, %r14d
    movq %rdi, %r11

    # Update %edx to hold the inversed height
    movl $0, %edx
    movl IMAGE_HEIGHT_OFFSET(%r12), %r13d  # Reusing %r13
    addl %r13d, %edx
    subl $1, %edx
    subl %r14d, %edx

    # Prepare for the set_pixel call
    movq %r15, %rdi  # output image pointer

    # Set the pixel at the computed position
    call set_pixel

    # Restore the original y value and original input image pointer
    movl %r14d, %edx
    movq %r11, %rdi  # restore input image pointer

    # Increment x counter and loop again
    incl %esi
    jmp v_inner_x_loop

v_x_loop_end:
    incl %edx
    jmp v_outer_y_loop

v_y_loop_end:
    # Restore callee-saved registers
    popq %r15
    popq %r13
    popq %r12

    ret  # return


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
    # Push callee-saved registers (r14, r15)
    pushq %r14
    pushq %r15

    # Move pointers to the input and output images into callee-saved registers
    movq %rdi, %r14  # %r14 holds the input image pointer
    movq %rsi, %r15  # %r15 holds the output image pointer

    # Initialize y counter variable
    movl $0, %edx

gray_outer_y_loop:
    movl IMAGE_HEIGHT_OFFSET(%r14), %r13d
    # Compare y with the total height
    cmpl %r13d, %edx  

    # If y >= Image height, end the outer loop
    jae gray_y_loop_end

    # Initialize x counter variable
    movl $0, %esi

gray_inner_x_loop:
    # Compare x with total variable
    movl IMAGE_WIDTH_OFFSET(%r14), %r13d
    cmpl %r13d, %esi

    # If x >= Image width, end the inner loop
    jae gray_x_loop_end

    # Get pixel of the input image at the current coordinates 
    call get_pixel

    movl %edx, %r10d
    movl %esi, %r11d

    # Move the pixel into the pixel parameter for to_grayscale
    movl %eax, %edi

    # Call the to_grayscale function
    call to_grayscale 

    movl %eax, %ecx
    
    # Add the output image pointer into the input image pointer for set_pixel
    movq %r15, %rdi

    movl %r10d, %edx
    movl %r11d, %esi

    # Set the pixel at the provided position
    call set_pixel

    movq %r14, %rdi

    # Increase the x counter and jump in the loop again
    incl %esi
    jmp gray_inner_x_loop

gray_x_loop_end:
    incl %edx
    jmp gray_outer_y_loop

gray_y_loop_end:
    # Pop callee-saved registers
    popq %r15
    popq %r14

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
