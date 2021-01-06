#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    MPI_Win win;

    // TODO: MPI init
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    unsigned seed = (unsigned)time(NULL);
    srand((int)seed*world_rank);
    long long int part = tosses / world_size;
    long long int *local_count;

    if (world_rank == 0)
    {
        // Master
	long long int num_in_cir = 0;
	MPI_Alloc_mem(world_size*sizeof(long long int), MPI_INFO_NULL, &local_count);

	for(int i=0; i<part; i++){
	   double x = 2*((double)rand()) / (RAND_MAX + 1.0) -1;
	   double y = 2*((double)rand()) / (RAND_MAX + 1.0) -1;

	   double distance_squared = x*x + y*y;
	   if(distance_squared<=1.0){
	      num_in_cir++;
	   }
	}
	
	
	MPI_Win_create(local_count, world_size*sizeof(long long int), sizeof(long long int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Put(&num_in_cir, 1, MPI_LONG_LONG, 0, world_rank, 1, MPI_LONG_LONG, win);
        MPI_Win_unlock(0, win);

		
    }
    else
    {
        // Workers
	long long int num_in_cir = 0;
	for(int i=0; i<part; i++){
	   double x = 2*((double)rand()) / (RAND_MAX + 1.0) -1;
	   double y = 2*((double)rand()) / (RAND_MAX + 1.0) -1;

	   double distance_squared = x*x + y*y;
	   if(distance_squared <= 1.0){
	      num_in_cir++;
	   } 
	}

	MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Put(&num_in_cir, 1, MPI_LONG_LONG, 0, world_rank, 1, MPI_LONG_LONG, win);
        MPI_Win_unlock(0, win);

    }

    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result

	
	long long int final_count = 0;
	for(int i=0; i<world_size; i++){
	   final_count += local_count[i];
	}

	pi_result = ((double)final_count / (double)tosses) *4;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}
