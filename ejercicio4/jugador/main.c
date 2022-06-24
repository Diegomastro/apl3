#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX_PALABRA 8

char* getPalabraDeJuego();
void finalPartida();
void escribirPuntaje(int*);
int adivinoPalabra(char* palabraX);
int murio(int vidas);
int getResultadoFinal();
void checkHelp(int argc, char const* argv[]);
void checkServerExists();

int main(int argc, char const* argv[]) {
    checkHelp(argc, argv);
    const char *sem_cantJugadores_name = "cantJugadores";
    const char *sem_letraMandada_name = "letraMandada";
    const char *sem_jugadoresTerminados_name = "JugadoresTerminados";
    const char *sem_serverExists_name = "serverSem";

    sem_t* sem_serverExists = sem_open(sem_serverExists_name, 0);
    if (sem_serverExists == SEM_FAILED) {
        puts("Ups! no hay servidor de juego todavia");
        exit(1);
    }

    sem_t* sem_jugadoresTerminados = sem_open(sem_jugadoresTerminados_name, 0);
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
    char* sem_resultado_name = "resultadoPartida";


    sem_t* sem_turno = sem_open(sem_names[id-1], 0);
    sem_t* sem_partidaTerminada = sem_open(sem_partidaTerminada_name, 0);
    sem_t* sem_resultado = sem_open(sem_resultado_name, 0);
    int partidaTerminada = 0;

    sem_getvalue(sem_partidaTerminada, &partidaTerminada);

    char pathAMemoria[] = "../jug_*";
    pathAMemoria[7] = id+'0';

    size_t len = MAX_PALABRA; // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* palabraJugadorX = NULL;
    key_t key = ftok(pathAMemoria, 'X');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    palabraJugadorX = shmat(shmid, NULL, 0);
    fflush(stdout);

    char* palabraDeJuego = getPalabraDeJuego();
    int puntajeRonda = 0;
    int nuncaMando = 1;
    int cantJugadores;
    int jugadoresTerminados;

    while (!partidaTerminada) {
        checkServerExists();
        sem_wait(sem_turno);
        if (!adivinoPalabra(palabraJugadorX) && !murio(vidas)) {
            checkServerExists();
            sem_getvalue(sem_partidaTerminada, &partidaTerminada);
            if (partidaTerminada) {
                break;
            }
            puts("Es tu turno! tu palabra es:");
            puts(palabraJugadorX);
            puts("ingrese un caracter");
            fflush(stdout);

            char intento;
            scanf(" %c", &intento);
            getchar();
            char* tmp = palabraDeJuego;
            int primera = 1;
            int index = 0;
            while (*tmp) {
                if (*tmp == intento && palabraJugadorX[index] == '*') {
                    if (primera) {
                        puts("correcto! tu letra figura en la palabra");
                        fflush(stdout);
                        primera = 0;
                        puntajeRonda = 2;
                    }
                    palabraJugadorX[index] = intento;
                }
                ++index;
                ++tmp;
            }

            if (primera) {
                --vidas;
                puts("Error: la letra no se encuentra en la palabra");
                fflush(stdout);
                puntajeRonda = -1;
            }

            if (murio(vidas)) {
                puntajeRonda = 0;
                escribirPuntaje(&puntajeRonda);
                puts("Ahorcado! Esperando a que los demas jugadores terminen");
                fflush(stdout);
            } else {
                escribirPuntaje(&puntajeRonda);
            }

            if (adivinoPalabra(palabraJugadorX)) {
                puts("Adivinaste! Esperando a que los demas jugadores terminen");
                fflush(stdout);
            }

        } else { // si ya termino, sea pq murio o adivino la palabra

            if (nuncaMando) {
                checkServerExists();
                sem_post(sem_jugadoresTerminados); // mando que termino UNA SOLA VEZ
                nuncaMando = 0;
            }

            // si ya terminaron todos, salir del loop.
            checkServerExists();
            sem_getvalue(sem_cantJugadores, &cantJugadores);
            sem_getvalue(sem_jugadoresTerminados, &jugadoresTerminados);

            if (cantJugadores == jugadoresTerminados) {  // asqueroso esto eh
                checkServerExists();
                sem_post(sem_letraMandada);
                break;
            }


        }
        checkServerExists();
        sem_post(sem_letraMandada);
    }

    checkServerExists();
    sem_wait(sem_resultado);

    int resultado = getResultadoFinal();
    finalPartida(resultado, id);

    return 0;
}

void checkHelp(int argc, char const* argv[]) {
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            puts("Servidor del Ahorcado");
            printf("Sintaxis: %s", argv[0]);
            exit(0);
        }
    }
}

char* getPalabraDeJuego() {
    size_t len = MAX_PALABRA;
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("../palabraDeJuego", 'X');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);

    return addr;
}

int getResultadoFinal() {
    int res;
    size_t len = sizeof(int); // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("../resultado", 'X');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    memcpy(&res, addr, len);
    return res;
}

void finalPartida(int resultado, int id) {
    //MARQUITO completame esto -> listo rey
    // leer la memoria compartida del jugador y printearla
    printf("resultado: %d, id: %d\n", resultado, id);
    if (resultado != id) {
        printf("Has perdido, el ganador es el jugador %d, mejor suerte la proxima!", id);
        return;
    }
    printf("Felicidades jugador %d, eres el ganador!", id);
}

void escribirPuntaje(int* puntaje) {
    size_t len = sizeof(int);
    int shmid = 0;
    int* addr = NULL;
    key_t key = ftok("../puntajes", 'B');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);

    memcpy(addr, puntaje, sizeof(int));
}

int adivinoPalabra(char* palabraX) {
    while(*palabraX) {
        if (*palabraX == '*') {
            return 0;
        }
        ++palabraX;
    }

    return 1;
}

int murio(int vidas) {
    return vidas <= 0;
}

void checkServerExists() {
    const char *sem_serverExists_name = "serverSem";
    sem_t* sem_serverExists = sem_open(sem_serverExists_name, 0);
    if (sem_serverExists == SEM_FAILED) {
        system("clear");
        puts("El server se cerro");
        exit(1);
    }
}
