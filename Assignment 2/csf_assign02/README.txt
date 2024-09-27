Alp Demirtas (ademirt1)
Ryan Amer (ramer2)

For Assignment 1, Alp worked on the imgproc_tile, the imgproc_composite functions, the determine_tile_x_offset, the determine_tile_y_offset, copy_tile, determine_tile_w, determine_tile_h, and all_tiles_nonempty functions.
Ryan worked on the void imgproc_mirror_h, imgproc_mirror_v, imgproc_grayscale, make_pixel, get_pixel, set_pixel, get_r, get_g, get_b, get_a, to_grayscale, blend_components, and blend_color functions.
The two partners evenly split the responsibilities for writing the unit tests for the functions.

For Assignment 2, Alp worked on implementing the imgproc_mirror_h, imgproc_grayscale, and imgproc_mirror_v functions. He debugged issues that were in the set_pixel, to_grayscale, and get_pixel functions in this process.
Ryan worked on implementing the to_grayscale, make_pixel, set_pixel, get_r, get_g, get_a, and get_b methods. He also ensured that all methods were stack-compliant by pushing and popping callee-saved registers and offseting %rsp.

An interesting implementation this project used was to copy all of the C lines of code and then translate their implementation in Assembly. After each line had been translated, then registers were updated to ensure that the code chunks worked cohesively.
This approach helped overcome the challenges of writing code in Assembly.
