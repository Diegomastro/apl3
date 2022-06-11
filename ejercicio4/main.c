#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string.h>
#include <stdlib.h>
#define MAX_PALABRA 21

const char PALABRA_INICIAL[MAX_PALABRA] = "********************";

char* crearMemoriaCantJugadores();
char* crearMemoriaJugadores();
char* conseguirPalabra();
char* crearMemoriaJugador(char* path, char id);
int todosPierden(char* vidas, int numJugadores);
int hayGanador(char* jugadores[], char* palabra);

int main() {
    char* cantJugadores = crearMemoriaCantJugadores(); 
    char* vidasJugadores = crearMemoriaJugadores();
    int numJugadores;
    printf("%d", *cantJugadores);
    printf("Ingrese el numero de jugadores\n");
    scanf("%d", &numJugadores);
    char estanTodos = 0;
    puts("esperando que se conecten todos los jugadores");
    while (numJugadores != *cantJugadores); //esperamos por la cantidad de jugadores
    // PALABRA A ADIVINAR, despues habra que poner la logica para buscarlas de un archivo
    char palabra[] = "palabra";
    char* jugadores[] = {
        crearMemoriaJugador("/home/marco/sisop-apl/apl3/ejercicio4/jug_1", 'X'),
        crearMemoriaJugador("/home/marco/sisop-apl/apl3/ejercicio4/jug_2", 'Y'),
        crearMemoriaJugador("/home/marco/sisop-apl/apl3/ejercicio4/jug_3", 'Z')
    };
    int ganador = -1;
    int turno = 0;
    while ((ganador = hayGanador(jugadores, palabra)) == -1 && !todoPierden(vidasJugadores, numJugadores)) {
        // empieza turno
        /**
         *  
         * 
         */
        /**
        subimos semaforo de que puede jugar el
        jugador turno+1
        */
        /** 
         * esperamos a que el proceso turno+1
         * suba el semaforo que ya termino
        */

        
        // fin de turno
       ++turno;
       turno %= 3;  // va de 0 a 2
    }

    return 0;
}

int todosPierden(char* vidas, int numJugadores) {
    for (int i = 0; i < numJugadores; ++i) {
        if (*(vidas+i) > 0) {
            return 0;
        }
    }

    return 1;
}

int hayGanador(char* jugadores[], char* palabra) {
    for (int i = 0; i < 3; ++i) {
       if (strcmp(jugadores[i], palabra) == 0) {
           return i;
       }
    }
    return -1;
}

char* crearMemoriaJugadores() {
    size_t len = sizeof(char) * 3;
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("/home/marco/sisop-apl/apl3/ejercicio4/vidas_jugadores", 'B');
    shmid = shmget(key, len, IPC_CREAT);
    addr = shmat(shmid, NULL, 0);

    char vidasIniciales[] = {6 ,6 ,6};

    memcpy(addr, vidasIniciales, len);    

    return addr;
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

char* crearMemoriaCantJugadores() {
    size_t len = sizeof(char);
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("/home/marco/sisop-apl/apl3/ejercicio4/cant_jugadores", 'A');
    
    shmid = shmget(key, len, IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    printf("%d\n", shmid);
    printf("%p\n", addr);
    char x = 0;
    memcpy(addr, &x, sizeof(char)); 
    return addr;
}
