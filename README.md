Ejercicio 4:
Realizar el juego del ahorcado, pero por turnos y permitiendo entre 1 y 3 jugadores. Cada
jugador elije una letra por turno, si acierta suma 2 puntos, si no acierta resta un punto.
Gana el jugador que tiene mayor puntaje, siempre que no hayan terminado ahorcados,
por supuesto.
Se debe crear un proceso coordinador, que debe quedar ejecutando en primer plano
mostrando el estado de los jugadores. Este proceso no puede ser finalizado con Ctrl-C.
Para detenerlo se debe enviar la señal SIGTERM. Este proceso debe contar con un
archivo con la lista de palabras a jugar, que se deberán seleccionar en forma aleatoria.
La comunicación entre el coordinador y los jugadores se debe realizar a través de
memoria compartida.

Ejercicio 5:
Implementar el mismo juego del ahorcado, pero en red, estableciendo la comunicación vía sockets.
Tanto el cliente como el servidor deben tomar por parámetro los datos necesarios para poder
establecer la conexión.

Ejercicio 1:
Crear una jerarquía de procesos que replique el siguiente grafo:
Al proceso 8 lo genera el proceso 4, no es un error del dibujo. Cada proceso deberá imprimir por
pantalla:
Proceso 1 - PID 3467 - Padre: 15
Proceso 2 - PID 575 - Padre: 3467
Proceso 3 - PID 4672 - Padre: 3467
Generar una espera antes de finalizar los procesos para poder validar la correcta generación (10
segundos), o bien el típico “Presione una tecla para continuar…”
