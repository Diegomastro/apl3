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
        if (cliente < 0) {
            continue;
        }
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
        printf("Turno jugador %d\n", turno+1);
        if (vidasJugadores[turno] <= 0) {
            continue;
        }
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
    return 0;
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







