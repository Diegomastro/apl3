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

    char *sem_turno1_name = "turno1";
    char *sem_turno2_name = "turno2";
    char *sem_turno3_name = "turno3";
    char *sem_partidaTerminada_name = "partidaTerminada";
    char *sem_names[] = {sem_turno1_name, sem_turno2_name, sem_turno3_name};

   
    sem_t* sem_turno = sem_open(sem_names[id-1], 0);
    sem_t* sem_partidaTerminada = sem_open(sem_partidaTerminada_name, 0);
    int partidaTerminada = 0;

    sem_getvalue(sem_partidaTerminada, &partidaTerminada);

    char pathAMemoria[] = "jug_*";
    pathAMemoria[4] = id+'0';
    puts(pathAMemoria);
    size_t len = MAX_PALABRA; // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* palabraJugadorX = NULL;
    key_t key = ftok(pathAMemoria,id);
    shmid = shmget(key, len, IPC_CREAT);
    palabraJugadorX = shmat(shmid, NULL, 0);

}