#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include "datos.h"

#define NUMITER 15
#define SHARED_MEM_NAME "/caldero_shared_mem"
#define COOK_SEM_NAME "/cook_semaphore"
#define SAV_SEM_NAME "/savage_semaphore"
#define MTX_NAME "/mutex"

void eat(void)
{
    unsigned long id = (unsigned long) getpid();
    printf("Savage %lu eating\n", id);
    sleep(rand() % 3);
}

bool getServingsFromPot(sem_t *cook_queue, sem_t *sav_queue, sem_t *sem_mtx, struct data *shared_mem)
{
    sem_wait(sem_mtx); /*lock*/
    
    while (shared_mem->servings == 0) {
        /*signal*/
        if (shared_mem->cook_waiting > 0) {
            sem_post(cook_queue);
            (shared_mem->cook_waiting)--;
        }
         /*cond wait*/
        (shared_mem->nr_sav_waiting)++;
        sem_post(sem_mtx);
        sem_wait(sav_queue);
        sem_wait(sem_mtx);
    }

    (shared_mem->servings)--;
    printf("%d", shared_mem->servings);

    sem_post(sem_mtx); /*unlock*/

    return true;
}

void savages(sem_t *cook_queue, sem_t *sav_queue, sem_t *sem_mtx, struct data *shared_mem)
{
    for (int i = 0; i < NUMITER; i++) {
        if (getServingsFromPot(cook_queue, sav_queue, sem_mtx, shared_mem)) {
            eat();
        }
    }
}

int main(int argc, char *argv[])
{
    // Shared memory setup
    int shm_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    struct data *shared_mem = mmap(0, sizeof(struct data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Semaphore setup
    sem_t *cook_queue = sem_open(COOK_SEM_NAME, O_RDWR);
    sem_t *sav_queue = sem_open(SAV_SEM_NAME, O_RDWR);
    sem_t *sem_mtx = sem_open(MTX_NAME, O_RDWR);

    // Check if semaphore creation is successful
    if (cook_queue == SEM_FAILED || sav_queue == SEM_FAILED || sem_mtx == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Main loop
    savages(cook_queue, sav_queue, sem_mtx, shared_mem);

    // Cleanup
    munmap(shared_mem, sizeof(struct data));
    close(shm_fd);
    sem_close(cook_queue);
    sem_close(sav_queue);
    sem_close(sem_mtx);

    return 0;
}
