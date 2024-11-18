#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define SHARED_MEM_NAME "/shared_mem_example"
#define SEMAPHORE_NAME "/semaphore_example"
#define MEM_SIZE sizeof(int)

int main()
{
    int fd, *counter;
    sem_t *sem;

    // Create and open shared memory
    fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("Failed to create shared memory");
        exit(EXIT_FAILURE);
    }

    // Resize the shared memory
    ftruncate(fd, MEM_SIZE);

    // Map shared memory
    counter = (int *)mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (counter == MAP_FAILED)
    {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }

    // Initialize counter to 0
    *counter = 0;

    // Create and initialize semaphore
    sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED)
    {
        perror("Failed to create semaphore");
        exit(EXIT_FAILURE);
    }

    // Increment counter in a loop
    for (int i = 0; i < 10; i++)
    {
        // Wait on semaphore
        sem_wait(sem);

        // Increment counter
        (*counter)++;
        printf("Server: Counter incremented to %d\n", *counter);

        // Post (release) semaphore
        sem_post(sem);

        sleep(1); // Sleep for a while to simulate processing
    }

    // Cleanup
    munmap(counter, MEM_SIZE);
    close(fd);
    sem_close(sem);
    sem_unlink(SEMAPHORE_NAME);
    shm_unlink(SHARED_MEM_NAME);

    return 0;
}


