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

    //agarrar pedazo de memoria segun su id

    //strcat("jug_",id+'0');
    return 0;
}
