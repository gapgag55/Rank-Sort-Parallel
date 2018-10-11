#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>

void shuffle(int *array, size_t n) {
  if (n > 1) {
    size_t i, j;
    int t;
    for (i = 0; i < n - 1; i++) {
      j = i + rand() / (RAND_MAX / (n - i) + 1);
      t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}

int main(int argc, char *argv[]) {
  int MAX;
  int i,j,myid,numprocs;
  double t1, t2;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  MPI_Status status;

  if(myid == 0) {
    printf("Number of Elements: ");
    fflush(stdout);
    scanf("%d", &MAX);
  }

  MPI_Bcast((void*)&MAX, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int list[MAX];
  int buffer[MAX/numprocs];
  int rank_list[MAX/numprocs];
  int all_rank_list[MAX];
  int sort_list[MAX];

  for(i = 0; i < MAX/numprocs; i++) {
    rank_list[i] = 0;
    sort_list[i] = 0;
  }

  if (myid == 0) {
    for(i=0; i<MAX; i++)
      list[i] = i;
    shuffle(list,MAX);
    t1 = MPI_Wtime();
  }

  MPI_Bcast(
    (void*)list, MAX, MPI_INT, 0, MPI_COMM_WORLD
  );
  MPI_Scatter(
    (void*)list, MAX/numprocs,MPI_INT,
    (void*)buffer, MAX/numprocs, MPI_INT, 0, MPI_COMM_WORLD
  );

  int position;
  for(i = 0; i < MAX/numprocs; i++) {
    position = 0;
    for(j = 0; j < MAX; j++) {
      if(buffer[i] > list[j]) position++;
    }
    rank_list[i] = position;
  }

  MPI_Gather(
    (void*)rank_list, MAX/numprocs, MPI_INT,
    (void*)all_rank_list, MAX/numprocs, MPI_INT, 0, MPI_COMM_WORLD
  );

  if(myid == 0) {
    for(i = 0; i < MAX; i++)
      sort_list[all_rank_list[i]] = list[i];
    t2 = MPI_Wtime();
    printf("Time spent: %.3f sec\n",t2-t1);
  }

  MPI_Finalize();
}
