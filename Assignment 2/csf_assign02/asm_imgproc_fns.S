/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

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

movl %edi, %eax  # place r into temp register
shll $24, %eax    # left shift r by 24
shll $16, %esi    # left shift g by 16
shll $8, %edx     # left shift b by 8

or %esi, %eax    # store (r << 24) | (g << 16) in %eax
or %edx, %eax    # store (r << 24) | (g << 16) | (b << 8) in %eax
or %ecx, %eax    # or with a to get final result

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

    # Save img pointer in r12
    movq %rdi, %r12   

    # Load img->width into r14d
    movl IMAGE_WIDTH_OFFSET(%r12), %r14d   

    # Move img->width into r10d 
    movl %r14d, %r10d  

    # Multiply y (edx) * width (r10d)                     
    imull %edx, %r10d                       

    # Add x to the result (y * img->width + x)

    # r10d = y * width + x
    addl %esi, %r10d                        

    # Load the base address of img->data

    # Load img->data (address)
    mov IMAGE_DATA_OFFSET(%r12), %rdi       

    # Retrieve the pixel at img->data[y * width + x]

    # Load the pixel at img->data[index]
    movl (%rdi, %r10, 4), %eax             

    # Restore the img pointer to rdi
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

    # Move pixel value into eax
    movl %ecx, %eax                          

    # Calculate y * img->width

    # Save img pointer in %r12
    movq %rdi, %r12   

    # Load img->width into %r13d                       
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d 

    # Multiply y (edx) * width (%r13d)    
    imull %edx, %r13d                        

    # Add x to the result (y * img->width + x)

    # r13d = y * width + x
    addl %esi, %r13d                         

    # Load the base address of img->data

    # Load img->data (address) into %r12
    movq IMAGE_DATA_OFFSET(%r12), %r12        

    # Store the pixel value at img->data[y * width + x]

    # Store pixel at img->data[index]
    movl %eax, (%r12, %r13, 4)               

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

    # Move pixel to eax
    movl %edi, %eax      

    # Shift right by 24
    shrl $24, %eax       

    # Mask with 0xFF
    andl $0xFF, %eax   

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

    # Move pixel to eax
    movl %edi, %eax  

    # Shift right by 16
    shrl $16, %eax 

    # Mask with 0xFF            
    andl $0xFF, %eax     

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

    # Move pixel to eax
    movl %edi, %eax     

    # Shift right by 8       
    shrl $8, %eax  

    # Mask with 0xFF            
    andl $0xFF, %eax   

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

    # Move pixel to eax
    movl %edi, %eax        

    # Mask with 0xFF
    andl $0xFF, %eax   

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

    # Push callee-saved registers
    pushq %r12                   

    # Get R, G, B, and A components from the pixel

    # Call get_r(pixel), result in %eax
    call get_r  

    # Multiply R value by 79                 
    imull $79, %eax 

    # Store result in %r12 (for later use)             
    movl %eax, %r12d      
     
    # Call get_g(pixel), result in %eax
    call get_g     

    # Multiply G value by 128
    imull $128, %eax  

    # Add to previous result in %r10
    addl %eax, %r12d            

    # Call get_b(pixel), result in %eax
    call get_b  

    # Multiply B value by 49                 
    imull $49, %eax 

    # Add to result in %r10             
    addl %eax, %r12d             

    # Now divide the total by 256 to get the gray value using an arithmetic right shift by 8 
    sarl $8, %r12d               

    # Get the A value from the pixel by calling get_a(pixel), result in %ebx
    call get_a                  

    # Create the grayscale pixel using make_pixel(gray_value, gray_value, gray_value, a)

    # Copy gray_value for green channel
    movl %r12d, %esi 

    # Copy gray_value for blue channel            
    movl %r12d, %edx 

    # Move gray_value to %rdi (R)            
    movl %r12d, %edi 

    # Move A value to %rcx (A)           
    movl %eax, %ecx             
    
    # Call make_pixel(R, G, B, A), stored in %eax
    call make_pixel    

    # Pop callee-saved register         
    popq %r12                   

     # return %eax
    ret                        


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
 * Registers:
 *   %r11 - used to restore the original input image
 *   %r12 - callee-saved register to hold original struct Image
 *   %r13 - callee-saved register to hold image widths and heights
 *   %r14 - callee-saved register to hold original x value
 *   %r15 - callee-saved register to hold output struct Image
 */
	.globl imgproc_mirror_h
imgproc_mirror_h:

    # Aligning stack pointer
    subq $8, %rsp

    # Push callee-saved registers
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    # Move pointers to the input and output images into callee-saved registers r12 and r15
    movq %rdi, %r12
    movq %rsi, %r15

    # Initialize y counter variable
    movl $0, %edx

# Outer Loop to compare y value with height
h_outer_y_loop:
    # Move image height into r13d
    movl IMAGE_HEIGHT_OFFSET(%r12), %r13d

    # Compare y with the total height
    cmpl %r13d, %edx  

    # If y >= Image height, end the outer loop
    jae h_y_loop_end

    # Initialize x counter variable
    movl $0, %esi

# Inner loop to execute body of code
h_inner_x_loop:
    # Move image width into r13d 
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d

    # Compare x with total width
    cmpl %r13d, %esi

    # If x >= Image width, end the inner loop
    jae h_x_loop_end

    # Get pixel of the input image at the current position
    call get_pixel

    # Move the pixel into the pixel parameter for set_pixel
    movl %eax, %ecx

    # Store the original x position in r14d
    movl %esi, %r14d

    # Store the input image pointer in r11
    movq %rdi, %r11

    # Update %esi to hold the inversed width
    movl $0, %esi

    # Move image width into r13d 
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d

    # Add image width to %esi
    addl %r13d, %esi

    # Subtract (1 + original_x) from esi
    subl $1, %esi
    subl %r14d, %esi

    # Prepare for the set_pixel call by setting r15 to output image pointer
    movq %r15, %rdi  

    # Set the pixel at the computed position
    call set_pixel

    # Restore the original x position
    movl %r14d, %esi

     # Restore input image pointer
    movq %r11, %rdi 

    # Increment x counter and loop again
    incl %esi
    jmp h_inner_x_loop

# Code to reset the inner loop once inner body is finished
h_x_loop_end:
    # At the end of the x loop, increment y and start the loop again
    incl %edx
    jmp h_outer_y_loop

# Code to run once outer loop is finished
h_y_loop_end:
    # Restore callee-saved registers
    popq %r15
    popq %r14
    popq %r13
    popq %r12

    # Aligning stack pointer
    addq $8, %rsp
    ret  # return

    
/*
 * void imgproc_mirror_v( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by mirroring its pixels vertically.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 * Registers:
 *   %r12 - callee-saved register to hold original struct Image
 *   %r13 - callee-saved register to hold image widths and heights
 *   %r14 - callee-saved register to hold original y value
 *   %r15 - callee-saved register to hold output struct Image
 */

	.globl imgproc_mirror_v
imgproc_mirror_v:
    # Adjust the stack to maintain 16-byte alignment
    subq $8, %rsp

    # Push callee-saved registers
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    # Move pointer to the input image into %r12 
    movq %rdi, %r12

    # Move pointer to output image into callee-saved registers
    movq %rsi, %r15

    # Initialize y counter variable
    movl $0, %edx

# Code to determine whether outer loop should iterate again
v_outer_y_loop:
    # Store height in %r13d
    movl IMAGE_HEIGHT_OFFSET(%r12), %r13d  

    # Compare y with the total height
    cmpl %r13d, %edx  

    # If y >= Image height, end the outer loop
    jae v_y_loop_end

    # Initialize x counter variable
    movl $0, %esi

v_inner_x_loop:
    # Update image width in %r13d
    movl IMAGE_WIDTH_OFFSET(%r12), %r13d  

    # Compare x with total width
    cmpl %r13d, %esi

    # If x >= Image width, end the inner loop
    jae v_x_loop_end

    # Get pixel of the input image at the current position
    call get_pixel

    # Move the pixel into the pixel parameter for set_pixel
    movl %eax, %ecx

    # Store the original y value in %r14d 
    movl %edx, %r14d

    # Update %edx to hold the inversed height
    movl $0, %edx

    # Update image height in %r13d
    movl IMAGE_HEIGHT_OFFSET(%r12), %r13d 

    # Compute inversed y value by first adding y height
    addl %r13d, %edx

    # Subtract (1 + original_y) from the inversed height
    subl $1, %edx
    subl %r14d, %edx

    # Prepare for the set_pixel call by moving output image in %rdi
    movq %r15, %rdi  

    # Set the pixel at the computed position
    call set_pixel

    # Restore the original y value into %edx 
    movl %r14d, %edx

    # Restore input image pointer into %rdi
    movq %r12, %rdi  

    # Increment x counter and loop again
    incl %esi

    # Repeat the loop
    jmp v_inner_x_loop

# Describes what to do once the inner loop ends
v_x_loop_end:
    # Increae y pixel value by 1
    incl %edx

    # Restart check for outer loop
    jmp v_outer_y_loop

# Describes what to do once the loop ends
v_y_loop_end:
    # Restore callee-saved registers
    popq %r15
    popq %r14
    popq %r13
    popq %r12

    # Restore the stack pointer
    addq $8, %rsp

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
	/* Will be implemented in Assignment 3, right now returns 0 */
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

 * Registers:
 *   %r10 - Used to hold original x value
 *   %r11 - Used to hold original y value
 *   %r13 - callee-saved register to hold image widths and heights 
 *   %r14 - callee-saved register to hold original struct Image
 *   %r15 - callee-saved register to hold output struct Image
 */
	.globl imgproc_grayscale
imgproc_grayscale:
    # Push callee-saved registers (r13, r14, r15)
    pushq %r13
    pushq %r14
    pushq %r15

    # Move input image pointer into %r14 
    movq %rdi, %r14 

    # Move output image pointer into %r15
    movq %rsi, %r15

    # Initialize y counter variable
    movl $0, %edx

# Outer loop checks to see whether to proceed to go to inner loop
gray_outer_y_loop:
    # Store image height into %r13d
    movl IMAGE_HEIGHT_OFFSET(%r14), %r13d

    # Compare y with the total height
    cmpl %r13d, %edx  

    # If y >= Image height, end the outer loop
    jae gray_y_loop_end

    # Initialize x counter variable
    movl $0, %esi

# Proceed to the inner loop and execute the body of code
gray_inner_x_loop:
    # Storage image width into %r13d
    movl IMAGE_WIDTH_OFFSET(%r14), %r13d

    # Compare x with total width variable
    cmpl %r13d, %esi

    # If x >= Image width, end the inner loop
    jae gray_x_loop_end

    # Get pixel of the input image at the current coordinates 
    call get_pixel

    # Move the original y value into %r10d
    movl %edx, %r10d

    # Move the original x value into %r11d
    movl %esi, %r11d

    # Move the pixel into the pixel parameter for to_grayscale
    movl %eax, %edi

    # Call the to_grayscale function
    call to_grayscale 

    # Move the returned pixel into %ecx for set_pixel
    movl %eax, %ecx
    
    # Add the output image pointer into the input image pointer for set_pixel
    movq %r15, %rdi

    # Restore the original y value
    movl %r10d, %edx

    # Restore the original x value
    movl %r11d, %esi

    # Set the pixel at the provided position
    call set_pixel

    # Move the original input value into %rdi
    movq %r14, %rdi

    # Increase the x counter and jump in the loop again
    incl %esi

    # Repeat the loop
    jmp gray_inner_x_loop

# Code to execute once the outer loop finishes
gray_x_loop_end:
    # Increae the y value by 1
    incl %edx

    # Reset the outer loop 
    jmp gray_outer_y_loop

# Code to execute once the loop ends
gray_y_loop_end:
    # Pop callee-saved registers
    popq %r15
    popq %r14
    popq %r13

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
	/* Will be implemented in Assignment 3, right now returns 0 */
	xorl %eax, %eax
	ret

/*
vim:ft=gas:
*/
