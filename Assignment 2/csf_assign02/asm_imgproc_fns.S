/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8


/* Returns 1 if all tiles nonempty(true), 0 if any tiles would be empty (false)

    Arguments:
      - rdi: width (int)
      - rsi: height (int)
      - rdx: n (int)

    Return:
      - eax: 1 if all tiles are non-empty (true), 0 if any tiles would be empty (false)

    Registers:
      - rdi: width (input)
      - rsi: height (input)
      - rdx: n (input, divisor)
      - eax: stores result of division and final return value
      - edx: used for sign-extension of eax during division
      - r9d: backup for n (divisor)
      - r10d: stores width / n result
      - r11d: stores height / n result
*/

.globl all_tiles_nonempty
all_tiles_nonempty:

# Align stack pointer (good style)
subq $8, %rsp

# If n == 0, return 0 (false)
cmpl $0, %edx     # Test if %edx (n) is zero
je .LFalseCondition  # If zero, jump to false condition

# Save n to the stack (since %edx will be overwritten by cdq)
movl %edx, %r9d      # Save the divisor n in %r9d

# place width in eax for division
movl %edi, %eax      # Move width to %eax (32-bit register)
cdq                  # Sign-extend %eax into %edx for signed division

# width / n
idivl %r9d           # Signed division of %edx:%eax by %r9d (n)

# store width / n
movl %eax, %r10d     # Store result of width / n in %r10d

# place height in eax for division
movl %esi, %eax      # Move height to %eax (32-bit register)
cdq                  # Sign-extend %eax into %edx for signed division

# height / n
idivl %r9d           # Signed division of %edx:%eax by %r9d (n)

# store height / n
movl %eax, %r11d     # Store result of height / n in %r11d

# is width / n <= 0?
cmpl $0, %r10d   # Test if width / n is zero or less
jle .LFalseCondition # Jump if less than or equal to 0 (signed comparison)

# is height / n <= 0?
cmpl $0, %r11d   # Test if height / n is zero or less
jle .LFalseCondition # Jump if less than or equal to 0 (signed comparison)

# True case, place 1 in return reg
movl $1, %eax
jmp .Lreturn         # Jump to return

.LFalseCondition:
# False case, place 0 in return reg
movl $0, %eax

.Lreturn:
# Restore stack pointer (good style)
addq $8, %rsp
ret


/*
// Returns:
//   1 if an offset is necessary and 0 if an offset is not required

int determine_tile_x_offset( int width, int n, int tile_col ){
  //See how many pixels in the width cannot be split equally
  // and then see if the tile column index makes it eligible
  int remainder = width % n;
  if(tile_col < remainder){
    return 1;
  } else {
    return 0;
  }
} */

.globl determine_tile_x_offset
determine_tile_x_offset:

# Arguments: width = %rdi, n = %rsi, tile_col = %rdx

# Push callee-saved register
pushq %r12         # Save the 64-bit register

# Store tile_col in r12d
movl %edx, %r12d

# If n == 0, cannot divide by 0
cmpl $0, %esi     # Test if %esi (n) is zero
je .LnoXOffset  # If zero, jump to false condition

# Move width into eax for division
movl %edi, %eax

# Clear edx for unsigned division
xorl %edx, %edx    # Clear %edx

# Divide width by n
divl %esi          # Unsigned divide eax by n (%esi)

# Quotient will be in %eax, remainder in %edx

# Now, the remainder (width % n) is in %edx
movl %edx, %r10d   # Store remainder in r10d

# If tile_col (r12d) >= remainder, go to else
cmpl %r10d, %r12d
jge .LnoXOffset

# If tile_col < remainder, return 1
movl $1, %eax
jmp .LendOfCode

.LnoXOffset:
movl $0, %eax

.LendOfCode:
popq %r12          # Restore saved register
ret                # Return to caller


/*
// Returns:
//   1 if an offset is necessary and 0 if an offset is not required

int determine_tile_y_offset( int height, int n, int tile_row ){
  // See how many pixels in the height cannot be split equally
  // and then see if the tile row index makes it eligible
  int remainder = height % n;
  if(tile_row < remainder){
    return 1;
  } else {
    return 0;
  }
} */

.globl determine_tile_y_offset
determine_tile_y_offset:

# Arguments: height = %rdi, n = %rsi, tile_col = %rdx

# Push callee-saved register
pushq %r12         # Save the 64-bit register

# Store tile_col in r12d
movl %edx, %r12d

# If n == 0, cannot divide by 0
cmpl $0, %esi     # Test if %esi (n) is zero
je .LnoYOffset  # If zero, jump to false condition

# Move height into eax for division
movl %edi, %eax

# Clear edx for unsigned division
xorl %edx, %edx    # Clear %edx

# Divide height by n
divl %esi          # Unsigned divide eax by n (%esi)

# Quotient will be in %eax, remainder in %edx

# Now, the remainder (height % n) is in %edx
movl %edx, %r10d   # Store remainder in r10d

# If tile_col (r12d) >= remainder, go to else
cmpl %r10d, %r12d
jge .LnoYOffset

# If tile_col < remainder, return 1
movl $1, %eax
jmp .LreturnOffset

.LnoYOffset:
movl $0, %eax

.LreturnOffset:
popq %r12          # Restore saved register
ret                # Return to caller


/*
// Returns:
//   The tile width for the specific tile column index

int determine_tile_w( int width, int n, int tile_col ){
  //Returns the amount of pixels to split equally in
  //addition to whether or not there is a pixel offset.
  return width/n + determine_tile_x_offset(width, n, tile_col);
}
*/
.globl determine_tile_w
determine_tile_w:

# Stack alignment
subq $8, %rsp

# Store total sum in %r12 callee-saved register
pushq %r12

# Save tile_col in callee-saved register
pushq %r14
movl %edx, %r14d  # Move tile_col into r14d

# If n == 0, return 0 (false)
cmpl $0, %esi     # Test if n is zero
je .LnEqualsZero   # If zero, jump to false condition

# place width in eax for division
movl %edi, %eax    # Move width to %eax
cdq                # Sign-extend %eax into %edx for signed division

# width / n
idivl %esi         # Signed division of %edx:%eax by %esi (n)

# store width / n in %r12d
movl %eax, %r12d

# Call determine_tile_x_offset
movl %edi, %edi    # width
movl %esi, %esi    # n
movl %r14d, %edx   # tile_col
call determine_tile_x_offset

# Add result from function call to sum
addl %eax, %r12d

# Move total into %eax for return
movl %r12d, %eax  

# Skip to end to return sum
jmp .LreturnWidth

.LnEqualsZero:
movl $0, %eax      # If n is zero, return 0

.LreturnWidth:
# Restore callee-saved registers
popq %r14          
popq %r12          

# Stack alignment
addq $8, %rsp
ret


/*
 *
int determine_tile_h(int height, int n, int tile_row ){
  return height/n + determine_tile_y_offset(height, n, tile_row);
} 
*/
.globl determine_tile_h
determine_tile_h:

# Stack alignment
subq $8, %rsp

# Store total sum in %r12 callee-saved register
pushq %r12

# Save tile_row in callee-saved register
pushq %r14
movl %edx, %r14d  # Move tile_row into r14d

# If n == 0, return 0 (false)
cmpl $0, %esi     # Test if n is zero
je .LnEqualZero   # If zero, jump to false condition

# place height in eax for division
movl %edi, %eax    # Move height to %eax
cdq                # Sign-extend %eax into %edx for signed division

# height / n
idivl %esi         # Signed division of %edx:%eax by %esi (n)

# store height / n in %r12d
movl %eax, %r12d

# Call determine_tile_y_offset
movl %edi, %edi    # height
movl %esi, %esi    # n
movl %r14d, %edx   # tile_row
call determine_tile_y_offset

# Add result from function call to sum
addl %eax, %r12d

# Move total into %eax for return
movl %r12d, %eax  

# Skip to end to return sum
jmp .LreturnHeight

.LnEqualZero:
movl $0, %eax      # If n is zero, return 0

.LreturnHeight:
# Restore callee-saved registers
popq %r14          
popq %r12          

# Stack alignment
addq $8, %rsp
ret

/*
uint32_t blend_components( uint32_t fg, uint32_t bg, uint32_t alpha ) {
  return (alpha * fg + (255 - alpha) * bg) / 255; //computation for new color component
}
*/
.globl blend_components
blend_components:

    # Arguments: %rdi = fg, %rsi = bg, %rdx = alpha
    # alpha * fg
    movl    %edx, %eax         # Move alpha into %eax
    imull   %edi, %eax         # Multiply alpha * fg, result in %eax

    # (255 - alpha) * bg
    movl    $255, %ecx         # Load 255 into %ecx
    subl    %edx, %ecx         # Compute 255 - alpha, store in %ecx
    imull   %esi, %ecx         # Multiply (255 - alpha) * bg, result in %ecx

    # (alpha * fg + (255 - alpha) * bg)
    addl    %ecx, %eax         # Add (alpha * fg) + ((255 - alpha) * bg)

    # Divide by 255
    xorl    %edx, %edx         # Clear %edx before division
    movl    $255, %ecx         # Move 255 into %ecx for division
    divl    %ecx               # Divide by 255, quotient in %eax
    ret                        # Return result in %eax




/*
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
} */
.globl blend_colors
blend_colors:
    # Prologue: Set up stack frame and save callee-saved registers
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp            # Reserve 16 bytes on the stack for local variables

    # Save callee-saved registers
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15                # Save %r15 as well

    # Step 1: Get foreground color components (r, g, b, a)
    movl %edi, %r12d           # Store fg in %r12d for reuse
    call get_r                 # fg_r = get_r(fg)
    movl %eax, %r13d           # Store fg_r in %r13d

    call get_g                 # fg_g = get_g(fg)
    movl %eax, %r14d           # Store fg_g in %r14d

    call get_b                 # fg_b = get_b(fg)
    movl %eax, %r15d           # Store fg_b in %r15d

    call get_a                 # fg_a = get_a(fg)
    movl %eax, -4(%rbp)        # Store fg_a in memory (fg_a is used multiple times)

    # Step 2: Get background color components (r, g, b)
    movl %esi, %edi            # Move bg to %edi for get_r call
    call get_r                 # bg_r = get_r(bg)
    movl %eax, %ebx            # Store bg_r in %ebx

    call get_g                 # bg_g = get_g(bg)
    movl %eax, -8(%rbp)        # Store bg_g in memory (limited registers left)

    call get_b                 # bg_b = get_b(bg)
    movl %eax, -12(%rbp)       # Store bg_b in memory

    # Step 3: Blend the components
    # Blend red component
    movl %r13d, %edi           # fg_r
    movl %ebx, %esi            # bg_r
    movl -4(%rbp), %edx        # fg_a (alpha)
    call blend_components       # blended_r = blend_components(fg_r, bg_r, fg_a)
    movl %eax, %r13d           # Store blended_r in %r13d

    # Blend green component
    movl %r14d, %edi           # fg_g
    movl -8(%rbp), %esi        # bg_g
    movl -4(%rbp), %edx        # fg_a (alpha)
    call blend_components       # blended_g = blend_components(fg_g, bg_g, fg_a)
    movl %eax, %r14d           # Store blended_g in %r14d

    # Blend blue component
    movl %r15d, %edi           # fg_b
    movl -12(%rbp), %esi       # bg_b
    movl -4(%rbp), %edx        # fg_a (alpha)
    call blend_components       # blended_b = blend_components(fg_b, bg_b, fg_a)
    movl %eax, %r15d           # Store blended_b in %r15d

    # Step 4: Create the new pixel using make_pixel
    movl %r13d, %edi           # blended_r as 1st argument
    movl %r14d, %esi           # blended_g as 2nd argument
    movl %r15d, %edx           # blended_b as 3rd argument
    movl $255, %ecx            # Alpha = 255 (fully opaque)
    call make_pixel            # Call make_pixel(blended_r, blended_g, blended_b, 255)

    # Step 5: Restore callee-saved registers and stack frame
    popq %r15                  # Restore %r15
    popq %r14                  # Restore %r14
    popq %r13                  # Restore %r13
    popq %r12                  # Restore %r12
    popq %rbx                  # Restore %rbx

    addq $16, %rsp             # Restore stack pointer
    popq %rbp                  # Restore base pointer
    ret


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


  # Move output image pointer into %rdi
  # Move input image pointer into %rsi
  # Move current row value into %edx
  # Move current column value into %ecx
  # %r8d already holds the n value from above

    .globl copy_tile
copy_tile:
  subq $24, %rsp
  pushq %rbp
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15
  pushq %rbx

  movq %rsp, %rbp

  movq %rdi, %r12
  movq %rsi, %r13
  movl %edx, %r14d

  movl IMAGE_WIDTH_OFFSET(%r13), %edi
  movl %r8d, %esi
  movl %r14d, %edx

  call determine_tile_w

  movl %eax, -8(%rbp)

  movl IMAGE_HEIGHT_OFFSET(%r13), %edi
  # movq %r8d, %esi
  movl %ecx, %edx

  call determine_tile_h

  movq %r12, %rdi
  movq %r13, %rsi
  movl %r14d, %edx  

  movl %eax, -4(%rbp)

start_left_update:  
  movl $0, -12(%rbp)
  movl $0, %ebx
  movq %rdi, %r12
  movq %rsi, %r13
  movl %edx, %r14d
  movl %ecx, %r15d

  # Move output image pointer into %rdi
  # Move input image pointer into %rsi
  # Move current row value into %edx
  # Move current column value into %ecx
  # %r8d already holds the n value from above

update_left_loop:
  cmpl %r15d, %ebx
  jae start_top_update

  movl IMAGE_WIDTH_OFFSET(%r13), %edi
  movl %r8d, %esi
  movl %ebx, %edx

  call determine_tile_w

  addl %eax, -12(%rbp)

  incl %ebx
  jmp update_left_loop




start_top_update:
  movl $0, -16(%rbp)
  movl $0, %ebx

update_top_loop:
  cmpl %r14d, %ebx
  jae start_tile_fill

  movl IMAGE_HEIGHT_OFFSET(%r13), %edi
  movl %r8d, %esi
  movl %ebx, %edx

  call determine_tile_h

  addl %eax, -16(%rbp)

  incl %ebx
  jmp update_top_loop

start_tile_fill:
  movq %r12, %rdi
  movq %r13, %rsi

  # Initialize h counter
  movl $0, %edx

copy_outer_h_loop:
  # Compare h counter with tile height
  movl -4(%rbp), %r10d
  cmpl %r10d, %edx  

  # If r >= n, end the outer loop
  jge end_copy

  # Initialize w counter
  movl $0, %ecx

copy_inner_w_loop:
  # Compare w counter with tile width
  movl -8(%rbp), %r10d
  cmpl %r10d, %ecx  
  jge copy_w_loop_end

  movl %ecx, %r10d
  imull %r8d, %r10d

  movl IMAGE_WIDTH_OFFSET(%rsi), %r11d

  cmpl %r11d, %r10d
  jge copy_advance

  movl %edx, %r10d
  imull %r8d, %r10d

  movl IMAGE_HEIGHT_OFFSET(%rsi), %r11d

  cmpl %r11d, %r10d
  jge copy_advance

  movq %rdi, %r12
  movq %rsi, %r13
  movl %edx, %r14d
  movl %ecx, %r15d

  movl %ecx, %r10d
  movl %edx, %r11d

  imull %r8d, %r10d
  imull %r8d, %r11d

  movl %r10d, %esi
  movl %r11d, %edx

  movq %r13, %rdi

  call get_pixel

  # Move the pixel into the pixel parameter for set_pixel
  movl %eax, %ecx

  movq %r12, %rdi

  movl -12(%rbp), %esi
  addl %r15d, %esi

  movl -16(%rbp), %edx
  addl %r14d, %edx

  # Set the pixel at the computed position
  call set_pixel

  movq %r12, %rdi
  movq %r13, %rsi
  movl %r14d, %edx
  movl %r15d, %ecx

copy_advance:
  # Increment c counter and loop again
  incl %ecx

  # Repeat the loop
  jmp copy_inner_w_loop

# Describes what to do once the inner loop ends
copy_w_loop_end:
    # Increae r pixel value by 1
    incl %edx

    # Restart check for outer loop
    jmp copy_outer_h_loop

end_copy:
  popq %rbx
  popq %r15
  popq %r14
  popq %r13
  popq %r12
  popq %rbp
  addq $24, %rsp
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
  subq $8, %rsp
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15

	cmpl $1, %esi
  jl returnFalse

  movq %rdi, %r12
  movl %esi, %r13d
  movq %rdx, %r14

  movl IMAGE_WIDTH_OFFSET(%r12), %edi
  movl IMAGE_HEIGHT_OFFSET(%r12), %esi
  movl %esi, %edx

  call all_tiles_nonempty

  cmpl $0, %eax
  je returnFalse

  # Might not need these two
  movq %r14, %rdi
  movq %r12, %rsi

  movl %r13d, %r8d

  # Initialize row counter at 0
  movl $0, %r13d 


tile_outer_r_loop:
  # Compare r with the tiling factor
  cmpl %r8d, %r13d  

  # If r >= n, end the outer loop
  jae returnTrue

  # Initialize column counter at 0
  movl $0, %r15d

tile_inner_c_loop:
  # Compare c with tiling factor 
  cmpl %r8d, %r15d

  # If c >= n, end the inner loop
  jae tile_c_loop_end

  # Move output image pointer into %rdi
  movq %r14, %rdi

  # Move input image pointer into %rsi
  movq %r12, %rsi

  # Move current row value into %edx
  movl %r13d, %edx

  # Move current column value into %ecx
  movl %r15d, %ecx

  # Call copy_tile
  # %r8d already holds the n value from above

  # call copy_tile TODO!

  # Restore the original y value into %edx 
  ; movl %r14d, %edx

  # Restore input image pointer into %rdi
  ; movq %r12, %rdi  

  # Increment c counter and loop again
  incl %r15d

  # Repeat the loop
  jmp tile_inner_c_loop

# Describes what to do once the inner loop ends
tile_c_loop_end:
    # Increae r pixel value by 1
    incl %r13d

    # Restart check for outer loop
    jmp tile_outer_r_loop

returnFalse:
  movl $0, %eax
  jmp endFunction

returnTrue:
  movl $1, %eax

endFunction:
  popq %r15
  popq %r14
  popq %r13
  popq %r12
  addq $8, %rsp
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


/* Offsets Width = 0, Height = 4, Data = 8
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

 /*
 * Register use:
 *   %r12 - pointer to base_img
 *   %r13 - pointer to overlay_img
 *   %r14 - pointer to output_img
 *   %r15d - row counter (r)
 *   %ebx - temporary for blended_pixel result
 *   %r8d - temporary for width comparison
 *   %r9d - temporary for height comparison
 *
 * Memory use:
 *   -16(%rbp) - column counter (c)
 *   -8(%rbp)  - foreground_pixel
 */

	.globl imgproc_composite
imgproc_composite:
    # Function prologue

    pushq %rbp                   # Save %rbp for stack operations
    movq %rsp, %rbp              # Set new base pointer for stack operations
    subq $8, %rsp                 # Reserve space for c (8 bytes)

    pushq %rbx                   # Save callee-saved register
    pushq %r12                   # Save callee-saved register
    pushq %r13                   # Save callee-saved register
    pushq %r14                   # Save callee-saved register
    pushq %r15                   # Save callee-saved register
    
    # Save image pointers in callee-saved registers
    movq %rdi, %r12              # Save base_img pointer
    movq %rsi, %r13              # Save overlay_img pointer
    movq %rdx, %r14              # Save output_img pointer

    movl $0, %eax

    # Check if base_img->width != overlay_img->width
    movl IMAGE_WIDTH_OFFSET(%r12), %r8d       # Load base_img->width
    movl IMAGE_WIDTH_OFFSET(%r13), %r9d       # Load overlay_img->width
    cmpl %r8d, %r9d              # Compare base and overlay widths
    jne .LreturnComposite        # If not equal, return 0

    # Check if base_img->height != overlay_img->height
    movl IMAGE_HEIGHT_OFFSET(%r12), %r8d      # Load base_img->height
    movl IMAGE_HEIGHT_OFFSET(%r13), %r9d      # Load overlay_img->height
    cmpl %r8d, %r9d              # Compare base and overlay heights
    jne .LreturnComposite         # If not equal, return 0

    # Set the success return value (return 1)
    movl $1, %eax

    # Outer loop: Iterate over rows (r)
    xorl %r15d, %r15d            # r = 0 (initialize row counter)

.LouterLoop:
    cmpl IMAGE_HEIGHT_OFFSET(%r14), %r15d     # Compare r < output_img->height
    jge .LreturnComposite          # If r >= height, return

    # Inner loop: Iterate over columns (c)
    movl $0, -8(%rbp)             # c = 0 (initialize column counter on stack)

.LinnerLoop:
    # Load c directly from the stack for foreground pixel
    movl -8(%rbp), %r8d                      # Load column counter c from stack
    cmpl IMAGE_WIDTH_OFFSET(%r14), %r8d       # Compare c < output_img->width
    jge .LnextRow                             # If c >= width, go to next row

    # Load foreground_pixel = get_pixel(overlay_img, c, r)
    movq %r13, %rdi                # 1st arg = pointer to overlay_img
    movl -8(%rbp), %esi           # 2nd arg = column index (c)
    movl %r15d, %edx               # 3rd arg = row index (r)
    call get_pixel                 # Call get_pixel(overlay_img, c, r)
    movl %eax, %ebx                # Store result (foreground_pixel) in %ebx 

    # Load background_pixel = get_pixel(base_img, c, r)
    movq %r12, %rdi                # 1st arg = pointer to base_img
    movl -8(%rbp), %esi           # 2nd arg = column index (c) directly from stack
    movl %r15d, %edx               # 3rd arg = row index (r)
    call get_pixel                 # Call get_pixel(base_img, c, r)
    
    # Blend the pixels using blend_colors
    movl %ebx, %edi                # 1st arg = foreground_pixel
    movl %eax, %esi                # 2nd arg = background_pixel
    call blend_colors              # Call blend_colors(bg_pixel, fg_pixel)
    movl %eax, %ebx                # Store blended result in %ebx

    # Store blended_pixel = set_pixel(output_img, c, r, blended_pixel)
    movq %r14, %rdi                # 1st arg = pointer to output_img
    movl -8(%rbp), %esi            # 2nd arg = column index (c) directly from stack
    movl %r15d, %edx               # 3rd arg = row index (r)
    movl %ebx, %ecx                # 4th arg = blended_pixel
    call set_pixel                 # Call set_pixel(output_img, c, r, blended_pixel)

    # Increment column counter (update value on stack)
    incl -8(%rbp)                 # c++
    jmp .LinnerLoop                # Repeat inner loop

.LnextRow:
    # Increment row counter
    incl %r15d                      # r++
    jmp .LouterLoop                 # Repeat outer loop

.LreturnComposite:
    # Restore stack and registers
    popq %r15                      # Restore callee-saved registers
    popq %r14
    popq %r13
    popq %r12
    popq %rbx                      

    addq $8, %rsp                  # Free space used for c on stack
    popq %rbp                      # Restore %rbp

    ret






/*
vim:ft=gas:
*/
