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

	# Test if %edx (n) is zero
  cmpl $0, %edx     

	# If zero, jump to false condition
  je .LFalseCondition  

  # Save n to the stack (since %edx will be overwritten by cdq)
	# Save the divisor n in %r9d
  movl %edx, %r9d      

  # place width in eax for division

	# Move width to %eax (32-bit register)
  movl %edi, %eax      

	# Sign-extend %eax into %edx for signed division
  cdq                  

  # width / n

	# Signed division of %edx:%eax by %r9d (n)
  idivl %r9d          

  # store width / n

	# Store result of width / n in %r10d
  movl %eax, %r10d     

  # place height in eax for division

	# Move height to %eax (32-bit register)
  movl %esi, %eax   

	# Sign-extend %eax into %edx for signed division
  cdq                  

  # height / n

	# Signed division of %edx:%eax by %r9d (n)
  idivl %r9d           

  # store height / n

	# Store result of height / n in %r11d
  movl %eax, %r11d     

  # Is width / n <= 0?

	# Test if width / n is zero or less
  cmpl $0, %r10d   

	# Jump if less than or equal to 0 (signed comparison)
  jle .LFalseCondition

  # Is height / n <= 0?

	# Test if height / n is zero or less
  cmpl $0, %r11d   

	# Jump if less than or equal to 0 (signed comparison)
  jle .LFalseCondition 

  # True case, place 1 in return reg
  movl $1, %eax

	# Jump to return
  jmp .Lreturn         

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


/* Returns 1 if an offset is necessary and 0 if an offset is not required

	int determine_tile_x_offset( int width, int n, int tile_col )

	Arguments:
	- rdi: width (int)
	- rdx: n (int)
	- rsi: tile_col (int)

	Return:
	- eax: 1 if an offset is necessary, 0 if an offset is not necessary
*/

  .globl determine_tile_x_offset
determine_tile_x_offset:

  # Arguments: width = %rdi, n = %rsi, tile_col = %rdx

  # Push callee-saved registers
  pushq %r12         

  # Store tile_col in r12d
  movl %edx, %r12d

  # If n == 0, cannot divide by 0

	# Test if %esi (n) is zero
  cmpl $0, %esi     

	# If zero, jump to false condition
  je .LnoXOffset  

  # Move width into eax for division
  movl %edi, %eax

  # Clear edx for unsigned division

	# Clear %edx
  xorl %edx, %edx    

  # Divide width by n

	# Unsigned divide eax by n (%esi)
  divl %esi          

  # Quotient will be in %eax, remainder in %edx. Now, the remainder (width % n) is in %edx

	# Store remainder in r10d
  movl %edx, %r10d   

  # If tile_col (r12d) >= remainder, go to else
  cmpl %r10d, %r12d
  jge .LnoXOffset

  # If tile_col < remainder, return 1
  movl $1, %eax
  jmp .LendOfCode

.LnoXOffset:
  movl $0, %eax

.LendOfCode:
	# Restore saved register
  popq %r12          

	# Return to caller
  ret                


/* Returns 1 if an offset is necessary and 0 if an offset is not required

	int determine_tile_y_offset( int width, int n, int tile_row )

	Arguments:
	- rdi: width (int)
	- rdx: n (int)
	- rsi: tile_row (int)

	Return:
	- eax: 1 if an offset is necessary, 0 if an offset is not necessary
*/

  .globl determine_tile_y_offset
determine_tile_y_offset:
  # Arguments: height = %rdi, n = %rsi, tile_col = %rdx

  # Push callee-saved register

	# Save the 64-bit register
  pushq %r12         

  # Store tile_col in r12d
  movl %edx, %r12d

  # If n == 0, cannot divide by 0

	# Test if %esi (n) is zero
  cmpl $0, %esi    

	# If zero, jump to false condition
  je .LnoYOffset  

  # Move height into eax for division
  movl %edi, %eax

  # Clear edx for unsigned division

	# Clear %edx
  xorl %edx, %edx    

  # Divide height by n

	# Unsigned divide eax by n (%esi)
  divl %esi          

  # Quotient will be in %eax, remainder in %edx. Now, the remainder (height % n) is in %edx

	# Store remainder in r10d
  movl %edx, %r10d   

  # If tile_col (r12d) >= remainder, go to else
  cmpl %r10d, %r12d
  jge .LnoYOffset

  # If tile_col < remainder, return 1
  movl $1, %eax
  jmp .LreturnOffset

.LnoYOffset:
  movl $0, %eax

.LreturnOffset:
	# Restore saved register
  popq %r12 

	# Return to caller         
  ret                


/* Returns the amount of pixels to split equally in addition to whether or not there is a pixel offset.

	int determine_tile_w( int width, int n, int tile_col )	

	Arguments:
	- rdi: width (int)
	- rdx: n (int)
	- rsi: tile_col (int)

	Return:
	- eax: Width of specified tile
*/

  .globl determine_tile_w
determine_tile_w:

  # Stack alignment
  subq $8, %rsp

  # Store total sum in %r12 callee-saved register
  pushq %r12

  # Save tile_col in callee-saved register
  pushq %r14

	# Move tile_col into r14d
  movl %edx, %r14d  

  # If n == 0, return 0 (false)

	# Test if n is zero
  cmpl $0, %esi    

	# If zero, jump to false condition 
  je .LnEqualsZero   

  # place width in eax for division

	# Move width to %eax
  movl %edi, %eax    

	# Sign-extend %eax into %edx for signed division
  cdq                

  # width / n

	# Signed division of %edx:%eax by %esi (n)
  idivl %esi         

  # store width / n in %r12d
  movl %eax, %r12d

  # Call determine_tile_x_offset

	# width
  movl %edi, %edi  

	# n  
  movl %esi, %esi  

	# tile_col  
  movl %r14d, %edx  

  call determine_tile_x_offset

  # Add result from function call to sum
  addl %eax, %r12d

  # Move total into %eax for return
  movl %r12d, %eax  

  # Skip to end to return sum
  jmp .LreturnWidth

  .LnEqualsZero:
	# If n is zero, return 0
  movl $0, %eax      

  .LreturnWidth:
  # Restore callee-saved registers
  popq %r14          
  popq %r12          

  # Stack alignment
  addq $8, %rsp
  ret

/* Returns the amount of pixels to split equally in addition to whether or not there is a pixel offset.

	int determine_tile_h( int height, int n, int tile_row )	

	Arguments:
	- rdi: height (int)
	- rdx: n (int)
	- rsi: tile_row (int)

	Return:
	- eax: Height of specified tile
*/

  .globl determine_tile_h
determine_tile_h:

  # Stack alignment
  subq $8, %rsp

  # Store total sum in %r12 callee-saved register
  pushq %r12

  # Save tile_row in callee-saved register
  pushq %r14

	# Move tile_row into r14d
  movl %edx, %r14d  

  # If n == 0, return 0 (false)

	# Test if n is zero
  cmpl $0, %esi   

	# If zero, jump to false condition  
  je .LnEqualZero   

  # place height in eax for division

	# Move height to %eax
  movl %edi, %eax  

	# Sign-extend %eax into %edx for signed division
  cdq                

  # height / n

	# Signed division of %edx:%eax by %esi (n)
  idivl %esi         

  # store height / n in %r12d
  movl %eax, %r12d

  # Call determine_tile_y_offset

	# height
  movl %edi, %edi    

	# n
  movl %esi, %esi    

	# tile_row
  movl %r14d, %edx   
  call determine_tile_y_offset

  # Add result from function call to sum
  addl %eax, %r12d

  # Move total into %eax for return
  movl %r12d, %eax  

  # Skip to end to return sum
  jmp .LreturnHeight

.LnEqualZero:
	# If n is zero, return 0
  movl $0, %eax      

.LreturnHeight:
  # Restore callee-saved registers
  popq %r14          
  popq %r12          

  # Stack alignment
  addq $8, %rsp
  ret

/*
 * void copy_tile( struct Image *out_img, struct Image *img, int tile_row, int tile_col, int n )
 * 
 * Takes in an input image pointer and places it into the output image
 * at a certain tile column row and value with tiling factor n.

 * Parameters:
 *   %rdi - output image pointer
 *   %rsi - input image pointer
 *   %edx - current row value of tile
 *   %ecx - current column value of tile
 *   %r8d - tiling factor (n)
 *
 * Returns:
 *   There are no returns for this function
 */


  .globl copy_tile
copy_tile:
	# Align stack pointer and allocate space for stack variables
  subq $24, %rsp

	# Push all callee-saved registers used through the function
  pushq %rbp
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15
  pushq %rbx

  # Move stack pointer into %rbp
  movq %rsp, %rbp

  # Move output image pointer into %r12
  movq %rdi, %r12

  # Move input image pointer into %r13
  movq %rsi, %r13

  # Move tile row value into %r14d
  movl %edx, %r14d

  # Move image width into %edi
  movl IMAGE_WIDTH_OFFSET(%r13), %edi

  # Move n tiling factor into %esi 
  movl %r8d, %esi

  # Move tile column value into %edx
  movl %ecx, %edx

  # Determine the tile width
  call determine_tile_w

  # Move the tile width into 16(%rbp)
  movl %eax, 16(%rbp)

  # Move the image height into %rdi
  movl IMAGE_HEIGHT_OFFSET(%r13), %edi

  # Move the tile row value into %edx
  movl %r14d, %edx

  # Determine tile width
  call determine_tile_h

  # Restore output image pointer into %rdi
  movq %r12, %rdi

  # Restore input image pointer into %rsi
  movq %r13, %rsi

  # Restore tile row value into %edx
  movl %r14d, %edx  

  # Move the tile height into -12(%rbp)
  movl %eax, 12(%rbp)

start_left_update:  
  # Initialize left most pixel into 20(%rbp) with value 0
  movl $0, 20(%rbp)

  # Set loop counter to 0
  movl $0, %ebx

  # Move output image pointer to %r12
  movq %rdi, %r12

  # Move input image pointer to %r13
  movq %rsi, %r13

  # Move tile row value into %r14d
  movl %edx, %r14d

  # Move tile column value into %r15d
  movl %ecx, %r15d

update_left_loop:
  # Compare loop counter with the tile column value
  cmpl %r15d, %ebx

  # If greater than or equal to, move to updating top value
  jge start_top_update

  # Move image width into %edi
  movl IMAGE_WIDTH_OFFSET(%r13), %edi

  # Move tiling factor n into %esi
  movl %r8d, %esi

  # Move individual col value into %edx
  movl %ebx, %edx

  # Determine tile width
  call determine_tile_w

  # Add tile width to leftmost pixel offset
  addl %eax, 20(%rbp)
  
  # Increase loop counter and restart loop
  incl %ebx
  jmp update_left_loop

start_top_update:
  # Initialize top most y pixel into %r9d
  movl $0, %r9d

  # Intiailize loop counter
  movl $0, %ebx

update_top_loop:
  # Compare tile row value with loop counter
  cmpl %r14d, %ebx

  # If greater than or equal to, start filling tile
  jge start_tile_fill

  # Move image height into %edi
  movl IMAGE_HEIGHT_OFFSET(%r13), %edi

  # Move n tiling factor into %esi
  movl %r8d, %esi

  # Move loop counter into %edx
  movl %ebx, %edx

  # Determine the tile h
  call determine_tile_h

  # Increment the top most pixel by previous heights
  addl %eax, %r9d

  # Increment loop counter and repeat loop
  incl %ebx
  jmp update_top_loop

start_tile_fill:
  # Move output image pointer into %rdi
  movq %r12, %rdi

  # Move input image pointer into %rsi
  movq %r13, %rsi

  # Initialize h counter
  movl $0, %edx

copy_outer_h_loop:
  # Move tile height into storage variable
  movl 12(%rbp), %r10d

  # Compare h counter with tile height
  cmpl %r10d, %edx  

  # If r >= n, end the outer loop
  jge end_copy

  # Initialize w counter
  movl $0, %ecx

copy_inner_w_loop:
  # Move tile width into storage variable
  movl 16(%rbp), %r10d

  # Compare w counter with tile width
  cmpl %r10d, %ecx  
  jge copy_w_loop_end

  # Move w counter into %r10d 
  movl %ecx, %r10d

  # Multiply w counter by n
  imull %r8d, %r10d

	# Move the image width into %r11d
  movl IMAGE_WIDTH_OFFSET(%rsi), %r11d

	# Compare n*w counter with the image width
  cmpl %r11d, %r10d

	# If it is larger, repeat the loop
  jge copy_advance

	# Move h counter into %r10d
  movl %edx, %r10d

	# Multiply h counter by n
  imull %r8d, %r10d

	# Move the image height into %r11d
  movl IMAGE_HEIGHT_OFFSET(%rsi), %r11d

	# Compare n*h counter with the image height
  cmpl %r11d, %r10d

	# If it is larger, repeat the loop
  jge copy_advance

	# Store the output image pointer into %r12
  movq %rdi, %r12

	# Store the input image pointer into %r13
  movq %rsi, %r13

	# Move h counter into %r14d
  movl %edx, %r14d

	# Move w counter into %r15d
  movl %ecx, %r15d

	# Move w counter into %r10d
  movl %ecx, %r10d

	# Move h counter into %r11d
  movl %edx, %r11d

	# Multiply both w and h counters by n
	# Pixel x and y to get from input Image
  imull %r8d, %r10d
  imull %r8d, %r11d

	# Move n*w counter into %esi
  movl %r10d, %esi

	# Move n*h counter into %edx
  movl %r11d, %edx

	# Move input image pointer into %rdi
  movq %r13, %rdi

	# Call get pixel
  call get_pixel

	# Prepare call for set pixel

  # Move the pixel into the pixel parameter for set_pixel
  movl %eax, %ecx

	# Move output image pointer in %r12 into %rdi
  movq %r12, %rdi

	# Move left most pixel into %esi
  movl 20(%rbp), %esi

	# Increment esi by current w counter
  addl %r15d, %esi

	# Move top most pixel into %edx
  movl %r9d, %edx

	# Increment %edx by current h counter
  addl %r14d, %edx

  # Set the pixel at the computed position
  call set_pixel

	# Reset the output image pointer to %rdi
  movq %r12, %rdi

	# Reset the input image pointer to %rsi
  movq %r13, %rsi

	# Reset the current h counter to %edx
  movl %r14d, %edx

	# Reset the current w counter to %ecx
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
	# Pop all callee-saved registers
  popq %rbx
  popq %r15
  popq %r14
  popq %r13
  popq %r12
  popq %rbp

	# Release stack variables and realign stack
  addq $24, %rsp
  ret

/*
 * uint32_t blend_components( uint32_t fg, uint32_t bg, uint32_t alpha )
 * 
 * Helper function that computes the new color
 * component for foreground and background pixels.

 * Parameters:
 *   %rdi - uint32_t pixel value for foreground pixel
 *   %rsi - uint32_t pixel value for background pixel
 *   %rdx - uint32_t value for alpha
 *
 * Returns:
 *   %eax - new computtion of new color component of (alpha * fg + (255 - alpha) * bg) / 255
 */

	.globl blend_components
blend_components:
  # Arguments: %rdi = fg, %rsi = bg, %rdx = alpha
  # alpha * fg

	# Move alpha into %eax
  movl    %edx, %eax         

	# Multiply alpha * fg, result in %eax
  imull   %edi, %eax         

  # (255 - alpha) * bg

	# Load 255 into %ecx
  movl    $255, %ecx     

	# Compute 255 - alpha, store in %ecx    
  subl    %edx, %ecx   

	# Multiply (255 - alpha) * bg, result in %ecx      
  imull   %esi, %ecx         

  # Add (alpha * fg) + ((255 - alpha) * bg)
  addl    %ecx, %eax         

  # Divide by 255

	# Clear %edx before division
  xorl    %edx, %edx         

	# Move 255 into %ecx for division
  movl    $255, %ecx        

	# Divide by 255, quotient in %eax 
  divl    %ecx      

	# Return result in %eax         
  ret                       


/*
 * uint32_t blend_colors( uint32_t fg, uint32_t bg )
 * 
 * Blends the foreground and background pixel to produce a
 * new blended pixel.

 * Parameters:
 *   %rdi - uint32_t pixel value for foreground pixel
 *   %rsi - uint32_t pixel value for background pixel
 *
 * Returns:
 *   %eax - new uint32_t pixel value representing blended pixel
 */

	.globl blend_colors
blend_colors:
  # Prologue: Set up stack frame and save callee-saved registers
  pushq %rbp
  movq %rsp, %rbp

	# Reserve 16 bytes on the stack for local variables
  subq $16, %rsp            

  # Save callee-saved registers
  pushq %rbx
  pushq %r12
  pushq %r13
  pushq %r14

	# Save %r15 as well
  pushq %r15                

  # Step 1: Get foreground color components (r, g, b, a)

	# Store fg in %r12d for reuse
  movl %edi, %r12d           

	# fg_r = get_r(fg)
  call get_r  

	# Store fg_r in %r13d               
  movl %eax, %r13d           

	# fg_g = get_g(fg)
  call get_g   

	# Store fg_g in %r14d
  movl %eax, %r14d  

	# fg_b = get_b(fg)     
  call get_b     

	# Store fg_b in %r15d            
  movl %eax, %r15d           

	# fg_a = get_a(fg)
  call get_a 

	# Store fg_a in memory (fg_a is used multiple times)               
  movl %eax, -4(%rbp)        

  # Step 2: Get background color components (r, g, b)

	# Move bg to %edi for get_r call
  movl %esi, %edi           

	# bg_r = get_r(bg)
  call get_r        

	# Store bg_r in %ebx         
  movl %eax, %ebx            

	# bg_g = get_g(bg)
  call get_g     

	# Store bg_g in memory (limited registers left)            
  movl %eax, -8(%rbp)        

	# bg_b = get_b(bg)
  call get_b        

	# Store bg_b in memory         
  movl %eax, -12(%rbp)       

  # Step 3: Blend the components
  # Blend red component

	# fg_r
  movl %r13d, %edi  

	# bg_r         
  movl %ebx, %esi   

	# fg_a (alpha)         
  movl -4(%rbp), %edx  

	# blended_r = blend_components(fg_r, bg_r, fg_a)      
  call blend_components 

	# Store blended_r in %r13d      
  movl %eax, %r13d           

  # Blend green component

	# fg_g
  movl %r14d, %edi 

	# bg_g          
  movl -8(%rbp), %esi

	# fg_a (alpha)        
  movl -4(%rbp), %edx 

	# blended_g = blend_components(fg_g, bg_g, fg_a)       
  call blend_components 

	# Store blended_g in %r14d      
  movl %eax, %r14d           

  # Blend blue component

	# fg_b
  movl %r15d, %edi 

	# bg_b         
  movl -12(%rbp), %esi  

	# fg_a (alpha)
  movl -4(%rbp), %edx

	# blended_b = blend_components(fg_b, bg_b, fg_a)        
  call blend_components

	# Store blended_b in %r15d      
  movl %eax, %r15d           

  # Step 4: Create the new pixel using make_pixel

	# blended_r as 1st argument
  movl %r13d, %edi   

	# blended_g as 2nd argument        
  movl %r14d, %esi     

	# blended_b as 3rd argument      
  movl %r15d, %edx   

	# Alpha = 255 (fully opaque)        
  movl $255, %ecx    

	# Call make_pixel(blended_r, blended_g, blended_b, 255)        
  call make_pixel  

  # Step 5: Restore callee-saved registers and stack frame
  popq %r15                  
  popq %r14                  
  popq %r13               
  popq %r12           
  popq %rbx       

	# Restore stack pointer
  addq $16, %rsp     

	# Restore base pointer        
  popq %rbp                  
  ret


/*
 * Makes a pixel with the r, g, b, and a attributes
 *
 * uint32_t make_pixel(uint32_t r, uint32_t g, uint32_t b, uint32_t a) 
 * return (r << 24) | (g << 16) | (b << 8) | a;
 * 
 * Register Use:
 *   %edi - r
 *   %esi - g
 *   %edx - b
 *   %ecx - a
 *
 * Returns:
 *   %eax - Used as temp in computation, also pixel value to return
 */
  .globl make_pixel
make_pixel:
	# place r into temp register
  movl %edi, %eax  

	# left shift r by 24
  shll $24, %eax  

	# left shift g by 16  
  shll $16, %esi    

	 # left shift b by 8
  shll $8, %edx    

	# store (r << 24) | (g << 16) in %eax
  or %esi, %eax   

	# store (r << 24) | (g << 16) | (b << 8) in %eax 
  or %edx, %eax  

	# or with a to get final result  
  or %ecx, %eax    

  ret


/*
 * Gets an image from a certain Image at a certain x and y
 * 
 * uint32_t get_pixel(struct Image *img, int32_t x, int32_t y);
 * 
 * Register Use:
 *   %rdi - pointer to struct Image
 *   %esi - x coordinate (int32_t)
 *   %edx - y coordinate (int32_t)
 *   %r10 - Used to compute the pixel index
 *   %r12, %r14 - callee-saved registers (need to be pushed and popped)
 * Returns:
 *   %eax - pixel value to return
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
 * Return red value for a uint32_t pixel
 * uint32_t get_r(uint32_t pixel) 
 *
 * Register Use:
 *  %rdi - uint32_t pixel parameter

 * Returns:
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
 * Return green value for a uint32_t pixel
 * uint32_t get_g(uint32_t pixel) 
 *
 * Register Use:
 *  %rdi - uint32_t pixel parameter
 *
 * Returns:
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
 * Return blue value for a uint32_t pixel
 * uint32_t get_b(uint32_t pixel) 
 *
 * Register Use:
 *  %rdi - uint32_t pixel parameter
 * 
 * Returns:
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
 * Return alpha value for a uint32_t pixel
 * uint32_t get_a(uint32_t pixel)  *
 * Register Use:
 *  %rdi - uint32_t pixel parameter
 *
 * Returns:
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
 *
 * Transforms a pixel into its grayscale form
 *
 * Parameters:
 *   %rdi - uint32_t pixel parameter
 *
 * Returns (in %eax):
 *   Grayscale pixel factor using grayscale equation:
 *   ((79 * get_r(pixel)) + (128 * get_g(pixel)) + (49 * get_b(pixel))) / 256
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

	# Realign stack to have odd multiple 8
  subq $8, %rsp

	# Push callee-saved registers
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15

	# Compare the tiling factor with 1
  cmpl $1, %esi

	# Return 0 if tiling factor is less than 1
  jl returnFalse

	# Save pointer to original struct Image in %r12
  movq %rdi, %r12

	# Save tiling factor in %r13d
  movl %esi, %r13d

	# Save pointer to output struct Image in %r14
  movq %rdx, %r14

	# Move the input image width into %edi
  movl IMAGE_WIDTH_OFFSET(%r12), %edi

	# Move the input image height into %esi
  movl IMAGE_HEIGHT_OFFSET(%r12), %esi

	# Move the tile factor into %edx for all_tiles_nonempty
  movl %esi, %edx

	# Call all tiles nonempty
  call all_tiles_nonempty

	# See if all tiles nonempty returns 0
  cmpl $0, %eax

	# If nonempty is equal to 0, return False
  je returnFalse

	# Move the tiling factor into %r8d for copy_tile
  movl %r13d, %r8d

  # Initialize row counter at 0
  movl $0, %r13d 

tile_outer_r_loop:
  # Compare r with the tiling factor
  cmpl %r8d, %r13d  

  # If r >= n, end the outer loop
  jge returnTrue

  # Initialize column counter at 0
  movl $0, %r15d

tile_inner_c_loop:
  # Compare c with tiling factor 
  cmpl %r8d, %r15d

  # If c >= n, end the inner loop
  jge tile_c_loop_end

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

  call copy_tile  

	# Reset values with %rdi 
  movq %rdi, %r14

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
	# Move 0 into return register %eax
  movl $0, %eax

	# Enter the epilogue
  jmp endFunction

returnTrue:
	# Move 1 into return register %eax and move onto %eax
  movl $1, %eax

endFunction:
	# Pop all callee-saved registers
  popq %r15
  popq %r14
  popq %r13
  popq %r12

	# Reset stack realignment from above
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
 *   16(%rbp) - column counter (c)
 *   -8(%rbp)  - foreground_pixel
 */

  .globl imgproc_composite
imgproc_composite:
  # Function prologue

	# Save %rbp for stack operations
  pushq %rbp    

	# Set new base pointer for stack operations               
  movq %rsp, %rbp  

	# Reserve space for c (8 bytes)            
  subq $8, %rsp                 

	# Save callee-saved registers used in function 
  pushq %rbx                   
  pushq %r12                   
  pushq %r13                   
  pushq %r14               
  pushq %r15               
  
  # Save image pointers in callee-saved registers

	# Save base_img pointer
  movq %rdi, %r12   

	# Save overlay_img pointer           
  movq %rsi, %r13 

	# Save output_img pointer             
  movq %rdx, %r14            

  movl $0, %eax

  # Check if base_img->width != overlay_img->width

	# Load base_img->width
  movl IMAGE_WIDTH_OFFSET(%r12), %r8d 

	# Load overlay_img->width      
  movl IMAGE_WIDTH_OFFSET(%r13), %r9d   

	# Compare base and overlay widths    
  cmpl %r8d, %r9d    

	# If not equal, return 0          
  jne .LreturnComposite        

  # Check if base_img->height != overlay_img->height

	# Load base_img->height
  movl IMAGE_HEIGHT_OFFSET(%r12), %r8d  

	# Load overlay_img->height    
  movl IMAGE_HEIGHT_OFFSET(%r13), %r9d   

	# Compare base and overlay heights   
  cmpl %r8d, %r9d 

	# If not equal, return 0            
  jne .LreturnComposite         

  # Set the success return value (return 1)
  movl $1, %eax

  # Outer loop: Iterate over rows (r)

	# r = 0 (initialize row counter)
  xorl %r15d, %r15d            

.LouterLoop:
	# Compare r < output_img->height
  cmpl IMAGE_HEIGHT_OFFSET(%r14), %r15d   

	# If r >= height, return  
  jge .LreturnComposite          

  # Inner loop: Iterate over columns (c)

	# c = 0 (initialize column counter on stack)
  movl $0, -8(%rbp)      

.LinnerLoop:
  # Load c directly from the stack for foreground pixel

	# Load column counter c from stack
  movl -8(%rbp), %r8d          

	# Compare c < output_img->width            
  cmpl IMAGE_WIDTH_OFFSET(%r14), %r8d 

	# If c >= width, go to next row     
  jge .LnextRow                             

  # Load foreground_pixel = get_pixel(overlay_img, c, r)

	# 1st arg = pointer to overlay_img
  movq %r13, %rdi 

	# 2nd arg = column index (c)               
  movl -8(%rbp), %esi   

	# 3rd arg = row index (r)        
  movl %r15d, %edx 

	# Call get_pixel(overlay_img, c, r)             
  call get_pixel 

	# Store result (foreground_pixel) in %ebx                 
  movl %eax, %ebx                

  # Load background_pixel = get_pixel(base_img, c, r)

	# 1st arg = pointer to base_img
  movq %r12, %rdi               

	# 2nd arg = column index (c) directly from stack
  movl -8(%rbp), %esi       

	# 3rd arg = row index (r)    
  movl %r15d, %edx

	# Call get_pixel(base_img, c, r)               
  call get_pixel                 
  
  # Blend the pixels using blend_colors

	# 1st arg = foreground_pixel
  movl %ebx, %edi  

	# 2nd arg = background_pixel              
  movl %eax, %esi  

	# Call blend_colors(bg_pixel, fg_pixel)              
  call blend_colors

	# Store blended result in %ebx              
  movl %eax, %ebx               

  # Store blended_pixel = set_pixel(output_img, c, r, blended_pixel)

	# 1st arg = pointer to output_img
  movq %r14, %rdi   

	# 2nd arg = column index (c) directly from stack             
  movl -8(%rbp), %esi  

	# 3rd arg = row index (r)          
  movl %r15d, %edx

	# 4th arg = blended_pixel               
  movl %ebx, %ecx 

	# Call set_pixel(output_img, c, r, blended_pixel)               
  call set_pixel                 

  # Increment column counter (update value on stack)

	# c++
  incl -8(%rbp)    

	# Repeat inner loop             
  jmp .LinnerLoop                

.LnextRow:
  # Increment row counter

	# r++
  incl %r15d     

	# Repeat outer loop
  jmp .LouterLoop                 

.LreturnComposite:
  # Restore stack and registers

	# Restore callee-saved registers
  popq %r15                      
  popq %r14
  popq %r13
  popq %r12
  popq %rbx  

	# Free space used for c on stack                  
  addq $8, %rsp        

	# Restore %rbp          
  popq %rbp                      

  ret

/*
vim:ft=gas:
*/
