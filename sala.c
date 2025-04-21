#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int* sala_teatro = NULL;
int capacidad_total = 0;

int reserva_asiento(int id_persona){
	//Falla si la sala no esta creada o si el id de la persona no es valido.
	if(sala_teatro==NULL || id_persona < 1) return -1; 

	//Busca un espacio libre.
	for(int i=0; i<capacidad_total; i++){
		if(sala_teatro[i] == -1){
			sala_teatro[i] = id_persona;
			return i;
		}
	}
	return -1; //No hay espacio libre.
}

int libera_asiento(int id_asiento){
	//Falla si la sala no esta creada o si el id del asiento se sale del espacio.
	if(sala_teatro==NULL || id_asiento >= capacidad_total || id_asiento < 0) return -1;
	
	//Hay asiento por lo que lo libera.
	int id_persona = sala_teatro[id_asiento];
	sala_teatro[id_asiento]=-1;
	return id_persona;

}

int estado_asiento(int id_asiento){
	//Falla si la sala no esta creada o si el id del asiento se sale del espacio.
	if(sala_teatro==NULL || id_asiento >= capacidad_total || id_asiento < 0) return -1;
	
	//El asiento esta vacío.
	if(sala_teatro[id_asiento] == -1) return 0;
	
	//Devuelve la persona que reservó el asiento.
	return sala_teatro[id_asiento];
}

int asientos_ocupados(){
    //Falla si la sala no esta creada o si el id del asiento se sale del espacio.
    if(sala_teatro == NULL){
        return -1;
    }
    int ocupados = 0;
    // Inicializamos el contador de asientos ocupados a 0
    for(int i=0; i<capacidad_total; i++){
        if(sala_teatro[i] != -1){
            // Si el asiento no esta libre, incrementamos la variable "ocupados"
            ocupados++;
        }
    }
    return ocupados;
}

int asientos_libres(){
	//Falla si la sala no esta creada.
	if(sala_teatro==NULL) return -1;
	return capacidad_total - asientos_ocupados();
}

int capacidad_sala(){
    //Falla si la sala no esta creada o si el id del asiento se sale del espacio.
    if(sala_teatro == NULL){
        return -1;
    }
    return capacidad_total;
}

int crea_sala(int capacidad){
    //Falla si la sala no esta creada o si el id del asiento se sale del espacio.
    if(sala_teatro != NULL || capacidad < 1){
        return -1;
    }
    capacidad_total = capacidad;
    // Creamos un puntero en memoria, reservando espacio para almacenar
    // los asientos de la sala. Lo ajustamos en base al tamaño del entero.
    sala_teatro = (int*)malloc(capacidad*sizeof(int));
    for(int i=0; i<capacidad; i++){
		// A cada asiento de la sala le asignamos -1 para indicar de que estan libres
		sala_teatro[i] = -1;
    }
    return capacidad;
}

int elimina_sala(){
    //Falla si la sala no esta creada o si el id del asiento se sale del espacio.
    if(sala_teatro == NULL){
        return -1;
    }
    // Liberamos el espacio reservado en memoria y inicializamos nuevamente "sala_teatro" a NULL.
	free(sala_teatro);
    sala_teatro=NULL;
    return 0;
}

int guarda_estado_sala(const char* ruta_fichero){
	int fd = creat(ruta_fichero, 0644); // El usuario tiene acceso a leer y escribir. El resto solo tienen 
	if(fd==-1){							// acceso a la lectura
		perror("Error al crear el fichero");
		return -1;
	}
	
	ssize_t escritura = write(fd,sala_teatro,capacidad_total*sizeof(int)); // Se escriben los estados de los asientos
	if(escritura==-1 || escritura != capacidad_total * sizeof(int)){												 	   // de la sala
		perror("Error al escribir el estado de la sala");
		close(fd);
		return -1;
	}
	close(fd);
	// Una vez abierto el fichero siempre hay que cerrarlo.
	return 0;	// Si todo se ha ejecutado correctamente devolvemos un 0
}

int recupera_estado_sala(const char* ruta_fichero){
	if(sala_teatro == NULL){ // Se verifica que la sala esta creada
		return -1;
	}
	int fd = open(ruta_fichero, O_RDONLY); // Abrimos el fichero correspondiente y comprobamos que se
	if(fd==-1){							   // pueda abrir con exito
		perror("Error al abrir el fichero");
		return -1;
	}
	struct stat info; // Creamos una variable "stat" para analizar los datos del fichero abierto
	if(fstat(fd,info)==-1 || info.st_size != capacidad_total*sizeof(int)){ 		// Comprobamos que se puedan ver los detalles y
		perror("Error con la informacion del fichero o la capacidad no coincide"); 	// que la capacidad coincida
		close(fd);
		return -1;
	}
	int lectura = read(fd,sala_teatro,info.st_size); // Leemos el fichero y transcribimos los datos a "sala_teatro"
	if(lectura==-1){
		perror("Error al leer el fichero");
		close(fd);
		return -1;
	}
	close(fd);
	// Una vez abierto el fichero siempre hay que cerrarlo. Incluso en las verificaciones anteriores, si llegase
	// a ocurrir algun error y el fichero estuviera abierto.
	return 0; // Si todo se ha ejecutado correctamente devolvemos un 0
}

int guarda_estado_parcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
	if(sala_teatro == NULL){ // Verificamos si la sala ha sido creada, en caso contrario retornamos -1
		return -1;
	}
	inf fd = open(ruta_fichero, O_RDWR); // Abrimos el fichero en formato LECTURA/ESCRITURA
	if(fd==-1){
		perror("Error al abrir el fichero");
		return -1;
	}
	struct stat info; // Creamos la variable y con la funcion "fstat()" la llenamos con la info del fichero
	if(fstat(fd,info)==-1 || info.st_size != capacidad_total*sizeof(int)){
		perror("Error con la informacion del fichero o la capacidad no coincide");
		close(fd);
		return -1;
	}
	for(size_t i=0; i<num_asientos; i++){	// Creamos un bucle para rellenar el fichero con el conjunto de 
		int id = id_asientos[i];			// asientos que se nos indican
		if(id<0 || id>=capacidad_total){
			perror("ID de asientos fuera de rango");
			close(fd);
			return -1;
		}
		if(lseek(fd, id*sizeof(int), SEEK_SET)==-1){	// Ajustamos el puntero del fichero con respecto al inicio
			perror("Error al pasar la informacion del fichero");
			close(fd);
			return -1;
		}
		int valor = sala_teatro[id];	// Guardamos la posicion del asiento que vamos a guardar y lo escribimos
		ssize_t escritura = write(fd,&valor,sizeof(int));
		if(escritura != sizeof(int){
			perror("Error al escribir el estado de la sala");
			close(fd);
			return -1;
		})
	}
	// Una vez abierto el fichero siempre hay que cerrarlo. Incluso en las verificaciones anteriores, si llegase
	// a ocurrir algun error y el fichero estuviera abierto.
	close(fd);
	return 0;
}

// Minishell. 1º argumento: nombre de sala. 2º argumento: capacidad de sala.
int main(int argc, char * argv[]){
	
	crea_sala(atoi(argv[2])); // Crea la sala.
	
	char *nombre_sala = argv[1];
	char menu[215] = "INSTRUCCIONES SALA DE %s:\n1. reserva_asiento\n2. libera_asiento\n3. estado_asiento\n4. estado_sala\n5. cerrar_sala\n6. limpiar_panel\n\n";
	
	// Menú.
	printf(menu,nombre_sala);
	while(1){
		char instruccion[100];
		scanf("%s",&instruccion); // Nueva instruccion.
		
		if(!strcmp(instruccion,"reserva_asiento")){ // 1. Reserva asiento.
			char id[100];
			printf("Dame el ID de la persona: ");
			scanf("%s",&id);
			int id_persona = atoi(id);
			int id_asiento = reserva_asiento(id_persona);
			switch(id_asiento){
				case -1: // Persona erronea.
					if(id_persona < 1){
						printf("Error. El ID nº %d de la persona es invalido. Los ID's de personas deben ser mayor de 0.\n\n",
						id_persona);
					} else {
						printf("Todos los asientos están ocupados.\n\n");
					}
					break;
				default: // Asiento encontrado.
					printf("El asiento nº %d ahora está asociado a la persona con ID nº %d.\n\n",
							id_asiento,
							id_persona);
					break;
			}
		}
		
		else if(!strcmp(instruccion,"libera_asiento")){ // 2. Liberar asiento.
			char id[100];
			printf("Dame el ID del asiento: ");
			scanf("%s",&id);
			int id_asiento = atoi(id);
			int id_persona = libera_asiento(id_asiento);
			switch(id_persona){
				case -1: // Asiento erroneo.
					if(id_asiento < 0 || capacidad_sala() <= id_asiento){
						printf("Error. El asiento nº %d no existe. Solo hay asientos del 0 al %d.\n\n",
								id_asiento,
								capacidad_sala()-1);
					} else {
						printf("El asiento nº %d ya estaba libre.\n\n",
								id_asiento);
					}
					break;
				default: // Asiento ocupado.
					printf("El asiento nº %d estaba ocupado por la persona con ID nº %d. Ahora está libre.\n\n",
							id_asiento,
							id_persona);
					break;
			}
		}
		
		else if(!strcmp(instruccion,"estado_asiento")){ // 3. Estado asiento.
			char id[100];
			printf("Dame el ID del asiento: ");
			scanf("%s",&id);
			int id_asiento = atoi(id);
			int id_persona = estado_asiento(id_asiento);
			switch(id_persona){
				case -1: // Asiento erroneo.
					printf("Error. El asiento nº %d no existe. Solo hay asientos del 0 al %d\n\n",
							id_asiento,
							capacidad_sala()-1);
					break;
				case 0: // Asiento libre.
					printf("El asiento nº %d está libre.\n\n",
							id_asiento);
					break;
				default: // Asiento ocupado.
					printf("El asiento nº %d está ocupado por la persona %d\n\n",
							id_asiento,
							id_persona);
					break;
			}
		}
		
		else if(!strcmp(instruccion,"estado_sala")){ // 4. Estado sala.
			printf("Sala de %s:\nAsientos totales: %d\nAsientos ocupados: %d\nAsientos libres: %d\n\n",
					nombre_sala,
					capacidad_sala(),
					asientos_ocupados(),
					asientos_libres());
		}
		
		else if(!strcmp(instruccion,"cerrar_sala")){ // 5. Cierra la sala.
			elimina_sala();
			break;
		}
		
		else if(!strcmp(instruccion,"limpiar_panel")){ // 6. Limpia el terminal.
			int estado;
			switch(fork()){
				case -1: // Error al lanzar proceso.
					printf("No se pudo limpiar la pantalla. Repitalo ahora o más tarde.\n\n");
					break;
				case 0:	// (hijo) Borra la pantalla.
					execlp("clear","clear",NULL);
					break;
				default: // (Padre) Espera a que el hijo borre.
					wait(&estado);
					printf(menu,nombre_sala);
					break;
			}
		}
		
		else printf("Instruccion invalida '%s'. Intente de nuevo.\n\n",instruccion); // Opcion invalida.
	}
}
