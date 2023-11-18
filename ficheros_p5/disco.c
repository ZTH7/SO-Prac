#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

int client_num = 0, vip_query = 0;
pthread_mutex_t mutex;
pthread_cond_t full;

struct client_info {
	int id;
	int isvip;
};

void enter_normal_client(int id)
{
	pthread_mutex_lock(&mutex);
	
	while (client_num >= CAPACITY || vip_query > 0)
		pthread_cond_wait(&full, &mutex);

    printf("Client %d (not vip) enter disco\n", id);
	client_num++;

	pthread_mutex_unlock(&mutex);
}

void enter_vip_client(int id)
{
	pthread_mutex_lock(&mutex);

	while (client_num >= CAPACITY) {
		vip_query++;
		pthread_cond_wait(&full, &mutex);
	}

    printf("Client %d (vip) enter disco\n", id);
	client_num++;
	vip_query--;

	pthread_mutex_unlock(&mutex);
}

void dance(int id, int isvip)
{
	printf("Client %d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	pthread_mutex_lock(&mutex);
	
	printf("Client %d (%s) exit disco\n", id, VIPSTR(isvip));
	client_num--;

	pthread_cond_signal(&full);
	pthread_mutex_unlock(&mutex);
}

void *client(void *arg)
{
	struct client_info* info = (struct client_info *)arg;
	int id = info->id, isvip = info->isvip;
	if ( isvip )
        enter_vip_client(id);
    else
        enter_normal_client(id);
    dance(id, isvip);
    disco_exit(id, isvip);

	free(arg);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	FILE* file;
	if ((file = fopen(argv[1], "r")) == NULL)
		printf("The input file %s could not be opened",argv[1]);

	int num;
	fscanf(file, "%d", &num);

	pthread_t *threads = malloc(num * sizeof(pthread_t));
	pthread_cond_init(&full, NULL);
	pthread_mutex_init(&mutex, NULL);

	for (int i = 0; i < num; i++)
    {
        struct client_info* info = malloc(sizeof(struct client_info));

        fscanf(file, "%d", &info->isvip);
        info->id = i;

        pthread_create(&threads[i], NULL, client, info);
    }

	for (int i = 0; i < num; i++) pthread_join(threads[i], NULL);

	free(threads);
	pthread_cond_destroy(&full);
	pthread_mutex_destroy(&mutex);

	return 0;
}