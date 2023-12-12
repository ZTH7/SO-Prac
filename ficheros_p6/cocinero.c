#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include "datos.h"

#define M 10
#define SHARED_MEM_NAME "/caldero_shared_mem"
#define COOK_SEM_NAME "/cook_semaphore"
#define SAV_SEM_NAME "/savage_semaphore"
#define MTX_NAME "/mutex"


volatile int finish = 0;
struct data *shared_mem = NULL;
sem_t *cook_queue = NULL;
sem_t *sav_queue = NULL;
sem_t *sem_mtx = NULL;
int shm_fd;

void handler(int signo) {
    finish = 1;
    // Cleanup
    munmap(shared_mem, sizeof(struct data));
    close(shm_fd);
    sem_close(cook_queue);
    sem_close(sav_queue);
    sem_close(sem_mtx);
    shm_unlink(MTX_NAME);
    sem_unlink(COOK_SEM_NAME);
    sem_unlink(SAV_SEM_NAME);
    shm_unlink(SHARED_MEM_NAME);
}

void putServingsInPot(int m, sem_t *cook_queue, sem_t *sav_queue, sem_t *sem_mtx, struct data *shared_mem)
{
    sem_wait(sem_mtx); /*lock*/

    while (shared_mem->servings > 0) {
        /*cond wait*/
        shared_mem->cook_waiting++;
        sem_post(sem_mtx);
        sem_wait(cook_queue);
        sem_wait(sem_mtx);
    }

    printf("Cocinero: cooking %d meals\n", m);
    shared_mem->servings = m;

    /*cond broadcast*/
    while (shared_mem->nr_sav_waiting > 0) {
        sem_post(sav_queue);
        shared_mem->nr_sav_waiting--;
    }

    sem_post(sem_mtx); /*unlock*/
}

void cook(sem_t *cook_queue, sem_t *sav_queue, sem_t *sem_mtx, struct data *shared_mem)
{
    while (!finish) {
        putServingsInPot(M, cook_queue, sav_queue, sem_mtx, shared_mem);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, handler);
	signal(SIGTERM, handler);

    // Shared memory setup
    shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(struct data));
    shared_mem = mmap(0, sizeof(struct data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shared_mem->servings = 0;
    shared_mem->cook_waiting = 0;
    shared_mem->nr_sav_waiting = 0;

    // Semaphore setup
    cook_queue = sem_open(COOK_SEM_NAME, O_CREAT, 0666, 0); /* Initialize with 0 */
    sav_queue = sem_open(SAV_SEM_NAME, O_CREAT, 0666, 0); /* Initialize with 0 */
    sem_mtx = sem_open(MTX_NAME, O_CREAT, 0666, 1); /* Initialize with 1 */

    if (cook_queue == SEM_FAILED || sav_queue == SEM_FAILED || sem_mtx == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Main loop
    cook(cook_queue, sav_queue, sem_mtx, shared_mem);

    return 0;
}
