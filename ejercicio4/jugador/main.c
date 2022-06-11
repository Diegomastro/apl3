#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#define MAX_PALABRA 8

char* getPalabraDeJuego();
void finalPartida();
void escribirPuntaje(int*);

int main() {
    const char *sem_cantJugadores_name = "cantJugadores";
    const char *sem_letraMandada_name = "letraMandada";
    sem_t* sem_letraMandada = sem_open(sem_letraMandada_name, 0);
    sem_t* sem_cantJugadores = sem_open(sem_cantJugadores_name, 0);
    sem_post(sem_cantJugadores);
    int id;
    sem_getvalue(sem_cantJugadores, &id);
    int vidas = 6;
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

    char* palabraDeJuego = getPalabraDeJuego();
    int puntajeRonda = 0;

    while (!partidaTerminada) {
        if (vidas <= 0) {
            escribirPuntaje(0);
            sem_getvalue(sem_partidaTerminada, &partidaTerminada);
            if (partidaTerminada) {
                break;
            }
            continue;
        }
        sem_wait(sem_turno);
        sem_getvalue(sem_partidaTerminada, &partidaTerminada);
        if (partidaTerminada) {
            break;
        }
        puts("Es tu turno! tu palabra es:");
        puts(palabraJugadorX);
        puts("Es tu turno! ingrese un caracter");

        char intento;
        scanf("%c", &intento);

        char* tmp = palabraDeJuego;
        int primera = 1;
        int index = 0;
        while (*tmp) {
            if (*tmp == intento) {
                if (primera) {
                    system("clear");
                    puts("correcto! tu letra figura en la palabra");
                    primera = 0;
                    puntajeRonda = 2;
                }
                palabraJugadorX[index] = intento;
            }
            ++index;
            ++tmp;
        }

        if (!primera) {
            --vidas;
            system("clear");
            puts("Error: la letra no se encuentra en la palabra");
            puntajeRonda = -1;
        }

        escribirPuntaje(&puntajeRonda);

        sem_post(sem_letraMandada);
    }

}

char* getPalabraDeJuego() {
    /**
     * aca esta la logica para ver
     * cual es la palabra de juego y comparar
     */
    return "palabra";
}

void finalPartida() {
    //tendria que recibir la string entera del server
}

void escribirPuntaje(int* puntaje) {
    size_t len = sizeof(int);
    int shmid = 0;
    int* addr = NULL;
    key_t key = ftok("./puntajes", 'B');
    shmid = shmget(key, len, IPC_CREAT);
    addr = shmat(shmid, NULL, 0);

    memcpy(addr, puntaje, sizeof(int));
}
