// Header files from PartialQSort.cpp

// PartialSort() sorts the array A[loIndex:hiIndex] so that when it returns, A[loIndex:midIndex] are sorted in descending order.
// The partial array A[(midIndex + 1):hiIndex] can be arranged in any order.

// When calling PartialSort(), the caller is responsible to make sure loIndex <= midIndex <= hiIndex.
void	PartialSort(float A[], int loIndex, int midIndex, int hiIndex, float fPivot);
