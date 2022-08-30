/* ***DISCLAIMER***

The following code is mostly not mine
It can be found it on GeeksforGeeks website on the following url
https://www.geeksforgeeks.org/bucket-sort-2/

*/

#include <algorithm>
#include <iostream>
#include <vector>
#include <omp.h>
#include <stdio.h>
using namespace std;

void bucketSort(float arr[], int n, float range)
{
	
	vector<float> buckets[n];

	//Put array elements in different buckets
	for (int i = 0; i < n; i++) {
		int bi = arr[i] / range; //Index in bucket
		buckets[bi].push_back(arr[i]);
	}

	//Sort individual buckets
	for (int i = 0; i < n; i++)
		sort(buckets[i].begin(), buckets[i].end());

	//Concatenate all buckets into arr[]
	int index = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < buckets[i].size(); j++)
			arr[index++] = buckets[i][j];
}

int main(int argc, char *argv[])
{
	int n = atoi(argv[1]);
	int i;
	float array[n], max_ele = 0.0, min_ele = 10000.0 , cur_ele, range;
    FILE *file;
	file = fopen("sample_array.txt","r");
    for(i = 0; i<n; i++) fscanf(file, "%f", &array[i]);
   	cout << "Initial array is \n"; //Comment lines 38, 42 and 46 if you don't want array to be printed
    for (i = 0; i<n; i++)
	{
		cur_ele = *(array + i);
		printf("%f ", cur_ele);
		if(cur_ele > max_ele) max_ele = cur_ele;
		else if (cur_ele < min_ele) min_ele = cur_ele; //Finding min and max element to compute range of numbers
	}
	cout<<endl;
	range = max_ele - min_ele;
	double  start = omp_get_wtime();
	bucketSort(array, n, range);
	double  end = omp_get_wtime();
	
	cout << "Sorted array is \n";
	for (int i = 0; i < n; i++)
		cout << array[i] << " ";
	cout<<endl; 
	printf("Total time for execution is %f sec\n",(end-start));
	return 0;
}

