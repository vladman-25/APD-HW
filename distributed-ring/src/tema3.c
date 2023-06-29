#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NR_OF_BOSS 4
#define MAX_WORKERS 10
#define MAX_SIZE 10000
void print_topology(int rank, int table[][10]) {
    char buffer[100] = "";
    sprintf(buffer + strlen(buffer), "%d -> ", rank);
    for(int i = 0; i < NR_OF_BOSS; i++) {
        sprintf(buffer + strlen(buffer), "%d:", i);

        int c = 0;
        while(table[i][c] != 0) {
            sprintf(buffer + strlen(buffer), "%d,", table[i][c]);
            c++;
        }
        if(c > 0) {
            sprintf(buffer + strlen(buffer)-1, " ");
        } else  {
            sprintf(buffer + strlen(buffer), " ");
        }
    }

    printf("%s\n",buffer);
    return;
}

int get_nr_workers(int table[][10]) {
    int total = 0;
    for(int i = 0; i < NR_OF_BOSS; i++) {
        int c = 0;
        while(table[i][c] != 0) {
            c++;
        }
        total += c;
    }

    return total;
}

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    MPI_Status status;
    // int** table;
    // table = calloc(NR_OF_BOSS,sizeof(int*));
    // for(int i = 0; i < NR_OF_BOSS; i ++) {
    //     table[i] = calloc(MAX_WORKERS,sizeof(int));
    // }
    int table[4][10];
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 10; j++) {
            table[i][j] = 0;
        }
    }
    int finalv[MAX_SIZE];
    int final_size = 0;

    int arr_len[2] = {0};

    if (rank == 0) {
    

        char file[50];
        sprintf(file, "./cluster%d.txt", rank);

        FILE *fptr;
        fptr = fopen(file,"r");
        int nr_workers = 0;
        fscanf(fptr,"%d",&nr_workers);

        for(int i = 0; i < nr_workers; i++) {
            int x;
            fscanf(fptr,"%d",&x);
            table[rank][i] = x;
        }

        MPI_Send(table, 40, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,3);
        MPI_Recv(table, 40, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(table, 40, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
        }

        int N = atoi(argv[1]);
        int v[N];
        for(int i = 0; i < N; i++) {
            v[i] =  N - i - 1;
        }
        int total_workers = get_nr_workers(table);
        
        int rangemin = 0;
        float aux = (N * nr_workers) / total_workers;
        int rangemax = aux;
        /////////////
        aux = N / total_workers;
        arr_len[0] = N;
        arr_len[1] = rangemin;
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(arr_len, 2, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
            arr_len[1] += aux + 1;
        }
        ////////////
        arr_len[0] = N;
        MPI_Send(arr_len, 2, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,3);

        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(v, N, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
        }

        MPI_Send(v, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,3);
        

        
        aux = N / total_workers;
        int section = aux;
        int start = 0;
        int final = section + 1;

        for(int i = 0; i < nr_workers; i++) {
            MPI_Recv(v, arr_len[0], MPI_INT, table[rank][i], 0, MPI_COMM_WORLD, &status);
            for(int j = start; j < final; j++) {
                finalv[j] = v[j];
            }
            start = final;
            final = start + section + 1;
            if(final > N) {
                final = N;
            }
        }
        int count = 0;
        while(table[0][count] != 0) {
            count ++;
        }
        count = get_nr_workers(table) - count;
        for(int i = 0; i < count; i++) {
            MPI_Recv(v, arr_len[0], MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
            for(int j = start; j < final; j++) {
                finalv[j] = v[j];
            }
            start = final;
            final = start + section + 1;
            if(final > N) {
                final = N;
            }
        }
        final_size = N;

    } else if (rank == 1) {

        char file[50];
        sprintf(file, "./cluster%d.txt", rank);

        FILE *fptr;
        fptr = fopen(file,"r");
        int nr_workers = 0;
        fscanf(fptr,"%d",&nr_workers);

        MPI_Recv(table, 40, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

        for(int i = 0; i < nr_workers; i++) {
            int x;
            fscanf(fptr,"%d",&x);
            table[rank][i] = x;
        }

        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(table, 40, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
        }
        MPI_Send(table, 40, MPI_INT, 2, 0, MPI_COMM_WORLD);

        MPI_Recv(arr_len, 2, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        int total_workers = get_nr_workers(table);
        int rangemin = arr_len[1];
        float aux = (arr_len[0] * nr_workers) / total_workers;
        int rangemax = arr_len[1] + aux;
        /////////////
        aux = arr_len[0] / total_workers;
        arr_len[1] = rangemin;
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(arr_len, 2, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
            arr_len[1] += aux + 1;
        }
        ////////////

        int v[MAX_SIZE];
        MPI_Recv(v, arr_len[0], MPI_INT, 2, 0, MPI_COMM_WORLD,&status);
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(v, arr_len[0], MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
        }

        for(int i = 0; i < nr_workers; i++) {
            MPI_Recv(v, arr_len[0], MPI_INT, table[rank][i], 0, MPI_COMM_WORLD, &status);
            MPI_Send(v, arr_len[0], MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,2);
        }

    } else if (rank == 2) {
        char file[50];
        sprintf(file, "./cluster%d.txt", rank);

        FILE *fptr;
        fptr = fopen(file,"r");
        int nr_workers = 0;
        fscanf(fptr,"%d",&nr_workers);

        MPI_Recv(table, 40, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

        for(int i = 0; i < nr_workers; i++) {
            int x;
            fscanf(fptr,"%d",&x);
            table[rank][i] = x;
        }
        MPI_Send(table, 40, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,1);

        MPI_Recv(table, 40, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(table, 40, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
        }
        MPI_Send(table, 40, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,3);

        MPI_Recv(arr_len, 2, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
        int total_workers = get_nr_workers(table);
        int rangemin = arr_len[1];
        float aux = (arr_len[0] * nr_workers) / total_workers;
        int rangemax = arr_len[1] + aux;
        /////////////
        aux = arr_len[0] / total_workers;
        arr_len[1] = rangemin;
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(arr_len, 2, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
            arr_len[1] += aux + 1;
        }
        ////////////

        MPI_Send(arr_len, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,1);

        int v[MAX_SIZE];
        MPI_Recv(v, arr_len[0], MPI_INT, 3, 0, MPI_COMM_WORLD,&status);
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(v, arr_len[0], MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
        }
        MPI_Send(v, arr_len[0], MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,1);

        for(int i = 0; i < nr_workers; i++) {
            MPI_Recv(v, arr_len[0], MPI_INT, table[rank][i], 0, MPI_COMM_WORLD, &status);
            MPI_Send(v, arr_len[0], MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,3);
        }
        int count = 0;
        while(table[1][count] != 0) {
            count ++;
        }
        for(int i = 0; i < count; i++) {
            MPI_Recv(v, arr_len[0], MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
            MPI_Send(v, arr_len[0], MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,3);
        }

    } else if (rank == 3) {
        char file[50];
        sprintf(file, "./cluster%d.txt", rank);

        FILE *fptr;
        fptr = fopen(file,"r");
        int nr_workers = 0;
        fscanf(fptr,"%d",&nr_workers);

        MPI_Recv(table, 40, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        for(int i = 0; i < nr_workers; i++) {
            int x;
            fscanf(fptr,"%d",&x);
            table[rank][i] = x;
        }
    

        MPI_Send(table, 40, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,2);
        MPI_Recv(table, 40, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(table, 40, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
        }
        MPI_Send(table, 40, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,0);

        MPI_Recv(arr_len, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        int total_workers = get_nr_workers(table);
        int rangemin = arr_len[1];
        float aux = (arr_len[0] * nr_workers) / total_workers;
        int rangemax = arr_len[1] + aux;
        /////////////
        aux = arr_len[0] / total_workers;
        arr_len[1] = rangemin;
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(arr_len, 2, MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
            arr_len[1] += aux + 1;
        }
        ////////////

        MPI_Send(arr_len, 2, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,2);

        int v[MAX_SIZE];
        MPI_Recv(v, arr_len[0], MPI_INT, 0, 0, MPI_COMM_WORLD,&status);
        for(int i = 0; i < nr_workers; i++) {
            MPI_Send(v, arr_len[0], MPI_INT, table[rank][i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,table[rank][i]);
        }
        MPI_Send(v, arr_len[0], MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,2);

        for(int i = 0; i < nr_workers; i++) {
            MPI_Recv(v, arr_len[0], MPI_INT, table[rank][i], 0, MPI_COMM_WORLD, &status);
            MPI_Send(v, arr_len[0], MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,0);
        }
        int count = 0;
        int j = 0;
        while(table[1][j] != 0) {
            j ++;
        }
        count = j;
        j = 0;
        while(table[2][j] != 0) {
            j ++;
        }
        count += j;
        for(int i = 0; i < count; i++) {
            MPI_Recv(v, arr_len[0], MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
            MPI_Send(v, arr_len[0], MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",rank,0);
        }

    } 
    else {
        MPI_Recv(table, 40, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        MPI_Recv(arr_len, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        int total_workers = get_nr_workers(table);
        float aux = arr_len[0] / total_workers;
        int rangemin = arr_len[1];
        int rangemax = arr_len[1] + aux + 1;
        if(rangemax > arr_len[0]) {
            rangemax = arr_len[0];
        }
        // printf("%d -> v[%d] to v[%d]-----------------------\n",rank,rangemin,rangemax);

        int v[MAX_SIZE];
        MPI_Recv(v, arr_len[0], MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,&status);

        for(int i = rangemin; i < rangemax; i ++) {
            v[i] *= 5;
        }

        int parent = -1;
        for(int i = 0; i < NR_OF_BOSS; i ++) { 
            int j = 0;
            while(table[i][j] != 0) {
                if (table[i][j] == rank) {
                    parent = i;
                }
                j++;
            }
        }
        MPI_Send(v, arr_len[0], MPI_INT, parent, 0, MPI_COMM_WORLD);
    }
    print_topology(rank, table);
    if (rank == 0) {
        printf("Rezultat: ");
        for (int i = 0 ; i < final_size; i++) {
            printf("%d ",finalv[i]);
        }
         printf("\n");
    }
    MPI_Finalize();


}
