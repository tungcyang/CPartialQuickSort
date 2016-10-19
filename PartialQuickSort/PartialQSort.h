// Header files from PartialQSort.cpp
#include <stdlib.h>

// compfunc() compares a and b for qsort().
int compfunc(const void *a, const void *b);

// PartialSort() sorts the array A[loIndex:hiIndex] so that when it returns, A[loIndex:midIndex] are sorted.
// The partial array A[(midIndex + 1):hiIndex] can be arranged in any order.

// When calling PartialSort(), the caller is responsible to make sure loIndex <= midIndex <= hiIndex.
void	PartialSort(float A[], int loIndex, int midIndex, int hiIndex,
					int(*compar)(const void*, const void*));
