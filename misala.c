#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "sala.h"

// Lee los primeros 8 bytes del fichero para obtener la capacidad de la sala.
int capacidad_fichero(const 	char* ruta_fichero){
		int fd;
		int capacidad;
		ssize_t leido;
		// Accedemos al fichero.
		fd = open(ruta_fichero, O_RDONLY);
		if(fd == -1){ // Error acceder fichero.
			perror("Error acceder fichero");
			exit(-1);
		}
		// Leemos la capacidad.
		leido = read(fd, &capacidad, sizeof(int));
		if(leido == -1){ // Error leer fichero.
			perror("Error leer fichero");
			close(fd);
			exit(-1);
		}
		// Cerramos fichero y devolvemos.
		close(fd);
		return capacidad;
}

int main(int argn, char* argv[]){
	// No hay argumentos.
	if(argn <= 1) {
		fprintf(stderr, "No se han dado argumentos. Para más información consulta './misala help'\n");
		exit(-1);
	}
	
	// Crea.
	if(!strcmp(argv[1], "crea") && argn == 6 && (!strcmp(argv[2], "-f") || !strcmp(argv[2], "-o")) && !strcmp(argv[4], "-c")){
		// Caso -f.
		if(!strcmp(argv[2], "-f") && access(argv[3], F_OK) == 0){ // Error. Archivo ya existe.
			fprintf(stderr, "El archivo '%s' ya existe. Para más información usa 'misala help'\n", argv[3]);
			exit(-1);			
		}
		// Creamos la sala vacia.
		if(crea_sala(atoi(argv[5])) == -1){ // Error crea sala.
			fprintf(stderr, "Error al crear sala.\n");
			exit(-1);	
		}
		// Guardamos la sala en un fichero.
		if(guarda_estado_sala(argv[3]) == -1){ // Error guardar estado.
			fprintf(stderr, "Error al guardar sala.\n");
			exit(-1);
		}
		// Cerramos la sala.
		if(elimina_sala() == -1){ // Error eliminar sala.
			fprintf(stderr, "Error al eliminar sala.\n");
			exit(-1);
		}
		return 0;
	}
	// Reserva.
	if(!strcmp(argv[1], "reserva") && argn >= 4 && !strcmp(argv[2], "-f")){
		// Creamos la sala.
		int capacidad = capacidad_fichero(argv[3]);
		if(crea_sala(capacidad) == -1){ // Error crear sala.
			fprintf(stderr, "Error al crear sala.\n");
			exit(-1);
		}
		// Recuperamos la información de la sala.
		if(recupera_estado_sala(argv[3]) == -1) { // Error recuperar sala.
			fprintf(stderr, "Error al recuperar sala.\n");
			exit(-1);
		}
		// Comprobamos si hay espacio en la sala.
		if(argn-4 > asientos_libres()){ // Error. Espacio infuficiente.
			fprintf(stderr, "Error. No hay suficientes asientos. Asientos libres: %d. Asientos que quieres reservar: %d.\n", asientos_libres(), argn-4);
			exit(-1);
		}
		// Reservamos cada asiento.
		for(int i=4; i<argn; i++){
			int id = atoi(argv[i]);
			if(id < 1){ // Error. ID < 1.
				fprintf(stderr, "Error. ID invalido '%d'. Los ID's deben ser mayores a 0.\n", id);
				exit(-1);
			}
			reserva_asiento(id); // No puede haber errores ya que nos aseguramos de que haya sala, que haya espacio y que el id sea correcto.
		}
		// Guardamos la sala en el fichero.
		if(guarda_estado_sala(argv[3]) == -1){ // Error guardar sala.
			fprintf(stderr, "Error al guardar sala.\n");
			exit(-1);
		}
		// Cerramos la sala.
		if(elimina_sala() == -1){ // Error eliminar sala.
			fprintf(stderr, "Error al eliminar sala.\n");
			exit(-1);
		}
		return 0;
	}
	// Anulacion.
	if(!strcmp(argv[1], "anula") && argn >= 6 && !strcmp(argv[2], "-f") && !strcmp(argv[4], "-asientos")){
		const char* ruta = argv[3];
		size_t num_asientos = argn - 5;
		// Creamos la sala.
		int capacidad = capacidad_fichero(ruta);
		if(crea_sala(capacidad) == -1){ // Error crear sala.
			fprintf(stderr, "Error al crear sala.\n");
			exit(-1);
		}
		// Recuperamos la información de la sala.
		if (recupera_estado_sala(ruta) == -1) {
			fprintf(stderr, "Error al recuperar sala desde el fichero '%s'\n", ruta);
			exit(-1);
		}
		// Liberamos asientos.
		for (int i = 5; i < argn; i++) {
			int id = atoi(argv[i]);
			if(id < 0 || capacidad_sala() <= id) { // Error. ID fuera de rango.
				fprintf(stderr, "ID de asiento inválido: %d\n", id);
			}
			libera_asiento(id); // El unico error posible es que el asiento ya este libre pero es irrelevante.
		}
		// Guardamos la sala en el fichero.
		if(guarda_estado_sala(ruta) == -1){ // Error guardar sala.
			fprintf(stderr, "Error al guardar sala.\n");
			exit(-1);
		}
		// Cerramos la sala.
		if(elimina_sala() == -1){ // Error eliminar sala.
			fprintf(stderr, "Error al eliminar sala.\n");
			exit(-1);
		}
		return 0;
	}
	// Estado.
	if(!strcmp(argv[1], "estado") && argn == 4 && !strcmp(argv[2], "-f")){
		// Creamos la sala.
		int capacidad = capacidad_fichero(argv[3]);
		if(crea_sala(capacidad) == -1){ // Error crear sala.
			fprintf(stderr, "Error al crear sala.\n");
			exit(-1);
		}
		// Recuperamos la información de la sala.
		if(recupera_estado_sala(argv[3]) == -1) { // Error recuperar sala.
			fprintf(stderr, "Error al recuperar sala.\n");
			exit(-1);
		}
		comprobar_asientos();
		// Cerramos la sala.
		if(elimina_sala() == -1){ // Error eliminar sala.
			fprintf(stderr, "Error al eliminar sala.\n");
			exit(-1);
		}
		return 0;
	}
	// help.
	if(!strcmp(argv[1], "help")){
		printf("- misala crea –f ruta –c capacidad\n    Crea una sala y la guarda en un nuevo fichero. Falla si hay un fichero previamente creado.\n\n");
		printf("- misala crea –o ruta –c capacidad\n    Crea una sala y la guarda en un fichero que sobreescribe. Si no hay fichero lo crea.\n\n");
		printf("- misala reserva –f ruta id_persona1 id_persona2 ... id_personaN\n    Asigna id's de personas a los asientos. Falla si no existe la ruta, si no hay espacio suficiente o si un id es invalido.\n\n");
		printf("- misala anula –f ruta -asientos id_asiento1 id_asiento2 ... id_asientoN\n    Libera id's de los asientos. Falla si no existe la ruta. Si un id es invalido lo notifica y continua.\n\n");
		printf("- misala estado –f ruta\n    Muesta los asientos de la sala. Falla si no existe la ruta.\n\n");
		printf("El uso de caracteres en los campos que requieren enteros pueden dar lugar a resultados ines`perados.\n\n");
		return 0;
	}
	// Instruccion desconocida.
	fprintf(stderr, "Instruccion desconocida. Para más información consulta 'misala help'\n");
	exit(-1);
}

