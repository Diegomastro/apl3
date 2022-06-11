#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#define MAX_PALABRA 21

const char PALABRA_INICIAL[MAX_PALABRA] = "********************";

char* crearMemoriaCantJugadores();
char* crearMemoriaJugadores();
char* conseguirPalabra();
char* crearMemoriaJugador(char* path, char id);
    

int main() {
    const char *sem_cantJugadores_name = "cantJugadores";
    sem_t* sem_cantJugadores = sem_open(sem_cantJugadores_name, O_CREAT, 0600, 0);
    int cantJugadores;
    sem_getvalue(sem_cantJugadores, &cantJugadores);


    //char* vidasJugadores = crearMemoriaJugadores();
    int numJugadores;
    printf("%d", cantJugadores);

    printf("Ingrese el numero de jugadores\n");
    scanf("%d", &numJugadores);
    char estanTodos = 0;
    puts("esperando que se conecten todos los jugadores");
    while (numJugadores != cantJugadores) {
            sleep(2);
            sem_getvalue(sem_cantJugadores, &cantJugadores);
            printf("jugadores: %d \n", cantJugadores);
    } //esperamos por la cantidad de jugadores
    // PALABRA A ADIVINAR, despues habra que poner la logica para buscarlas de un archivo
    char palabra[] = "palabra";
    char* jugador1 = crearMemoriaJugador("jug_1", 'X');
    char* jugador2 = crearMemoriaJugador("jug_2", 'Y');
    char* jugador3 = crearMemoriaJugador("jug_3", 'Z');
    char* jugadores[] = {
        crearMemoriaJugador("jug_1", 'X'),
        crearMemoriaJugador("jug_2", 'Y'),
        crearMemoriaJugador("jug_3", 'Z')
    };
    return 0;
}



char* crearMemoriaJugador(char* path, char id) {
    size_t len = MAX_PALABRA; // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok(path, id);
    shmid = shmget(key, len, IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    memcpy(addr, PALABRA_INICIAL, len);
    return addr;

}

char* crearMemoriaJugadores() {
    size_t len = sizeof(char) * 3;
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("vidas_jugadores", 'B');
    shmid = shmget(key, len, IPC_CREAT);
    addr = shmat(shmid, NULL, 0);

    char vidasIniciales[] = {6 ,6 ,6};

    memcpy(addr, vidasIniciales, len);    

    return addr;
}