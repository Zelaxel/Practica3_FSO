#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argn, char* argv[]){
	// No hay argumentos.
	if(argn <= 1) {
		fprintf(stderr, "No se han dado argumentos. Para más información usa './misala help'\n");
		exit(-1);
	}
	
	// Crea.
	if(!strcmp(argv[1], "crea")){
		if(argn != 6 || (strcmp(argv[2], "-f") != 0 && strcmp(argv[2], "-o") != 0) || strcmp(argv[4], "-c") != 0){ // Error. Instruccion invalida.
			fprintf(stderr, "Instruccion desconocida. Para más información usa './misala help'\n");
			exit(-1);
		}
		if(!strcmp(argv[2], "-f")){ // Caso -f.
			if(access(argv[3], F_OK) == 0) { // Error. Archivo ya existe.
				fprintf(stderr, "El archivo '%s' ya existe. Para más información usa './misala help'\n", argv[3]);
				exit(-1);
			}
		}
		if(crea_sala(atoi(argv[5])) == -1){
			fprintf(stderr, "Error al crear sala.\n");
			exit(-1);	
		}
		if(guarda_estado_sala(argv[3]) == -1){
			fprintf(stderr, "Error al guardar sala.\n");
			exit(-1);
		}
	}
	// Reserva
	if(!strcmp(argv[1], "reserva")){
		if(argn < 4 || strcmp(argv[2], "-f") != 0){ // Error. Instruccion invalida.
			fprintf(stderr, "Instruccion desconocida. Para más información usa './misala help'\n");
			exit(-1);
		}
		if(recupera_estado_sala(argv[3]) == -1) {
			fprintf(stderr, "Error al recuperar sala.\n");
			exit(-1);
		}
		if(argn-4 > asientos_libres()){ // Error. Espacio infuficiente.
			fprintf(stderr, "No hay suficientes asientos. Asientos libres: %d. Asientos que quieres reservar: %d.\n", asientos_libres(), argn-4);
			exit(-1);
		}
		for(int i=4; i<argn; i++){ // Reservamos cada asiento.
			if(reserva_asiento(atoi(argv[i])) == -1){ // Error reserva asiento.
				fprintf(stderr, "Error reserva asiento.\n");
				exit(-1);
			}
		}
		if(guarda_estado_sala(argv[3]) == -1){
			fprintf(stderr, "Error al guardar sala.\n");
			exit(-1);
		}
	}
	
	return 0;
}

