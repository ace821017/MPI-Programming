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

    // TODO: MPI init
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    unsigned seed;
    seed = (unsigned)time(NULL);
    srand((int)seed*world_rank);

    long long int part = tosses / world_size;
    long long int *local_count;


    if (world_rank > 0)
    {
        // TODO: MPI workers
	
	long long int num_in_cir = 0;
	
	for(int i=0; i<part; i++){
	   double x = 2*((double)rand()) / (RAND_MAX + 1.0) -1;
	   double y = 2*((double)rand()) / (RAND_MAX + 1.0) -1;

	   double distance_squared = x*x + y*y;
	   if(distance_squared <= 1.0){
	      num_in_cir++;
	   }
	}
	MPI_Send(&num_in_cir, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
        MPI_Request* requests = (MPI_Request *)malloc(sizeof(MPI_Request) * world_size);
        MPI_Status* status = (MPI_Status *)malloc(sizeof(MPI_Status) * world_size);
	local_count = (long long int*)malloc(sizeof(long long int) * world_size); 

	long long int num_in_cir = 0;
        
	for(int i=0; i<part; i++){
	   double x = 2*((double)rand()) / (RAND_MAX + 1.0) -1;
	   double y = 2*((double)rand()) / (RAND_MAX + 1.0) -1;

	   double distance_squared = x*x + y*y;
	   if(distance_squared <= 1.0){
	      num_in_cir++;
	   }
	}

	local_count[0] = num_in_cir;
	for(int i=1; i<world_size; i++){
	   MPI_Irecv(&local_count[i], 1, MPI_LONG_LONG, i, 0,MPI_COMM_WORLD, &requests[i]);
	}

        MPI_Waitall(world_size-1, requests+1, status+1);
    }

    if (world_rank == 0)
    {
        // TODO: PI result
	long long int final_count = 0;
	for(int i=0; i<world_size; i++){
	   final_count += local_count[i];
	}

	pi_result = ((double)final_count/(double)tosses)*4;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
