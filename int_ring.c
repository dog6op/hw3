#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "util.h"

int main(int argc, char *argv[])
{
    int N, size, rank, message_in, message_out, i, Dst, Src;
    int tag = 99;
    MPI_Status status;
    timestamp_type t0, t1;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(argc != 2){
        printf("Specify how often the message is communicated around the ring.");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    
    else{
        N = atoi(argv[1]);
        message_in = 0;
        
        get_timestamp(&t0);
        
        for(i = 0; i < N; i++){
            if(rank == 0){
                message_out = message_in + rank;
                Dst = rank % size +1;
                Src = size - 1;
                MPI_Send(&message_out, 1, MPI_INT, Dst, tag, MPI_COMM_WORLD);
                MPI_Recv(&message_in, 1, MPI_INT, Src, tag, MPI_COMM_WORLD, &status);
            }
            else if(rank == size - 1){
                message_out = message_in + rank;
                Dst = 0;
                Src = rank - 1;
                MPI_Send(&message_out, 1, MPI_INT, Dst, tag, MPI_COMM_WORLD);
                MPI_Recv(&message_in, 1, MPI_INT, Src, tag, MPI_COMM_WORLD, &status);
            }
            else{
                message_out = message_in + rank;
                Dst = rank % size + 1;
                Src = rank - 1;
                MPI_Send(&message_out, 1, MPI_INT, Dst, tag, MPI_COMM_WORLD);
                MPI_Recv(&message_in, 1, MPI_INT, Src, tag, MPI_COMM_WORLD, &status);
            }
        }
        
        get_timestamp(&t1);
        
        if(rank == 0){
            double t = timestamp_diff_in_seconds(t0, t1);
            printf("Latency: %lf.", t/N/size);
        }
    }
    
    MPI_Finalize();
    return 0;
}
 