#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>


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
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // TODO: binary tree redunction
   unsigned seed;
   seed = (unsigned)time(NULL);
   srand((int)seed*(int)world_rank);
	   
   int part = tosses / world_size;
   long long int num_in_cir = 0;
  
   for(int i=0; i<part; i++){
      double x = 2*((double)rand()) / (RAND_MAX+1.0) -1;
      double y = 2*((double)rand()) / (RAND_MAX+1.0) -1;

      double distance_squared = x*x + y*y;
      if(distance_squared <=1.0){
         num_in_cir++;
      }
   }
   

   int action[16][4]={
      1,1,1,1, 
      -1,0,0,0,
      1,-1,0,0,
      -1,0,0,0,
      1,1,-1,0,
      -1,0,0,0,
      1,-1,0,0,
      -1,0,0,0,
      1,1,1,-1,
      -1,0,0,0,
      1,-1,0,0,
      -1,0,0,0,
      1,1,-1,0,
      -1,0,0,0,
      1,-1,0,0,
      -1,0,0,0
   };


   int a = 1;
   for(int i=0; i<log2(world_size); i++){
       if(action[world_rank][i]==1){
	  long long int temp_num = 0;
          MPI_Recv(&temp_num, 1, MPI_LONG_LONG, world_rank+a, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	  num_in_cir += temp_num;
       }
       else if(action[world_rank][i]==-1){
          MPI_Send(&num_in_cir, 1, MPI_LONG_LONG, world_rank-a, 0, MPI_COMM_WORLD);
       }
       a *= 2;
   }
     
  

    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = ((double)num_in_cir / (double)tosses)*4;	
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
