// PartialQuickSortTestApp.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <assert.h>
#include <windows.h>			// Performance measurement
#include "PartialQSort.h"

#define NUM_OF_PARTIALSORT_TEST_ITERATIONS			262144	// Number of test iterations we like to carry out.
#ifdef _DEBUG
#	define MAX_SIZE_ARRAY2SORT							2048	// For each test iteration, we randomly select the array size so that it lies
#	define MIN_SIZE_ARRAY2SORT							256		// between MAX and MIN_SIZE_ARRAY2SORT.
#else
#	define MAX_SIZE_ARRAY2SORT							16384	// We use larger parameters for release builds because the debug build is
#	define MIN_SIZE_ARRAY2SORT							2048	// much slower.
#endif
#define MAX_PROP_PARTIAL_ARRAY2SORT					0.5		// Given the selected array size, we randomly select the size of the partial array
															// which we need to sort.  It should be between 1 and
															// (int) (MAX_PROP_PARTIAL_ARRAY2SORT*array_size).
#define MAX_NUM_DUPLICATED_GROUPS					5		// We can have as many as MAX_NUM_DUPLICATED_GROUPS of entries being identical.
#define MIN_DUPLICATED_GROUP_SIZE					2		// We can have MIN_DUPLICATED_GROUP_SIZE ... MAX_DUPLICATED_GROUP_SIZE entries
#define MAX_DUPLICATED_GROUP_SIZE					7		// to be of identical value in each group (of MAX_NUM_DUPLICATED_GROUPS).
#define INITIAL_RAND_SEED							0

// randInt() generates a random integer in [lowerBound ... upperbound] (both included).  It
// assumes without checking that upperBound >= lowerBound.
int	randInt(int lowerBound, int upperBound)
{
	return(lowerBound + (int)((upperBound - lowerBound + 1)*((float) rand() / (RAND_MAX + 1))));
}

// Function elapsedTime() evaluates the elapsed time between two DWORD timing measurements as returned
// from GetTickCount().  It takes into account the possible wraparound happening between the two
// GetTickCount() calls.
DWORD	elapsedTime(DWORD dStartTime, DWORD dStopTime)
{
	if (dStopTime > dStartTime)
		return (dStopTime - dStartTime);
	else
	{
		// A wraparound occurred between the two GetTickCount() calls.
		return (dStopTime + (ULONG_MAX - dStartTime) + 1);
	}
}

int main(int argc, char* argv[])
{
	unsigned int	i, j, k, l, uiArraySize, uiPartialArraySize, uiNumDupEntries;
	float			fDupValue;
	float			fValues[MAX_SIZE_ARRAY2SORT], fOrigValues[MAX_SIZE_ARRAY2SORT];
	bool			fValueAltered[MAX_SIZE_ARRAY2SORT];
#ifdef _DEBUG
	float			fTemp[MAX_SIZE_ARRAY2SORT], fOrigTemp[MAX_SIZE_ARRAY2SORT];
#endif
	DWORD			dStartTime, dStopTime;
	DWORD			dTotalPartialSortTime = 0, dTotalqsortTime = 0;

	// Initialize the random seed.
	srand(INITIAL_RAND_SEED);

	// Sanity check
	assert(MIN_SIZE_ARRAY2SORT <= MAX_SIZE_ARRAY2SORT);
	assert(MIN_DUPLICATED_GROUP_SIZE >= 2);

	for (i = 0; i < NUM_OF_PARTIALSORT_TEST_ITERATIONS; i++)
	{
		// Generating the array size.
		uiArraySize = randInt(MIN_SIZE_ARRAY2SORT, MAX_SIZE_ARRAY2SORT);

		// Generating the partial array size.
		uiPartialArraySize = randInt(1, (int) (uiArraySize*MAX_PROP_PARTIAL_ARRAY2SORT));	// Maximum partial array size.
		assert((uiPartialArraySize >= 1) && (uiPartialArraySize <= uiArraySize));

		// Filling the array to be sorted.  Note that it could be possible that fValues[] already
		// contain duplicated entries.
		for (j = 0; j < uiArraySize; j++)
			fValues[j] = (float) rand() / (RAND_MAX + 1);

		// Introducing duplicated array entries.  We use fValueAltered[] to keep track of
		// whether fValues[] entries have been used in duplication.
		for (j = 0; j < uiArraySize; j++)
			fValueAltered[j] = false;
		for (j = 0; j < MAX_NUM_DUPLICATED_GROUPS; j++)
		{
			// For the current group, we find out how many duplicated entries we want to
			// introduce, and then randomly select an entry to be duplicated over onto other
			// entries (which was not known to have been used in a duplication).
			uiNumDupEntries = (unsigned int) randInt(MIN_DUPLICATED_GROUP_SIZE, MAX_DUPLICATED_GROUP_SIZE);
			do
			{
				l = randInt(0, uiArraySize);
				fDupValue = fValues[l];
			} while (fValueAltered[l]);
			fValueAltered[l] = true;

			// Now we want to find (iNumDupEntries - 1) entries to make them duplicate of fDupValue
			// which are not already duplicated (fValueAltered[] being false).

			for (k = 0; k < (uiNumDupEntries - 1); k++)
			{
				do
				{
					l = randInt(0, uiArraySize);
				} while (fValueAltered[l]);
				fValues[l] = fDupValue;
				fValueAltered[l] = true;
			}
		}

		// Copying fValues[] to fOrigValues[] for possible debugging purposes (if we need the original array).
		for (j = 0; j < uiArraySize; j++)
			fOrigValues[j] = fValues[j];

		// Partial sorting.
		dStartTime = GetTickCount();
		PartialSort(fValues, 0, (uiPartialArraySize - 1), (uiArraySize - 1), compfunc);
		dStopTime = GetTickCount();
		dTotalPartialSortTime += elapsedTime(dStartTime, dStopTime);

#ifdef _DEBUG
		// Verifying whether the partial sorting is done properly in debug mode.

		// (1) First test: After the partial sorting, fValues[0:(uiPartialArraySize - 1)] needs to be sorted.
		for (j = 1; j < uiPartialArraySize; j++)
			if (compfunc(&(fValues[j - 1]), &(fValues[j])) == 1)
				fprintf(stderr, "Error!  This is not sorted in the partial array! %f and %f\n", fValues[j - 1], fValues[j]);

		// (2) Second test: After the partial sorting, all of fValues[uiPartialArraySize:(uiArraySize - 1)] cannot be larger
		//     than fValues[(uiPartialArraySize - 1)].
		for (j = uiPartialArraySize; j < uiArraySize; j++)
			if (compfunc(&(fValues[uiPartialArraySize - 1]), &(fValues[j])) == 1)
				fprintf(stderr, "Error!  This is not sorted in the tail! %f and %f\n", fValues[j], fValues[uiPartialArraySize]);

		// (3) Third test: We need to make sure no entries were introduced nor lost during the partial sorting.
		//                 In other words, the partially sorted fValues[] and fOrigValues[] will be identical
		//                 if they are completely sorted.  We use fTemp[] and fOrigTemp[] for verification.
		for (j = 0; j < uiArraySize; j++)
			fTemp[j] = fValues[j];
		for (j = 0; j < uiArraySize; j++)
			fOrigTemp[j] = fOrigValues[j];
		qsort(fTemp, uiArraySize, sizeof(float), compfunc);
		qsort(fOrigTemp, uiArraySize, sizeof(float), compfunc);
		for (j = 0; j < uiArraySize; j++)
			if (fTemp[j] != fOrigTemp[j])
				fprintf(stderr, "Array entries altered through Partial Quicksort when i and j are %d and %d\n", i, j);
#else
		// In release mode we measure the performance comparison between PartialSort() and qsort().
		for (j = 0; j < uiArraySize; j++)
			fValues[j] = fOrigValues[j];

		dStartTime = GetTickCount();
		qsort(fValues, uiArraySize, sizeof(float), compfunc);
		dStopTime = GetTickCount();
		dTotalqsortTime += elapsedTime(dStartTime, dStopTime);
#endif
	}

#ifndef _DEBUG
	// Showing performance comparison statistics.
	fprintf(stdout, "Under given sorting workloads:\n");
	fprintf(stdout, "\tFor %d float arrays of sizes between %d and %d entries,\n", NUM_OF_PARTIALSORT_TEST_ITERATIONS,
			MIN_SIZE_ARRAY2SORT, MAX_SIZE_ARRAY2SORT);
	fprintf(stdout, "\twhere up to %f of the entries need to be partially sorted,\n", MAX_PROP_PARTIAL_ARRAY2SORT);
	fprintf(stdout, "\tqsort() takes %f seconds to finish;\n", (float) (dTotalqsortTime / 1000.0));
	fprintf(stdout, "\tPartialSort() takes %f seconds to finish;\n", (float) (dTotalPartialSortTime / 1000.0));
#endif
	fprintf(stdout, "Testing of Partial Quicksort done.\n");
	return(0);
}