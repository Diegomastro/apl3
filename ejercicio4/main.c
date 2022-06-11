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

int todosPierden(char* vidas, int numJugadores);
int hayGanador(char* jugadores[], char* palabra);

int main() {
    char *sem_cantJugadores_name = "cantJugadores";
    char *sem_partidaTerminada_name = "partidaTerminada";
    char *sem_turno1_name = "turno1";
    char *sem_turno2_name = "turno2";
    char *sem_turno3_name = "turno3";
    char *sem_letraMandada_name = "letraMandada";
    char *sem_names[] = {sem_turno1_name, sem_turno2_name, sem_turno3_name};

    sem_unlink(sem_cantJugadores_name);
    sem_unlink(sem_turno1_name);
    sem_unlink(sem_turno2_name);
    sem_unlink(sem_turno3_name);

    sem_t* sem_cantJugadores = sem_open(sem_cantJugadores_name, O_CREAT, 0600, 0);
    sem_t* sem_partidaTerminada = sem_open(sem_partidaTerminada_name, O_CREAT, 0600, 0);
    sem_t* sem_letraMandada = sem_open(sem_letraMandada_name, O_CREAT, 0600, 0);
    sem_t* sem_turno1 = sem_open(sem_turno1_name, O_CREAT, 0600, 0);
    sem_t* sem_turno2 = sem_open(sem_turno2_name, O_CREAT, 0600, 0);
    sem_t* sem_turno3 = sem_open(sem_turno3_name, O_CREAT, 0600, 0);
    
    sem_t* sem_turnos[] = {sem_turno1, sem_turno2, sem_turno3};
    
    int cantJugadores;
    sem_getvalue(sem_cantJugadores, &cantJugadores);
    int numJugadores;
    char* vidasJugadores = crearMemoriaJugadores();

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
    char palabraX[] = "*******";
    char* jugadores[] = {
        crearMemoriaJugador("./jug_1", 'X'),
        crearMemoriaJugador("./jug_2", 'Y'),
        crearMemoriaJugador("./jug_3", 'Z')
    };
    int ganador = -1;
    int turno = 0;
    
    crearPalabraJuego(palabra, palabraJuego);

    while ((ganador = hayGanador(jugadores, palabra)) == -1 && !todosPierden(vidasJugadores, numJugadores)) {
        printf("Turno del jugador %d", turno+1);
        sem_post(sem_turnos[turno]);
        sem_wait(sem_letraMandada);//semaforo señalizando que se mandó la letra
        ++turno;
        turno %= cantJugadores;  // va de 0 a 2
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
    key_t key = ftok("./vidas_jugadores", 'B');
    shmid = shmget(key, len, IPC_CREAT);
    addr = shmat(shmid, NULL, 0);

    char vidasIniciales[] = {6 ,6 ,6};

    memcpy(addr, vidasIniciales, len);    
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


