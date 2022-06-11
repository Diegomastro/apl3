#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>

int main() {
	const char *sem_cantJugadores_name = "cantJugadores";
    sem_t* sem_cantJugadores = sem_open(sem_cantJugadores_name, 0);
    sem_post(sem_cantJugadores);
    int id;
    sem_getvalue(sem_cantJugadores, &id);

    char *sem_turno1_name = "turno1";
    char *sem_turno2_name = "turno2";
    char *sem_turno3_name = "turno3";

    char *sem_names[] = {sem_turno1_name, sem_turno2_name, sem_turno3_name};

   
    sem_t* sem_turno = sem_open(sem_names[id-1], 0);

	while (1) {
		puts("Espera tu turno..");
		sem_wait(sem_turno);
	}



    return 0;
}
