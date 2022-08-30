#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <mpi.h>
using namespace std;

int main (int argc, char ** argv)
{
	int i, num, rank, tag = 1;   
	double start,end;

    MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	int n_size = atoi(argv[1]);
	vector<float> sort_array;
	vector<float> local_array;
    local_array.resize(sizeof(float)*n_size);
	int size;
	double end_scatter_time, start_scatter_time, scatter_time;
	double end_gather_time, start_gather_time, gather_time;
	double start_send_time,end_send_time, send_time;
	double total_time;

	int *array_sizes =(int *) malloc(num*sizeof(float));
    int *displs = (int *) malloc(num*sizeof(float));
	int *displs_n = (int *) malloc(num*sizeof(float));
    vector< vector<float> > buckets(num);
    
    int sizes[num];
    int *init_mem;

    if(rank == 0) //First computer only (with rank == 0)
    {
		float array[n_size], max_ele = 0.0, min_ele = 10000.0 , cur_ele, range;
		int dividing_num;
    	FILE *file;
		file = fopen("sample_array.txt","r");
    	for(i = 0; i<n_size; i++) fscanf(file, "%f", &array[i]);
		fclose(file);

    	for (i = 0; i<n_size; i++)
		{
			cur_ele = *(array + i);
			if(cur_ele > max_ele) max_ele = cur_ele;
			else if (cur_ele < min_ele) min_ele = cur_ele;
		}
		range = max_ele - min_ele;
		dividing_num = range / num;
        

		start = MPI_Wtime();

		//Put array elements in different buckets
		for (int i = 0; i < n_size; i++) {
			int bi = array[i] / dividing_num; //Index in bucket
			if (bi > num-1) bi = num - 1;
			buckets[bi].push_back(array[i]);
		}

        init_mem = (int*) &buckets[0][0];

        //For Scatterv
        //We need to compute the lowest memory address for the first element
        //in every bucket in order to find the correct displacement
        for (int i = 0; i < num; i++)
        {
            if((int*) &buckets[i][0] < init_mem) init_mem = (int*) &buckets[i][0];
        }

		int size_n = 0 , j, mem_ok=0;

		displs[0] = 0 ;
		displs_n[0] = 0;

		//Send arrays to proper clients
		for (i=0; i<num; i++)
		{
			size_n = buckets[i].size();
            sizes[i] = size_n;
			array_sizes[i] = size_n*sizeof(float);
			if(i < num-1) displs[i+1] = displs[i]+size_n; //Displacements for gatherv
			displs_n[i] = ((int*) &buckets[i][0] - init_mem); //Displacements for scatterv

			//1. First send the array size to the computer
			start_send_time = MPI_Wtime();
			MPI_Send(&size_n,1,MPI_INT, i ,tag, MPI_COMM_WORLD);
			end_send_time = MPI_Wtime();
			send_time += (end_send_time-start_send_time);
		}
    }
	int my_size = 0;
	double start_receive, end_receive, receive_time;

	//1. Receive array size from main computer
	start_receive = MPI_Wtime();
	MPI_Recv(&my_size, 1 , MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
	end_receive = MPI_Wtime();
	receive_time = end_receive - start_receive;

	start_scatter_time = MPI_Wtime();
	MPI_Scatterv(&(*init_mem), sizes , displs_n, MPI_FLOAT, &local_array[0] , my_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
	end_scatter_time = MPI_Wtime();
	scatter_time = end_scatter_time-start_scatter_time;
	
	local_array.resize(my_size);
	sort(local_array.begin(), local_array.end());
	MPI_Barrier(MPI_COMM_WORLD);


	int my_size_n = (int) local_array.size();
	sort_array.resize(sizeof(float)*n_size);
	start_gather_time = MPI_Wtime();
	MPI_Gatherv(&local_array[0], my_size_n , MPI_FLOAT, &sort_array[0] , array_sizes , displs , MPI_FLOAT, 0, MPI_COMM_WORLD);
	end_gather_time = MPI_Wtime();
	gather_time = end_gather_time-start_gather_time;

	MPI_Barrier(MPI_COMM_WORLD);
	end = MPI_Wtime();
	total_time = end - start;
	
	
	if(rank == 0)
	{
		cout << "Sorted array is \n";
		for (int i = 0; i < n_size; i++) cout << sort_array[i] << " ";
		cout<<endl;
		printf("Total time for Scatterv is %f sec\n",scatter_time);
		printf("Total time for Gatherv is %f sec\n",gather_time);
		printf("Total time for Receive is %f sec\n",receive_time);
		printf("Total time for Send is %f sec\n",send_time);
		printf("Total Communication cost: %f sec\n",(scatter_time+gather_time+receive_time+send_time));
		printf("Total time for execution is %f sec\n",total_time);
	} 
	MPI_Finalize();

    free(array_sizes);
    free(displs);

    return 0;
}
