#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#define MAX_PALABRA 8
#define CANT_PALABRAS 14
#define MAX_JUGADORES 3

const char PALABRA_INICIAL[MAX_PALABRA] = "*******";
// variables globales
int PALABRA_DE_JUEGO;
int PUNTAJES;
int VIDAS;
int JUG_1;
int JUG_2;
int JUG_3;
int RESULTADO;

char* crearMemoriaCantJugadores();
char* crearMemoriaJugadores();
char* conseguirPalabra();
char* crearMemoriaJugador(char* path, char id, int *shmid);
int todosPierden(char* vidas, int numJugadores);
int hayGanador(char* jugadores[], char* palabra);
int maxIndex(int arr[], int size);
void finalPartida(int puntajes[], int size, sem_t* sem_resultado);
void mostrarEstado(char*, int[],int);
int* getMemoriaPuntajeGanado();
int todosGanan(char* jugadores[], int numJugadores);
void darResultadoJugador(int* ganador, sem_t* sem_resultado);
void registrarSeniales();
void leerPalabra(char* buffer);
void checkHelp(int argc, char const* argv[]);
void vaciarMemoria();
void signal_sigterm(int signum);

int main(int argc, char const* argv[]) {
    if (argc > 1) {
        checkHelp(argc, argv);
    }
    registrarSeniales();

    int cantJugadores;
    int jugadoresTerminados;

    printf("Ingrese el numero de jugadores\n");
    int numJugadores = getchar() - '0';

    if (numJugadores < 0 || numJugadores > 3) {
        puts("Ups! la cantidad de jugadores tiene que ser un numero entre 1 y 3");
        exit(1);
    }

    int puntajes[] = {0,0,0};
    char* vidasJugadores = crearMemoriaJugadores();

    char palabra[MAX_PALABRA];
    leerPalabra(palabra);
    fflush(stdout);
    int j1,j2,j3;
    char* jugadores[] = {
        crearMemoriaJugador("./jug_1", 'X', &j1),
        crearMemoriaJugador("./jug_2", 'X', &j2),
        crearMemoriaJugador("./jug_3", 'X', &j3)
    };
    JUG_1 = j1;
    JUG_2 = j2;
    JUG_3 = j3;

    int ganador = -1;
    int turno = 0;
    int* puntajeDeRonda = getMemoriaPuntajeGanado();

    

    while (1) {

        char *sem_cantJugadores_name = "cantJugadores";
        char *sem_partidaTerminada_name = "partidaTerminada";
        char *sem_turno1_name = "turno1";
        char *sem_turno2_name = "turno2";
        char *sem_turno3_name = "turno3";
        char *sem_letraMandada_name = "letraMandada";
        char *sem_names[] = {sem_turno1_name, sem_turno2_name, sem_turno3_name};
        char *sem_jugadoresTerminados_name = "JugadoresTerminados";
        char* sem_resultado_name = "resultadoPartida";
        char* sem_serverExists_name = "serverSem";

        sem_unlink(sem_cantJugadores_name);
        sem_unlink(sem_turno1_name);
        sem_unlink(sem_turno2_name);
        sem_unlink(sem_turno3_name);
        sem_unlink(sem_partidaTerminada_name);
        sem_unlink(sem_letraMandada_name);
        sem_unlink(sem_jugadoresTerminados_name);
        sem_unlink(sem_resultado_name);
        sem_unlink(sem_serverExists_name);

        sem_t* sem_cantJugadores = sem_open(sem_cantJugadores_name, O_CREAT, 0600, 0);
        sem_t* sem_partidaTerminada = sem_open(sem_partidaTerminada_name, O_CREAT, 0600, 0);
        sem_t* sem_letraMandada = sem_open(sem_letraMandada_name, O_CREAT, 0600, 0);
        sem_t* sem_turno1 = sem_open(sem_turno1_name, O_CREAT, 0600, 0);
        sem_t* sem_turno2 = sem_open(sem_turno2_name, O_CREAT, 0600, 0);
        sem_t* sem_turno3 = sem_open(sem_turno3_name, O_CREAT, 0600, 0);
        sem_t* sem_jugadoresTerminados = sem_open(sem_jugadoresTerminados_name, O_CREAT, 0600, 0);
        sem_t* sem_resultado = sem_open(sem_resultado_name, O_CREAT, 0600, 0);
        sem_t* sem_serverExists = sem_open(sem_serverExists_name, O_CREAT, 0600, 0);

        // como ya existe, se lo mando
        sem_post(sem_serverExists);

        sem_t* sem_turnos[] = {sem_turno1, sem_turno2, sem_turno3};
        sem_getvalue(sem_cantJugadores, &cantJugadores);
        sem_getvalue(sem_jugadoresTerminados, &jugadoresTerminados);

        char estanTodos = 0;
        puts("esperando que se conecten todos los jugadores");
        while (numJugadores != cantJugadores) {
            sleep(2);
            sem_getvalue(sem_cantJugadores, &cantJugadores);
            printf("jugadores: %d \n", cantJugadores);
        } //esperamos por la cantidad de jugadores
        // PALABRA A ADIVINAR, despues habra que poner la logica para buscarlas de un archivo

        sem_getvalue(sem_jugadoresTerminados, &jugadoresTerminados);
        while (jugadoresTerminados != cantJugadores) {
            printf("Turno del jugador %d\n", turno+1);
            fflush(stdout);
            sem_post(sem_turnos[turno]);
            sem_wait(sem_letraMandada);//semaforo señalizando que se mandó la letra
            int puntajeGanado = *puntajeDeRonda;
            puntajes[turno] += puntajeGanado;

            if (puntajeGanado < 0) {
                --vidasJugadores[turno];
            }
            ++turno;
            turno %= cantJugadores;
            //system("clear");
            sem_getvalue(sem_jugadoresTerminados, &jugadoresTerminados);
            printf("jugadores terminados: %d\n", jugadoresTerminados);
            fflush(stdout);
            mostrarEstado(vidasJugadores, puntajes, numJugadores);
        }

        sem_post(sem_partidaTerminada);
        for (int i = 0; i < 3; ++i) {
            sem_post(sem_turnos[i]);
        }

    finalPartida(puntajes, numJugadores, sem_resultado);

    printf("Fin de la partida! la palabra a adivinar era: %s\n", palabra);
    }
    return 0;
}

void checkHelp(int argc, char const* argv[]) {
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--help")) {
            puts("Servidor del Ahorcado");
            printf("Sintaxis: %s", argv[0]);
            exit(0);
        }
    }
}

void leerPalabra(char* buffer) {
    FILE* textfile;
    srand(time(NULL));
    int random = rand();
    int indiceRandom = random % CANT_PALABRAS;
    int indice = 0;
    textfile = fopen("palabras.txt", "r");

    for (int i = 0; i <= indiceRandom; ++i) {
        fgets(buffer, MAX_PALABRA + 1, textfile);
    }

    char* tmp = buffer;

    while (*tmp) {
        if (*tmp == '\n') {
            *tmp = '\0';
        }
        ++tmp;
    }

    fclose(textfile);

    size_t len = MAX_PALABRA;
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("./palabraDeJuego", 'X');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    PALABRA_DE_JUEGO = shmid;

    memcpy(addr, buffer, len);
}

void signal_sigterm(int signum) {
    vaciarMemoria();
    // cerrar semaforos
    char *sem_cantJugadores_name = "cantJugadores";
    char *sem_partidaTerminada_name = "partidaTerminada";
    char *sem_turno1_name = "turno1";
    char *sem_turno2_name = "turno2";
    char *sem_turno3_name = "turno3";
    char *sem_letraMandada_name = "letraMandada";
    char *sem_names[] = {sem_turno1_name, sem_turno2_name, sem_turno3_name};
    char *sem_jugadoresTerminados_name = "JugadoresTerminados";
    char* sem_resultado_name = "resultadoPartida";
    char* sem_serverExists_name = "serverSem";

    sem_unlink(sem_cantJugadores_name);
    sem_unlink(sem_partidaTerminada_name);
    sem_unlink(sem_letraMandada_name);
    sem_unlink(sem_jugadoresTerminados_name);
    sem_unlink(sem_resultado_name);
    sem_unlink(sem_serverExists_name);

    sem_t* sem_turno1 = sem_open(sem_turno1_name, 0);
    sem_t* sem_turno2 = sem_open(sem_turno2_name, 0);
    sem_t* sem_turno3 = sem_open(sem_turno3_name, 0);
    sem_post(sem_turno1);
    sem_post(sem_turno2);
    sem_post(sem_turno3);
    //sleep(1); //para que puedan salir los jugadores

    // limpiamos shared memory
    shmctl(JUG_1, IPC_RMID, NULL);
    shmctl(JUG_2, IPC_RMID, NULL);
    shmctl(JUG_3, IPC_RMID, NULL);
    shmctl(RESULTADO, IPC_RMID, NULL);
    shmctl(VIDAS, IPC_RMID, NULL);
    shmctl(PALABRA_DE_JUEGO, IPC_RMID, NULL);
    shmctl(PUNTAJES, IPC_RMID, NULL);
    sem_unlink(sem_turno1_name);
    sem_unlink(sem_turno2_name);
    sem_unlink(sem_turno3_name);
    sem_unlink(sem_turno1_name);
    sem_unlink(sem_turno2_name);
    sem_unlink(sem_turno3_name);


    //system("clear");
    puts("Usted decidio finalizar el programa, saludos!");
    exit(0);
}

void registrarSeniales() {
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, signal_sigterm);
}

int todosGanan(char* jugadores[], int numJugadores) {
    printf("numJugadores = %d\n", numJugadores);
    for (int i = 0; i < numJugadores; ++i) {
        puts(jugadores[i]);
        char* palabraActual = jugadores[i];

        while (*palabraActual) {
            if (*palabraActual == '*') {
                return 0;
            }
        }
    }
    puts("Termino");
    return 1;
}

int* getMemoriaPuntajeGanado() {
    size_t len = sizeof(int);
    int shmid = 0;
    int* addr = NULL;
    key_t key = ftok("./puntajes", 'B');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    PUNTAJES = shmid;

    return addr;
}

int todosPierden(char* vidas, int numJugadores) {
    for (int i = 0; i < numJugadores; ++i) {
        if (*(vidas+i) > 0) {
            return 0;
        }
    }
    return 1;
}

char* crearMemoriaJugadores() {
    size_t len = sizeof(char) * 3;
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("./vidas_jugadores", 'B');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    VIDAS = shmid;
    char vidasIniciales[] = {6 ,6 ,6};

    memcpy(addr, vidasIniciales, len);
}

char* crearMemoriaJugador(char* path, char id, int *idCopy) {
    size_t len = MAX_PALABRA; // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok(path, id);
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    memcpy(addr, PALABRA_INICIAL, len);
    *idCopy = shmid;
    return addr;
}

void finalPartida(int puntajes[], int size, sem_t* sem_resultado) {
    // MARQUITO completame esto
    // ademas de printearlo, que cree una string con esto y se lo mande a cada jugador por memoria compartida
    //system("clear");
    for (int i = 0; i < size; ++i) {
        printf("Jugador %d: %d Pts.", i+1, puntajes[i]);
    }
    puts("");
    int ganador = maxIndex(puntajes, size)+1;

    printf("Ganador: jugador %d\n", ganador);

    darResultadoJugador(&ganador, sem_resultado);
}

void vaciarMemoria() {
    return;
}

void darResultadoJugador(int* ganador, sem_t* sem_resultado) {
    size_t len = sizeof(int); // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("./resultado", 'X');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    memcpy(addr, ganador, sizeof(int));
    RESULTADO = shmid;
    for (int i = 0; i < MAX_JUGADORES; ++i) {
        sem_post(sem_resultado);
    }
}

int maxIndex(int arr[], int size) {
    int maxNum = -99999;
    int index = 0;
    for (int i = 0; i < size; ++i) {
        if (arr[i] > maxNum) {
            maxNum = arr[i];
            index = i;
        }
    }
    return index;
}

void mostrarEstado(char* vidas, int puntajes[], int cantJugadores) {
    for (int i = 0; i < cantJugadores; ++i) {
        printf("Estado del jugador %d:\nvidas: %d\tpuntaje: %d\n\n", i+1, (int)vidas[i], puntajes[i]);
        fflush(stdout);
    }
}
