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

// Guarda el estado de la sala en un archivo.
int guarda_estado_sala(const char* ruta_fichero){
	if(sala_teatro == NULL) return -1; // Error. No hay sala.
	ssize_t escrito;
	
	// Accedemos al fichero.
	int fd = open(ruta_fichero, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (fd == -1) { // Error acceder al fichero.
        perror("Error al abrir el fichero.");
        return -1;
    }
    // Guardamos la capacidad.
    escrito = write(fd, &capacidad_total, sizeof(int));
    if(escrito == -1 || escrito != sizeof(int)){ // Error al escribir la. capacidad.
    	perror("Error al escribir la capacidad de la sala en el fichero.");
        close(fd);
        return -1;
    }
    // Guardamos los asientos.
    escrito = write(fd, sala_teatro, capacidad_total*sizeof(int));
    if(escrito == -1 || escrito != capacidad_total*sizeof(int)){
    	perror("Error al escribir los asientos en el fichero.");
        close(fd);
        return -1;
    }
	
	close(fd);	// Una vez abierto el fichero siempre hay que cerrarlo.
	return 0;	// Si todo se ha ejecutado correctamente devolvemos un 0
}

int recupera_estado_sala(const char* ruta_fichero){
	if(sala_teatro == NULL) return -1; // Error. No hay sala.
	
	int capacidad_fichero;
	ssize_t leido;
	
	// Accedemos al fichero.
	int fd = open(ruta_fichero, O_RDONLY);
	if(fd == -1){ // Error acceder fichero.
		perror("Error al acceder a fichero.");
		return -1;
	}
	// Leemos la capacidad.
	leido = read(fd, &capacidad_fichero, sizeof(int));
	if(leido == -1 || leido != sizeof(int)){ // Error lectura.
		perror("Error al leer fichero.");
		close(fd);
		return -1;
	} else if(capacidad_fichero != capacidad_total){ // Error capacidad erronea.
		fprintf(stderr, "Error. la capacidad de la sala '%d' y la del fichero '%d' no coinciden.",
				capacidad_total, capacidad_fichero);
		close(fd);
		return -1;
	}
	// Leemos los asientos.
	leido = read(fd, sala_teatro, capacidad_total*sizeof(int));
	if(leido == -1 || leido != capacidad_total*sizeof(int)){ // Error lectura.
		perror("Error al leer fichero.");
		close(fd);
		return -1;
	}
	
	close(fd); // Todo salio bien;
	return 0;
}

int guarda_estado_parcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
	if(sala_teatro == NULL){ // Verificamos si la sala ha sido creada, en caso contrario retornamos -1
		return -1;
	}
	int fd = open(ruta_fichero, O_RDWR); // Abrimos el fichero en formato LECTURA/ESCRITURA
	if(fd==-1){
		perror("Error al abrir el fichero");
		return -1;
	}
	struct stat info; // Creamos la variable y con la funcion "fstat()" la llenamos con la info del fichero
	if(fstat(fd,&info)==-1 || info.st_size != capacidad_total*sizeof(int)){
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
		// Guardamos la posicion del asiento que vamos a guardar y lo escribimos
		ssize_t escritura = write(fd,&sala_teatro[id],sizeof(int));
		if(escritura != sizeof(int)){
			perror("Error al escribir el estado de la sala");
			close(fd);
			return -1;
		}
	}
	// Una vez abierto el fichero siempre hay que cerrarlo. Incluso en las verificaciones anteriores, si llegase
	// a ocurrir algun error y el fichero estuviera abierto.
	close(fd);
	return 0;
}

int recupera_estado_parcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
	if(sala_teatro == NULL){ // Se verifica que la sala esta creada
		return -1;
	}
	int fd = open(ruta_fichero, O_RDONLY); // Abrimos el fichero correspondiente y comprobamos que se
	if(fd==-1){							   // pueda abrir con exito
		perror("Error al abrir el fichero");
		return -1;
	}
	struct stat info; // Creamos una variable "stat" para analizar los datos del fichero abierto
	if(fstat(fd,&info)==-1 || info.st_size != capacidad_total*sizeof(int)){ 		// Comprobamos que se puedan ver los detalles y
		perror("Error con la informacion del fichero o la capacidad no coincide"); 	// que la capacidad coincida
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
		// Al leer los registros, se transquivira los datos a "sala_teatro".
		ssize_t lectura = read(fd,&sala_teatro[id],sizeof(int));
		if(lectura != sizeof(int)){ 
			perror("Error al escribir el estado de la sala");
			close(fd);
			return -1;
		}
	}
	// Una vez abierto el fichero siempre hay que cerrarlo. Incluso en las verificaciones anteriores, si llegase
	// a ocurrir algun error y el fichero estuviera abierto.
	close(fd);
	return 0;
}

int main(){
	char* fichero = "sala_texto";
	int capacidad = 500;
	
	crea_sala(capacidad);
	reserva_asiento(50);
	reserva_asiento(51);
	reserva_asiento(54);
	
	printf("Capacidad = %d\n",asientos_libres());
	printf("Asiento 0 = %d\n",estado_asiento(0));
	printf("Asiento 1 = %d\n",estado_asiento(1));
	printf("Asiento 2 = %d\n",estado_asiento(2));
	printf("Asiento 3 = %d\n",estado_asiento(3));
	
	guarda_estado_sala(fichero);
	
	elimina_sala();
	crea_sala(capacidad);
	
	printf("Capacidad = %d\n",asientos_libres());
	printf("Asiento 0 = %d\n",estado_asiento(0));
	printf("Asiento 1 = %d\n",estado_asiento(1));
	printf("Asiento 2 = %d\n",estado_asiento(2));
	printf("Asiento 3 = %d\n",estado_asiento(3));
	
	recupera_estado_sala("a");
	
	printf("Capacidad = %d\n",asientos_libres());
	printf("Asiento 0 = %d\n",estado_asiento(0));
	printf("Asiento 1 = %d\n",estado_asiento(1));
	printf("Asiento 2 = %d\n",estado_asiento(2));
	printf("Asiento 3 = %d\n",estado_asiento(3));
}
