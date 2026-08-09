// Parallel computing with MPI + OpenMP
//
// MPI distributes work across processes (ranks), across multiple hosts.
// OpenMP parallelises the work of a single rank across threads within one host.
//
// The program estimates pi via the midpoint rule for the integral of '4 / (1 + x^2) over [0, 1]'.
// The calculation is run twice and prints the results:
//   1. serial   - rank 0 alone, single-threaded
//   2. parallel - every rank, every OpenMP thread

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Number of rectangles across all ranks, more steps means more accuracy.
// Overridable on the command line: mpirun ... /path/to/binary/pi-estimation 20000000000
static const long long DEFAULT_STEPS = 5000000000LL;

int main(int argc, char **argv) {
    // MPI_THREAD_FUNNELED: Multi-threaded process, but only the thread running `MPI_Init_thread` makes MPI calls.
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED) {
        fprintf(stderr, "MPI does not provide the required threading level\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char host[MPI_MAX_PROCESSOR_NAME];
    int host_len;
    MPI_Get_processor_name(host, &host_len);

    const int threads = omp_get_max_threads();

    // mpirun passes the same arguments to every rank.
    long long steps = DEFAULT_STEPS;
    if (argc > 1) {
        long long requested = atoll(argv[1]);
        if (requested > 0) {
            steps = requested;
        }
    }
    const double width = 1.0 / (double)steps;

    // 1. Serial calculation
    double serial_pi = 0.0, serial_time = 0.0;
    if (rank == 0) {
        printf("[serial]   rank 0 on %s, 1 thread\n", host);
        double start = MPI_Wtime();
        double sum = 0.0;
        for (long long i = 0; i < steps; i++) {
            double x = ((double)i + 0.5) * width;
            sum += 4.0 / (1.0 + x * x);
        }
        serial_pi = sum * width;
        serial_time = MPI_Wtime() - start;
    }

    // 2. Parallel calculation
    // The barrier lines the ranks up to properly measure the parallel run time
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    double local_sum = 0.0;
    // Each rank handles a subset of the steps (i = rank, rank+size, ...).
    // Within a rank, OpenMP splits that subset across threads and reduces the partial sums back together.
    #pragma omp parallel reduction(+ : local_sum)
    {
        #pragma omp master
        printf("[parallel] rank %d/%d on %s using %d OpenMP thread(s)\n",
               rank, size, host, omp_get_num_threads());

        #pragma omp for
        for (long long i = rank; i < steps; i += size) {
            double x = ((double)i + 0.5) * width;
            local_sum += 4.0 / (1.0 + x * x);
        }
    }

    // Combine every rank's partial sum on rank 0.
    double parallel_pi = 0.0;
    MPI_Reduce(&local_sum, &parallel_pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    parallel_pi *= width;
    double parallel_time = MPI_Wtime() - start;

    if (rank == 0) {
        printf("\n");
        printf("steps:    %lld\n", steps);
        printf("serial:   pi ~= %.15f  in %.3f s\n", serial_pi, serial_time);
        printf("parallel: pi ~= %.15f  in %.3f s\n", parallel_pi, parallel_time);
        printf("speedup:  %.2fx  (%d rank(s) x %d thread(s))\n",
               serial_time / parallel_time, size, threads);
    }

    MPI_Finalize();
    return 0;
}
