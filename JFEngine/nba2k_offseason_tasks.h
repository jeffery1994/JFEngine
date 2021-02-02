#pragma once
#include <iostream>

using namespace std;

static const int NBA2K_NUM_OF_OFFSEASON_TASKS = 100;

class NBA2KOffseasonManager
{
public:
	NBA2KOffseasonManager() {}

	NBA2KOffseasonManager(int Data[], int numberOfData)
	{
		for (auto idx = 0; idx < numberOfData; ++idx)
		{
			data[idx] = Data[idx];
		}
		this->numOfData = numberOfData;
		TraverseTaskValues();
		//QuickSortTaskValue(data, 0, numOfData - 1);
		MergeSort(data, storage, 0, numberOfData);
		TraverseTaskValues();
	}
	~NBA2KOffseasonManager() {}

	void ClearTaskValues()
	{
		delete[]TaskValues;
	}

	void QuickSortTaskValue(int data[], int LeftIndex, int RightIndex)
	{
		if (LeftIndex >= RightIndex) return;
		int Pivot = data[LeftIndex];
		int LastValueBiggerThanPivot = LeftIndex + 1;
		for (auto idx = LeftIndex + 1; idx <= RightIndex; ++idx)
		{
			if (data[idx] <= Pivot)
			{
				SwitchInternal(data, idx, LastValueBiggerThanPivot);
				LastValueBiggerThanPivot += 1;
			}
		}
		SwitchInternal(data, LeftIndex, LastValueBiggerThanPivot - 1);
		QuickSortTaskValue(data, LeftIndex, LastValueBiggerThanPivot - 2);
		QuickSortTaskValue(data, LastValueBiggerThanPivot, RightIndex);
	}

	void Merge(int sourceArr[], int tempArr[], int startIndex, int midIndex, int endIndex)
	{
		int i = startIndex, j = midIndex + 1, k = startIndex;
		while (i != midIndex + 1 && j != endIndex + 1)
		{
			if (sourceArr[i] > sourceArr[j])
				tempArr[k++] = sourceArr[j++];
			else
				tempArr[k++] = sourceArr[i++];
		}
		while (i != midIndex + 1)
			tempArr[k++] = sourceArr[i++];
		while (j != endIndex + 1)
			tempArr[k++] = sourceArr[j++];
		for (i = startIndex; i <= endIndex; i++)
			sourceArr[i] = tempArr[i];
	}

	void MergeSort(int sourceArr[], int tempArr[], int startIndex, int endIndex)
	{
		int midIndex;
		if (startIndex < endIndex)
		{
			midIndex = startIndex + (endIndex - startIndex) / 2;
			MergeSort(sourceArr, tempArr, startIndex, midIndex);
			MergeSort(sourceArr, tempArr, midIndex + 1, endIndex);
			Merge(sourceArr, tempArr, startIndex, midIndex, endIndex);
		}
	}

	void MergeSortTaskValue(int data[], int LeftIndex, int RightIndex)
	{
		
	}

	void QuickSortTaskValueNonRecursively(int data[], int LeftIndex, int RightIndex)
	{
		
	}

	void MergeSortTaskValueNonRecursively(int Data[], int LeftIndex, int RightIndex)
	{

	}

	void SwitchInternal(int data[], int Index1, int Index2)
	{
		int temp = data[Index1];
		data[Index1] = data[Index2];
		data[Index2] = temp;
	}

	void TraverseTaskValues()
	{
		for (auto idx = 0; idx < numOfData; ++idx)
		{
			cout << data[idx] << " ";
		}
		cout << endl;
	}
private:
	int numOfData = 0;
	int data[NBA2K_NUM_OF_OFFSEASON_TASKS];
	int storage[NBA2K_NUM_OF_OFFSEASON_TASKS];
	int* TaskValues;
};