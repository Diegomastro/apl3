#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
#define MAX_JUGADORES 3
int main(int argc, char const* argv[]) {

    int cantJugadores;
    int socketsJugadores[MAX_JUGADORES] = {0};
    puts("Ingrese la cantidad de jugadores:");
    scanf("%d", &cantJugadores);

    int server = socket(AF_LOCAL, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_LOCAL;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server, &address, sizeof(address));
    listen(server, cantJugadores);

    int jugadoresConectados = 0;
    
    // jugadores conectados == ingresados por teclado
    while (jugadoresConectados != cantJugadores) {
        int cliente = accept(server, &address, sizeof(address));
        socketsJugadores[jugadoresConectados++] = cliente;
    }

    // while de partida
     

    return 0;
}
