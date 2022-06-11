#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#define MAX_PALABRA 21

int main() {
	const char *sem_cantJugadores_name = "cantJugadores";
    sem_t* sem_cantJugadores = sem_open(sem_cantJugadores_name, 0);
    sem_post(sem_cantJugadores);
    int id;
    sem_getvalue(sem_cantJugadores, &id);
    //agarrar pedazo de memoria segun su id
    char pathAMemoria[] = "jug_*";
    pathAMemoria[4] = id+'0';
    puts(pathAMemoria);
    size_t len = MAX_PALABRA; // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* palabraJugadorX = NULL;
    key_t key = ftok(pathAMemoria,id);
    shmid = shmget(key, len, IPC_CREAT);
    palabraJugadorX = shmat(shmid, NULL, 0);

    return 0;
}
