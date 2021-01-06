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

    // TODO: use MPI_Gather

    unsigned seed;
    seed = (unsigned)time(NULL);
    srand((int)seed*world_rank);

    long long int part = tosses / world_size;
    long long int num_in_cir;
    long long int *local_count;

    for(int i=0; i<part; i++){
       double x = 2*((double)rand()) / (RAND_MAX + 1.0) -1;
       double y = 2*((double)rand()) / (RAND_MAX + 1.0) -1;

       double distance_squared = x*x + y*y;
       if(distance_squared <= 1.0){
          num_in_cir++;
       }
    }

    if (world_rank == 0)
    {
       local_count = (long long int*)malloc(sizeof(long long int)*world_size);
    }

    MPI_Gather(&num_in_cir, 1, MPI_LONG_LONG, local_count, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        // TODO: PI result
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
