// parallel_sum_shm.c
// Compile: gcc parallel_sum_shm.c -o parallel_sum_shm
// Run: ./parallel_sum_shm

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

int main() {
    int n, num_children;

    // 1. Input: Array Size
    printf("Enter number of array elements: ");
    if (scanf("%d", &n) != 1 || n < 0) {
        fprintf(stderr, "Invalid number of elements.\n");
        return 1;
    }

    // 2. Allocation: Local Array
    // We allocate 1 byte if n=0 to ensure malloc returns a valid pointer
    int *arr = (int *)malloc(sizeof(int) * (n > 0 ? n : 1));
    if (!arr) {
        perror("malloc");
        return 1;
    }

    printf("Enter %d integer elements (space separated):\n", n);
    for (int i = 0; i < n; ++i) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "Invalid input.\n");
            free(arr);
            return 1;
        }
    }

    // 3. Input: Number of Processes
    printf("Enter number of child processes: ");
    if (scanf("%d", &num_children) != 1 || num_children <= 0) {
        fprintf(stderr, "Invalid number of children.\n");
        free(arr);
        return 1;
    }

    // 4. Setup: Shared Memory for Partial Sums
    // We need one slot (long) per child
    size_t shm_bytes = sizeof(long) * num_children;
    int shmid = shmget(IPC_PRIVATE, shm_bytes, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        free(arr);
        return 1;
    }

    // Attach shared memory to initialize
    long *partial_sums = (long *) shmat(shmid, NULL, 0);
    if (partial_sums == (void *) -1) {
        perror("shmat (parent)");
        shmctl(shmid, IPC_RMID, NULL);
        free(arr);
        return 1;
    }

    // Initialize slots to 0
    for (int i = 0; i < num_children; ++i) {
        partial_sums[i] = 0;
    }

    // Array to keep track of child PIDs
    pid_t *child_pids = (pid_t *)malloc(sizeof(pid_t) * num_children);
    if (!child_pids) {
        perror("malloc child_pids");
        shmdt(partial_sums);
        shmctl(shmid, IPC_RMID, NULL);
        free(arr);
        return 1;
    }

    // 5. Execution: Forking Children
    int base = n / num_children;
    int rem = n % num_children;
    int current_start = 0; // Tracks the starting index for the next child

    for (int i = 0; i < num_children; ++i) {
        // Calculate the length for this specific child
        // First 'rem' children get one extra element
        int length = base + (i < rem ? 1 : 0);

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            // Cleanup: wait for already created children
            for (int k = 0; k < i; ++k) wait(NULL);
            free(child_pids);
            shmdt(partial_sums);
            shmctl(shmid, IPC_RMID, NULL);
            free(arr);
            return 1;
        } else if (pid == 0) {
            // --- CHILD PROCESS ---
            
            // Child inherits 'arr' (copy-on-write) and loop variables ('current_start', 'length')
            
            // Attach shared memory in child
            long *ps = (long *) shmat(shmid, NULL, 0);
            if (ps == (void *) -1) {
                perror("shmat (child)");
                _exit(1); // Use _exit in child
            }

            long local_sum = 0;
            for (int j = 0; j < length; ++j) {
                local_sum += arr[current_start + j];
            }

            // Store result in the correct slot
            ps[i] = local_sum;

            printf("Child PID=%d (Index %d) sum = %ld (Range: %d to %d)\n",
                   getpid(), i, local_sum,
                   (length > 0 ? current_start : -1),
                   (length > 0 ? current_start + length - 1 : -1));

            // Detach and exit
            shmdt(ps);
            _exit(0);
        } else {
            // --- PARENT PROCESS ---
            child_pids[i] = pid;
            
            // Update start index for the NEXT child
            current_start += length;
        }
    }

    // 6. Synchronization: Wait for all children
    for (int i = 0; i < num_children; ++i) {
        int status;
        waitpid(child_pids[i], &status, 0);
        
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Warning: Child %d (PID %d) failed.\n", i, child_pids[i]);
        }
    }

    // 7. Aggregation
    long final_sum = 0;
    for (int i = 0; i < num_children; ++i) {
        final_sum += partial_sums[i];
    }

    printf("\nParent PID=%d\n", getpid());
    printf("Final aggregated sum = %ld\n", final_sum);

    // 8. Cleanup
    shmdt(partial_sums);            // Detach
    shmctl(shmid, IPC_RMID, NULL);  // Destroy Shared Memory
    free(arr);
    free(child_pids);

    return 0;
}