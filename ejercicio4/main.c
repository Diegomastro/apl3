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

char* crearMemoriaCantJugadores();
char* crearMemoriaJugadores();
char* conseguirPalabra();
char* crearMemoriaJugador(char* path, char id);
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

int main() {
    registrarSeniales();
    int puntajes[] = {0,0,0};
    char *sem_cantJugadores_name = "cantJugadores";
    char *sem_partidaTerminada_name = "partidaTerminada";
    char *sem_turno1_name = "turno1";
    char *sem_turno2_name = "turno2";
    char *sem_turno3_name = "turno3";
    char *sem_letraMandada_name = "letraMandada";
    char *sem_names[] = {sem_turno1_name, sem_turno2_name, sem_turno3_name};
    char *sem_jugadoresTerminados_name = "JugadoresTerminados";
    char* sem_resultado_name = "resultadoPartida";
    
    sem_unlink(sem_cantJugadores_name);
    sem_unlink(sem_turno1_name);
    sem_unlink(sem_turno2_name);
    sem_unlink(sem_turno3_name);
    sem_unlink(sem_partidaTerminada_name);
    sem_unlink(sem_letraMandada_name);
    sem_unlink(sem_jugadoresTerminados_name);
    sem_unlink(sem_resultado_name);

    sem_t* sem_cantJugadores = sem_open(sem_cantJugadores_name, O_CREAT, 0600, 0);
    sem_t* sem_partidaTerminada = sem_open(sem_partidaTerminada_name, O_CREAT, 0600, 0);
    sem_t* sem_letraMandada = sem_open(sem_letraMandada_name, O_CREAT, 0600, 0);
    sem_t* sem_turno1 = sem_open(sem_turno1_name, O_CREAT, 0600, 0);
    sem_t* sem_turno2 = sem_open(sem_turno2_name, O_CREAT, 0600, 0);
    sem_t* sem_turno3 = sem_open(sem_turno3_name, O_CREAT, 0600, 0);
    sem_t* sem_jugadoresTerminados = sem_open(sem_jugadoresTerminados_name, O_CREAT, 0600, 0);
    sem_t* sem_resultado = sem_open(sem_resultado_name, O_CREAT, 0600, 0);
    
    sem_t* sem_turnos[] = {sem_turno1, sem_turno2, sem_turno3};

    int cantJugadores;
    int jugadoresTerminados;

    sem_getvalue(sem_cantJugadores, &cantJugadores);
    sem_getvalue(sem_jugadoresTerminados, &jugadoresTerminados);

    int numJugadores = 0;
    char* vidasJugadores = crearMemoriaJugadores();

    char palabra[MAX_PALABRA]; 
    leerPalabra(palabra);
    char* jugadores[] = {
        crearMemoriaJugador("./jug_1", 'X'),
        crearMemoriaJugador("./jug_2", 'X'),
        crearMemoriaJugador("./jug_3", 'X')
    };
    int ganador = -1;
    int turno = 0;
    int* puntajeDeRonda = getMemoriaPuntajeGanado();

    printf("Ingrese el numero de jugadores\n");

    numJugadores = getchar() - '0'; 
    char estanTodos = 0;
    puts("esperando que se conecten todos los jugadores");
    while (numJugadores != cantJugadores) {
        sleep(2);
        sem_getvalue(sem_cantJugadores, &cantJugadores);
        printf("jugadores: %d \n", cantJugadores);
    } //esperamos por la cantidad de jugadores
    // PALABRA A ADIVINAR, despues habra que poner la logica para buscarlas de un archivo

    //while (!todosPierden(vidasJugadores, numJugadores) && !todosGanan(jugadores, numJugadores)) {
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
        system("clear");
        sem_getvalue(sem_jugadoresTerminados, &jugadoresTerminados);
        printf("jugadores terminados: %d\n", jugadoresTerminados);
        fflush(stdout);
        mostrarEstado(vidasJugadores, puntajes, numJugadores);
    }

    sem_post(sem_partidaTerminada); // CREO que no lo estoy usando, por las dudas igual queda
    for (int i = 0; i < 3; ++i) {
        puts("subiendo");
        sem_post(sem_turnos[i]);
    }

    finalPartida(puntajes, numJugadores, sem_resultado);
    return 0;
}

void leerPalabra(char* buffer) {
    FILE* textfile;
    srand(time(NULL));
    int random = rand();
    int indiceRandom = random % CANT_PALABRAS;
    int indice = 0;
    printf("random = %d, indiceRandom = %d\n", random, indiceRandom);
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

    memcpy(addr, buffer, len);
    puts("Buffer:");
    puts(buffer);
}

void signal_sigint(int signum) {
    system("clear");
    puts("Usted decidio finalizar el programa, saludos!");
    fflush(stdout);
    exit(0);
}


void signal_sigterm(int signum) {
    system("clear");
    puts("Usted decidio finalizar el programa, saludos!");
    exit(0);
}

void registrarSeniales() {
    signal(SIGINT, signal_sigint);
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

    char vidasIniciales[] = {6 ,6 ,6};

    memcpy(addr, vidasIniciales, len);    
}

char* crearMemoriaJugador(char* path, char id) {
    size_t len = MAX_PALABRA; // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok(path, id);
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    memcpy(addr, PALABRA_INICIAL, len);
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

    printf("Ganador: jugador %d", ganador);

    darResultadoJugador(&ganador, sem_resultado);
}

void darResultadoJugador(int* ganador, sem_t* sem_resultado) {
    size_t len = sizeof(int); // tam maximo de una palabra (sera una constante)
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("./resultado", 'X');
    shmid = shmget(key, len, 0666|IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    memcpy(addr, ganador, sizeof(int));

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
