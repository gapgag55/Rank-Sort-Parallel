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
  int i,j,rank,numprocs;
  double t1, t2;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Status status;

  if(rank == 0) {
    printf("Number of Elements: ");
    fflush(stdout);
    scanf("%d", &MAX);
  }

  MPI_Bcast((void*)&MAX, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int list[MAX];
  int size = MAX/numprocs;
	int buffer[size];
  int rank_list[size];
  int all_rank_list[MAX];
  int sort_list[MAX];

	if (rank == 0) {
    for(i = 0; i < MAX; i++) {
      list[i] = i;
    }

    shuffle(list, MAX);
  }

  MPI_Bcast((void*)list, MAX, MPI_INT, 0, MPI_COMM_WORLD);

  int position;
  int start = rank * size;
  int end = start + size;
  int index = 0;

  t1 = MPI_Wtime();
  for(i = start; i < end; i++) {
    position = 0;
    for(j = 0; j < MAX; j++) {
      if(list[i] > list[j]) position++;
    }
    rank_list[index++] = position;
  }

	t2 = MPI_Wtime();
  double spent = t2 - t1;
  double time;
  MPI_Reduce(&spent, &time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  MPI_Gather(
    (void*)rank_list, size, MPI_INT,
    (void*)all_rank_list, size, MPI_INT, 0, MPI_COMM_WORLD
  );

  if(rank == 0) {
    for(i = 0; i < MAX; i++)
      sort_list[all_rank_list[i]] = list[i];

    t2 = MPI_Wtime();
    for(i = 0; i < MAX; i++) {
      printf("%d ", sort_list[i]);
    }

    printf("\nTime spent: %f sec\n", time);
  }

  MPI_Finalize();
}