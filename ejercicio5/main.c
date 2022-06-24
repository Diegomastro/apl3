#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#define MAX_JUGADORES 3
#define ACIERTO 2
#define FALLO -1
#define INDICE_PUERTO 1
#define CANT_PARAMS 2
#define CANT_PALABRAS 14
#define MAX_PALABRA 8

void mostrarEstados(int puntajes[], int cantJugadores);
int procesarTurno(int idJugador, int socketId, char* palabraActual, char* palabraDeJuego);
int yaGano(char* cadena);
int getMaxIndex(int puntajes[], int cantJugadores);
void leerPalabra(char* buffer);
void signal_sigint(int signum);
void signal_sigterm(int signum);
void registrarSeniales();
void printHelpAndExit(char const* argv[]);
void checkHelp(int argc, char const* argv[]);

// GLOBAL VARIABLES
int SOCKET_SERVER_CONNECTION = -1;
int *SOCKETS_CLIENT_CONNECTIONS = NULL;
int SOCKETS_CLIENT_SIZE = 0;
int socketsJugadores[MAX_JUGADORES] = {0};
int cantJugadores;

int main(int argc, char const* argv[]) {
    if (argc < CANT_PARAMS) {
        printHelpAndExit(argv);
    }
    checkHelp(argc, argv);
    registrarSeniales();
    int puerto = atoi(argv[INDICE_PUERTO]);
    const int uno = 1;
    const int cero = 0;
    int server = socket(AF_INET, SOCK_STREAM, 0);
    SOCKET_SERVER_CONNECTION = server;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(puerto);
    socklen_t len = sizeof(address);
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 0;
    setsockopt(
      server,     // Socket descriptor
      SOL_SOCKET, // To manipulate options at the sockets API level
      SO_RCVTIMEO,// Specify the receiving or sending timeouts
      (const void *)(&t), // option values
      sizeof(t)
    );

    int conn = bind(server, (const struct sockaddr *) &address, (socklen_t) sizeof(address));
    if (conn < 0) {
        puts("Ups! ha habido un error, el puerto esta ocupado");
        exit(1);
    }
    listen(server, cantJugadores);
    int jugadoresConectados;

    puts("Ingrese la cantidad de jugadores:");
    scanf("%d", &cantJugadores);

    while (1) {
    int yaLeMandamosQueGano[] = {0,0,0};
    socketsJugadores[0] = 0;
    socketsJugadores[1] = 0;
    socketsJugadores[2] = 0;
    int vidasJugadores[] = {6, 6, 6};
    char palabrasJugadores[][8] = {
        "*******",
        "*******",
        "*******"
    };
    int puntajes[MAX_JUGADORES] = {0};

    char palabraDeJuego[MAX_PALABRA];
    leerPalabra(palabraDeJuego);
    puts(palabraDeJuego);


        jugadoresConectados = 0;

        // jugadores conectados == ingresados por teclado
        while (jugadoresConectados != cantJugadores) {
            printf("Jugadores conectados = %d\n", jugadoresConectados);
            int cliente = accept(server, (struct sockaddr *) &address, &len);
            socketsJugadores[jugadoresConectados++] = cliente;
            printf("clietne %d\n", cliente);
        }
        SOCKETS_CLIENT_CONNECTIONS = socketsJugadores;
        SOCKETS_CLIENT_SIZE = jugadoresConectados;
        // while de partida
        int cantTerminados = 0;
        int turno = 0;
        while (cantTerminados != cantJugadores) {
            if (vidasJugadores[turno] <= 0) {
                ++turno;
                turno %= cantJugadores;
                continue;
            }

            if (yaGano(palabrasJugadores[turno])) {
                if(!yaLeMandamosQueGano[turno]) {
                    cantTerminados++;
                    yaLeMandamosQueGano[turno] = 1;
                    send(socketsJugadores[turno], &uno, sizeof(int), 0);
                }
                ++turno;
                turno %= cantJugadores;
                continue;
            } else {
                send(socketsJugadores[turno], &cero, sizeof(int), 0);
            }

            printf("Turno jugador %d\n", turno+1);

            int resultado = procesarTurno(turno+1, socketsJugadores[turno], palabrasJugadores[turno], palabraDeJuego);
            puntajes[turno] += resultado;
            if (resultado == FALLO) {
                --vidasJugadores[turno];
            }

            if (vidasJugadores[turno] <= 0) {
                ++cantTerminados;
            }
            // incrementamos el turno
            ++turno;
            turno %= cantJugadores;

            mostrarEstados(puntajes, cantJugadores);
        }
        puts("Termino la partida!");
        fflush(stdout);

        char stringGanador[] = "Felicidades jugador *! sos el ganador";
        char stringPerdedor[] = "Lo lamento jugador * el ganador fue el jugador *";
        int maxIndex = getMaxIndex(puntajes, cantJugadores);

        for (int i = 0; i < cantJugadores; ++i) {
            if (i == maxIndex) {
                stringGanador[20] = i+1 + '0';
                send(socketsJugadores[i], stringGanador, strlen(stringGanador)+1, 0);
                continue;
            }
            stringPerdedor[19] = '0' + (i+1);
            stringPerdedor[47] = '0' + (maxIndex+1);
            send(socketsJugadores[i], stringPerdedor, strlen(stringPerdedor)+1, 0);
        }

        printf("Fin de la partida! la palabra a adivinar era: %s\n", palabraDeJuego);
    }
    return 0;
}

int yaGano(char* cadena) {
    while (*cadena) {
        if (*cadena == '*') {
            return 0;
        }
        ++cadena;
    }

    return 1;
}

int getMaxIndex(int puntajes[], int cantJugadores) {
    int maxPuntaje = -9999;
    int maxIndex = -1;
    for (int i = 0; i < cantJugadores; ++i) {
        if (puntajes[i] > maxPuntaje) {
            maxPuntaje = puntajes[i];
            maxIndex = i;
        }
    }

    return maxIndex;
}

void mostrarEstados(int puntajes[], int cantJugadores) {
    for (int i = 0; i < cantJugadores; ++i) {
        printf("Jugador %d, %d pts.\n", i+1, puntajes[i]);
    }
}

int procesarTurno(int idJugador, int socketId, char* palabraActual, char* palabraDeJuego) {

    send(socketId, palabraActual, strlen(palabraActual), 0);
    char intento;

    read(socketId, &intento, 1);
    int index = 0;
    char actual;
    int primera = 1;
    int acerto = 0;

    while ((actual = *palabraDeJuego)) {

        if (actual == intento && *(palabraActual+index) == '*') {
           acerto = 1;
           *(palabraActual+index) = intento;
        }
       ++index;
       ++palabraDeJuego;
    }
    send(socketId, &acerto, sizeof(int), 0);

    return acerto ? ACIERTO : FALLO;
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
}

void signal_sigint(int signum) {
    return;
}

void signal_sigterm(int signum) {
    system("clear");
    puts("Usted decidio finalizar el programa, saludos!");
    if (SOCKET_SERVER_CONNECTION > 0) {
        close(SOCKET_SERVER_CONNECTION);
    }

    if (1 <= cantJugadores && cantJugadores <= 3) {
        for(int i=0; i < cantJugadores; i++ ) {
            close(socketsJugadores[i]);
        }
    }

    exit(0);
}

void registrarSeniales() {
    signal(SIGINT, signal_sigint);
    signal(SIGTERM, signal_sigterm);
}

void printHelpAndExit(char const* argv[]) {
    printf("Servidor del Ahorcado!\n");
    printf("Sintaxis: %s <PUERTO>\n", argv[0]);
    printf("PUERTO: puerto donde se conectaran los clientes con este servidor\n");
    fflush(stdout);
    exit(0);
}


void checkHelp(int argc, char const* argv[]) {
    for (int i = 1; i < argc; ++i) {
         if (strcmp(argv[i], "--help") == 0) {
            printHelpAndExit(argv);
        }
    }
}

