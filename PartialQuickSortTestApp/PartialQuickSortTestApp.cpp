// PartialQuickSortTestApp.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "PartialQSort.h"

#define NUM_OF_PARTIALSORT_TEST_ITERATIONS			262144	// Number of test iterations we like to carry out.
#define MAX_SIZE_ARRAY2SORT							2048	// For each test iteration, we randomly select the array size so that it lies
#define MIN_SIZE_ARRAY2SORT							256		// between MAX and MIN_SIZE_ARRAY2SORT.
#define MAX_PROP_PARTIAL_ARRAY2SORT					0.5		// Given the selected array size, we randomly select the size of the partial array
															// which we need to sort.  It should be between 1 and
															// (int) (MAX_PROP_PARTIAL_ARRAY2SORT*array_size).
#define INITIAL_RAND_SEED							0


int main(int argc, char* argv[])
{
	unsigned int	i, j, uiArraySize, uiPartialArraySize;
	float			fValues[MAX_SIZE_ARRAY2SORT];
	float			fNewValues[MAX_SIZE_ARRAY2SORT];

	// Initialize the random seed.
	srand(INITIAL_RAND_SEED);

	for (i = 0; i < NUM_OF_PARTIALSORT_TEST_ITERATIONS; i++)
	{
		// Generating the array size.
		uiArraySize = MIN_SIZE_ARRAY2SORT + (unsigned int)((MAX_SIZE_ARRAY2SORT - MIN_SIZE_ARRAY2SORT + 1)*((float) rand()/(RAND_MAX + 1)));
		assert((uiArraySize >= MIN_SIZE_ARRAY2SORT) && (uiArraySize <= MAX_SIZE_ARRAY2SORT));

		// Generating the partial array size.
		uiPartialArraySize = (unsigned int) (uiArraySize*MAX_PROP_PARTIAL_ARRAY2SORT);	// Maximum partial array size.
		uiPartialArraySize = 1 + (unsigned int)(uiPartialArraySize*((float) rand() / (RAND_MAX + 1)));
		assert((uiPartialArraySize >= 1) && (uiPartialArraySize <= uiArraySize));

		// Filling the array to be sorted.
		for (j = 0; j < uiArraySize; j++)
			fValues[j] = (float) rand() / (RAND_MAX + 1);

		// Copying fValues[] to fNewValues[] for possible debugging purposes (if we need the original array).
		for (j = 0; j < uiArraySize; j++)
			fNewValues[j] = fValues[j];

		// Partial sorting.  For the pivot, we tentatively use fNewValues[0].
		PartialSort(fNewValues, 0, uiPartialArraySize, uiArraySize, fNewValues[0]);

		// Verifying whether the partial sorting is done properly.  Note that here we did not implement the test to make sure
		// no array entries are lost during the sorting.
		// (1) First test: After the partial sorting, fNewValues[0:uiPartialArraySize] needs to be sorted in descending order.
		for (j = 1; j < uiPartialArraySize; j++)
			if (fNewValues[j - 1] < fNewValues[j])
				fprintf(stderr, "This is not sorted in the partial array! %f and %f\n", fNewValues[j - 1], fNewValues[j]);
		// (2) Second test: After the partial sorting, all of fNewValues[(uiPartialArraySize + 1):uiArraySize] cannot be larger
		//     than fNewValues[uiPartialArraySize].
		for (j = (uiPartialArraySize + 1); j < uiArraySize; j++)
			if (fNewValues[j] > fNewValues[uiPartialArraySize])
				fprintf(stderr, "This is not sorted in the tail! %f and %f\n", fNewValues[j], fNewValues[uiPartialArraySize]);
		// (3) Third test (not yet implemented): We need to make sure no entries were duplicated/inserted nor lost during the partial
		//     sorting.  Intuitively we just sort both (with, say, std::sort()) and check if the entries of the results are
		//     identical.
	}

	return(0);
}