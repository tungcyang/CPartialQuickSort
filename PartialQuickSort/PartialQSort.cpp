#include <assert.h>
#include <stdlib.h>
#include "PartialQSort.h"

// compfunc() compares a and b for qsort().
int compfunc(const void *a, const void *b)
{
	// Make compfunc() return 1 when a > b for sorting in ascending order, and make it
	// return 1 when a < b for sorting in descending order.
	if (*(float*)a < *(float*)b) return 1;
	if (*(float*)a > *(float*)b) return -1;

	// if a is neither greater than nor smaller than b
	return 0;
}


// PartialSortPartition() carries out a "partial" quick sort and returns a pivotIndex in
// [loIndex, hiIndex] so that
// * all entries in A[loIndex:(pivotIndex - 1)] are larger than or equal to fPivot, and
// * all entries in A[(pivotIndex + 1):hiIndex] are smaller than or equal to fPivot.
// A[] could be reordered as PartialSortPartition() returns.
// It is implemented based on the Hoare partition as described in
// https://en.wikipedia.org/wiki/Quicksort#Hoare_partition_scheme
int		PartialSortPartition(float A[], int loIndex, int hiIndex,
							 int(*compar)(const void*, const void*))
{
	int		i, j;
	float	fTemp, fPivot;

	// pivot: = A[lo]
	// i : = lo – 1
	// j : = hi + 1
	fPivot = A[loIndex];
	i = loIndex - 1;
	j = hiIndex + 1;

	// loop forever
	while (1)
	{
		// do
		//	  i : = i + 1
		// while A[i] > pivot
		do {
			i++;
		} while (compar(&fPivot, &(A[i])) == 1);

		// do
		//    j : = j – 1
		// while A[j] < pivot
		do {
			j--;
		} while (compar(&fPivot, &(A[j])) == -1);

		// if i >= j then
		//     return j
		if (i >= j)
			return j;

		// swap A[i] with A[j]
		fTemp = A[i];
		A[i] = A[j];
		A[j] = fTemp;
	}
}


// PartialSort() sorts the array A[loIndex:hiIndex] so that when it returns, A[loIndex:midIndex] are sorted.
// The partial array A[(midIndex + 1):hiIndex] can be arranged in any order.

// When calling PartialSort(), the caller is responsible to make sure loIndex <= midIndex <= hiIndex.

// We will use Hoare partition scheme to implement the partial quick sort as described in "Quick Sort"
// in https://en.wikipedia.org/wiki/Quicksort#Hoare_partition_scheme.
void	PartialSort(float A[], int loIndex, int midIndex, int hiIndex,
					int(*compar)(const void*, const void*))
{
	// We have something to work on only when loIndex < hiIndex.  If they are equal,
	// A[] degenerates into one entry and there is nothing to sort.
	if (loIndex < hiIndex)
	{
		int		pivotIndex = PartialSortPartition(A, loIndex, hiIndex, compar);
		// Now A[] has been partitioned into two parts, A[loIndex:pivotIndex] and
		// A[(pivotIndex + 1):hiIndex].  For quick sorting, we sort these two parts in a
		// "divide and conquer" fashion.

		PartialSort(A, loIndex, midIndex, pivotIndex, compar);

		// For partial sorting, we can skip the second part if
		// midIndex <= pivotIndex because we only care about A[loIndex:midIndex].
		if (midIndex > pivotIndex)
			PartialSort(A, (pivotIndex + 1), midIndex, hiIndex, compar);
	}
}



