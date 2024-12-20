#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

int compare(const void *left, const void *right);
void swap(int64_t *arr, unsigned long i, unsigned long j);
unsigned long partition(int64_t *arr, unsigned long start, unsigned long end);
int quicksort(int64_t *arr, unsigned long start, unsigned long end, unsigned long par_threshold);
int64_t* mapfile(const char *fname, unsigned long *elements);

// Data type representing a child process
typedef struct {
  pid_t pid;
  int success;
} Child;

int main(int argc, char **argv) {
  unsigned long par_threshold;
  if (argc != 3 || sscanf(argv[2], "%lu", &par_threshold) != 1) {
    fprintf(stderr, "Usage: parsort <file> <par threshold>\n");
    exit(1);
  }

  // determine parameter threshold and number of elements
  par_threshold = strtoul(argv[2], NULL, 10);
  unsigned long num_elements;

  // Map the file to the arr object
  int64_t *arr = mapfile(argv[1], &num_elements);

  // Sort the data!
  int success;
  success = quicksort(arr, 0, num_elements, par_threshold);

  // Return an error if the sorting failed
  if (!success) {
    fprintf(stderr, "Error: sorting failed\n");
    exit(1);
  }

  // Unmap the file data
  munmap(arr, num_elements * sizeof(int64_t));

  return 0;
}

// Helper function to map file data into an array
int64_t* mapfile(const char *fname, unsigned long *elements) {
  // open the named file
  int fd = open(fname, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Error: cannot open named file\n");
    exit(1);
  }

  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    fprintf(stderr, "Error: fstat system call\n");
    exit(1);
  }

  // statbuf.st_size indicates the number of bytes in the file
  *elements = statbuf.st_size / sizeof(int64_t);

  // mmap the file data
  int64_t *arr = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  //can close the file now
  close(fd); 
  
  // Throw an error if mmap system call failed
  if(arr == MAP_FAILED) {
    fprintf(stderr, "Error: mmap system call\n");
    exit(1);
  }

  // Return the file data
  return arr;
}

// Helper function to fork and start a child process for sorting
Child quicksort_subproc(int64_t *arr, unsigned long start, unsigned long end, unsigned long par_threshold) {
  Child child;
  child.pid = fork();

  if (child.pid == 0) {
    // Child process: execute the sorting task
    if (quicksort(arr, start, end, par_threshold)) {
      // Success
      exit(0);  
    } else {
      // Failure
      exit(1);  
    }
  } else if (child.pid < 0) {
    // Fork failed
    child.success = 0;
  } else {
    // Parent process, child created successfully
    child.success = 1;
  }

  return child;
}

// Helper function to wait for a child process to complete
void quicksort_wait(Child *child) {
  if (child->pid > 0) {
    int wstatus;
    if (waitpid(child->pid, &wstatus, 0) < 0) {
      // Wait failed
      child->success = 0;
    } else if (!WIFEXITED(wstatus) || WEXITSTATUS(wstatus) != 0) {
      // Child did not exit normally or exited with an error
      child->success = 0;
    } else {
      // Child exited successfully
      child->success = 1;
    }
  }
}

// Helper function to check if a child completed successfully
int quicksort_check_success(Child *child) {
  return child->success;
}

// Compare elements.
// This function can be used as a comparator for a call to qsort.
//
// Parameters:
//   left - pointer to left element
//   right - pointer to right element
//
// Return:
//   negative if *left < *right,
//   positive if *left > *right,
//   0 if *left == *right
int compare(const void *left, const void *right) {
  int64_t left_elt = *(const int64_t *)left, right_elt = *(const int64_t *)right;

  // Compare the left and right elements for the comparator
  if (left_elt < right_elt) {
    return -1;
  } else if (left_elt > right_elt) {
    return 1;
  } else {
    return 0;
  }
}

// Swap array elements.
//
// Parameters:
//   arr - pointer to first element of array
//   i - index of element to swap
//   j - index of other element to swap
void swap(int64_t *arr, unsigned long i, unsigned long j) {
  int64_t tmp = arr[i];
  arr[i] = arr[j];
  arr[j] = tmp;
}

// Partition a region of given array from start (inclusive)
// to end (exclusive).
//
// Parameters:
//   arr - pointer to first element of array
//   start - inclusive lower bound index
//   end - exclusive upper bound index
//
// Return:
//   index of the pivot element, which is globally in the correct place;
//   all elements to the left of the pivot will have values less than
//   the pivot element, and all elements to the right of the pivot will
//   have values greater than or equal to the pivot
unsigned long partition(int64_t *arr, unsigned long start, unsigned long end) {
  assert(end > start);

  // choose the middle element as the pivot
  unsigned long len = end - start;
  assert(len >= 2);
  unsigned long pivot_index = start + (len / 2);
  int64_t pivot_val = arr[pivot_index];

  // stash the pivot at the end of the sequence
  swap(arr, pivot_index, end - 1);

  // partition all of the elements based on how they compare
  // to the pivot element: elements less than the pivot element
  // should be in the left partition, elements greater than or
  // equal to the pivot should go in the right partition
  unsigned long left_index = start;
  unsigned long right_index = start + (len - 2);

  while (left_index <= right_index) {
    // extend the left partition
    if (arr[left_index] < pivot_val) {
      ++left_index;
      continue;
    }

    // extend the right partition
    if (arr[right_index] >= pivot_val) {
      --right_index;
      continue;
    }

    // left_index refers to an element that should be in the right
    // partition, and right_index refers to an element that should
    // be in the left partition, so swap them
    swap(arr, left_index, right_index);
  }

  // at this point, left_index points to the first element
  // in the right partition, so place the pivot element there
  // and return the left index, since that's where the pivot
  // element is now
  swap(arr, left_index, end - 1);
  return left_index;
}

// Sort specified region of array.
// Note that the only reason that sorting should fail is
// if a child process can't be created or if there is any
// other system call failure.
//
// Parameters:
//   arr - pointer to first element of array
//   start - inclusive lower bound index
//   end - exclusive upper bound index
//   par_threshold - if the number of elements in the region is less
//                   than or equal to this threshold, sort sequentially,
//                   otherwise sort in parallel using child processes
//
// Return:
//   1 if the sort was successful, 0 otherwise
int quicksort(int64_t *arr, unsigned long start, unsigned long end, unsigned long par_threshold) {
  assert(end >= start);
  unsigned long len = end - start;

  // Base case: if there are fewer than 2 elements to sort,
  // do nothing
  if (len < 2) {
    return 1;
  }

  // Base case: if number of elements is less than or equal to
  // the threshold, sort sequentially using qsort
  if (len <= par_threshold) {
    qsort(arr + start, len, sizeof(int64_t), compare);
    return 1;
  }

  // Partition
  unsigned long mid = partition(arr, start, end);

  Child left = quicksort_subproc(arr, start, mid, par_threshold);
  Child right = quicksort_subproc(arr, mid + 1, end, par_threshold);

  // Wait for the left and right children and check if they are successful
  quicksort_wait(&left);
  quicksort_wait(&right);

  int left_success = quicksort_check_success(&left);
  int right_success = quicksort_check_success(&right);

  return left_success && right_success;
}