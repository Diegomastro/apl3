#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
#define MAX_JUGADORES 3
#define ACIERTO 2
#define FALLO -1


void mostrarEstados(int puntajes[], int cantJugadores);
int procesarTurno(int idJugador, int socketId, char* palabraActual, char* palabraDeJuego);

int main(int argc, char const* argv[]) {

    char* hello = "Hello from server";
    int cantJugadores;
    int socketsJugadores[MAX_JUGADORES] = {0};
    int vidasJugadores[] = {6, 6, 6};
    char* palabrasJugadores[] = {
        "*******",
        "*******",
        "*******"
    };
    int puntajes[MAX_JUGADORES] = {0};

    puts("Ingrese la cantidad de jugadores:");
    scanf("%d", &cantJugadores);

    int server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
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

    bind(server, (const struct sockaddr *) &address, (socklen_t) sizeof(address));
    listen(server, cantJugadores);

    int jugadoresConectados = 0;
    
    // jugadores conectados == ingresados por teclado
    while (jugadoresConectados != cantJugadores) {
        printf("Jugadores conectados = %d\n", jugadoresConectados);
        int cliente = accept(server, &address, &len);
        socketsJugadores[jugadoresConectados++] = cliente;
        printf("clietne %d\n", cliente);
    }

 
    for (int i = 0; i < jugadoresConectados; ++i)
    {
        int val = send(socketsJugadores[i], hello, strlen(hello), 0);
        printf("%d\n", val);
        fflush(stdout);
    }


    char* palabraDeJuego = "palabra"; // copiar del ejercio 4
    // while de partida
    int cantTerminados = 0;
    int turno = 0;
    while (cantTerminados != cantJugadores) {
        if (vidasJugadores[turno] <= 0 || yaGano(palabrasJugadores[turno])) {
            continue;
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
    system("clear");
    puts("Termino la partida!");

    char stringGanador[] = "Felicidades! sos el ganador";
    char stringPerdedor[] = "Lo lamento jugador * el ganador fue el jugador *";
    int maxIndex = getMaxIndex(puntajes, cantJugadores);

    for (int i = 0; i < cantJugadores; ++i) {
        if (i == maxIndex) {
            send(socketsJugadores[i], stringGanador, strlen(stringGanador)+1, 0);
            continue;
        }
        stringPerdedor[19] = (i+1) + '0';
        stringPerdedor[47] = (maxIndex+1) + '0';
        send(socketsJugadores[i], stringPerdedor, strlen(stringPerdedor)+1, 0);
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
       if (actual == intento && palabraActual[index] == '*') {
           acerto = 1;
           palabraActual[index] = intento;
       }
    }

    send(socketId, &acerto, sizeof(int), 0);

    return acerto ? ACIERTO : FALLO;
}







