#include <assert.h>
#include "PartialQSort.h"

// PartialSortPartition() carries out a "partial" quick sort and returns a pivotIndex in
// [loIndex, hiIndex] so that
// * all entries in A[loIndex:(pivotIndex - 1)] are smaller than or equal to A[pivotIndex], and
// * all entries in A[(pivotIndex + 1):hiIndex] are larger than or equal to A[pivotIndex].
// A[] could be reordered as PartialSortPartition() returns.
int		PartialSortPartition(float A[], int loIndex, int hiIndex, float fPivot)
{
	int		i, j;
	float	fTemp;

	for (i = loIndex; (i < hiIndex) && (A[i] > fPivot); i++);
	// Now i points to the smallest index where A[i] <= fPivot, or hiIndex if all A[loIndex:(hiIndex - 1)] are larger than fPivot.

	for (j = hiIndex; (j > loIndex) && (A[j] < fPivot); j--);
	// Now j points to the largest index where A[j] >= fPivot, or loIndex if all A[(loIndex + 1):hiIndex] are smaller than fPivot.

	while (i < j)
	{
		// At this point i points to the smallest index where A[i] <= fPivot, and j points to the largest index where A[j] >= fPivot,
		// while i < j.  Both i and j cannot go beyond the valid range [loIndex..hiIndex].  After swapping, there will be an A[j] <= fPivot
		// and j larger than i, so the do-while loop below will hit that A[j] and break.  This means we no longer need to check if
		// i <= hiIndex as we keep incrementing i inside the do-while loop.  By the same argument we do not need to check if j >= loIndex
		// either.

		// Swapping A[i] and A[j]
		fTemp = A[i];
		A[i] = A[j];
		A[j] = fTemp;

		do
		{
			i++;
		} while (A[i] > fPivot);
		// Now i points to the smallest index where A[i] <= fPivot.  Naturally i <= hiIndex so we do not need to check it in the loop.

		do
		{
			j--;
		} while (A[j] < fPivot);
		// Now j points to the largest index where A[j] >= fPivot.  Naturally j >= loIndex so we do not need to check it in the loop.
	}

	// When we return, we are just sure that A[loIndex:(j - 1)] are all larger than fPivot and A[(j+1):hiIndex} are smaller than fPivot.
	// A[j] itself is larger than fPivot but it could also be larger than some of A[loIndex:(j - 1)].
	for (i = loIndex; i < j; i++)
		assert(A[i] >= fPivot);
	for (i = j + 1; i <= hiIndex; i++)
	{
		assert(A[i] <= fPivot);
		assert(A[i] <= A[j]);
	}

	return j;
}


// PartialSort() sorts the array A[loIndex:hiIndex] so that when it returns, A[loIndex:midIndex] are sorted in descending order.
// The partial array A[(midIndex + 1):hiIndex] can be arranged in any order.

// When calling PartialSort(), the caller is responsible to make sure loIndex <= midIndex <= hiIndex.

// We will use Hoare partition scheme to implement the partial quick sort as described in "Quick Sort" in https://en.wikipedia.org/wiki/Quicksort.
void	PartialSort(float A[], int loIndex, int midIndex, int hiIndex, float fPivot)
{
	// If loIndex == hiIndex, there is nothing to sort (the array to be sorted consists of just one entry), and we can directly return.

	// If midIndex == loIndex, the sorting degenerates into a "finding maximum in A[loIndex:hiIndex] and putting it in A[loIndex]" problem.
	// However, this does not seem to happen frequently enough to implement it for performance considerations.
	if (loIndex == (hiIndex - 1))
	{
		// If loIndex == (hiIndex - 1), then the array to be sorted consists of just two entries, and a simple comparison and swap will work.
		// This is the case regardless of midIndex.
		if (A[loIndex] < A[hiIndex])
		{
			float	fTemp;

			// Swapping A[loIndex] and A[hiIndex]
			fTemp = A[loIndex];
			A[loIndex] = A[hiIndex];
			A[hiIndex] = fTemp;
		}
	}
	else if (loIndex < hiIndex)
	{
		int		pivotIndex;

		pivotIndex = PartialSortPartition(A, loIndex, hiIndex, fPivot);

		// Now A[] has been partitioned into two parts, A[loIndex:pivotIndex] and
		// A[(pivotIndex + 1):hiIndex].  For quick sorting, we sort these two parts in a
		// "divide and conquer" fashion.  For partial sorting, we can skip the second part if
		// midIndex <= pivotIndex because we only care about A[loIndex:midIndex].

		// We take the average of the first and the last entries of the array to be sorted and use them as the pivots for the two
		// partial sortings, to minimize the cases when we hit worst-case scenarios of the quick sort.
		PartialSort(A, loIndex, midIndex, pivotIndex, (A[loIndex] + A[pivotIndex])/2);
		if (midIndex > pivotIndex)
		    PartialSort(A, (pivotIndex + 1), midIndex, hiIndex, (A[pivotIndex + 1] + A[hiIndex])/2);
	}
}



