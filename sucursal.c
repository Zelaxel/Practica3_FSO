#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int max_salas = 1000;
pid_t salas_actuales[1000]; // Procesos de sala.
char nombres_salas[1000][100]; // Nombre de los procesos.
int estados;
int n_salas = 0;

void crea_sucursal (char* ciudad, char* capacidad){
	
	// Comprueba si la capacidad es valida.
	if(atoi(capacidad) < 1){
		printf("La capacidad %d es invalida. Solo se admiten capacidades > 0.\n", atoi(capacidad));
		return;
	} 

	// Generamos un duplicado del proceso actual
	pid_t proceso = fork();
	
	// Se comprueba si no se ha podido crear un nuevo proceso
	if(proceso == -1){
		printf("Se ha producido un error al crear la sala. Intentelo de nuevo.\n");
		exit(-1);
	}
	// En caso contrario, se ejecuta "gnome-terminal" y se abre la aplicacion ./sala"
	if(proceso == 0) {
		execlp("gnome-terminal","gnome-terminal", "--wait", "--title", ciudad, "--", "./sala", ciudad, capacidad, NULL);
		printf("Se ha producido un error al crear la sala. Intentelo de nuevo.\n");
		exit(-1);
	}
	// Padre.
	else{
		salas_actuales[n_salas] = proceso;
		strcpy(nombres_salas[n_salas],ciudad);
		n_salas++;
	}
}

int main(){
	char nombre_sala[100];
	char capacidad[100];
	char instruccion[100];
	char menu[200] = "1. crea_sucursal\n2. salas_cerradas\n3. salir\n4. limpiar_panel\n\n";
	
	printf(menu);
	while (1) {
		scanf("%s", &instruccion);
		
		if(!strcmp(instruccion,"crea_sucursal")){ // 1. Crea sucursal.
	       	if(n_salas == max_salas){ // No caben más salas.
				printf("Número de salas máximo alcanzado. Revisa si hay salas cerradas para liberarlas.\n");
			}
			else { // Se crea la sala.
			    printf("Ingrese el nombre de la ciudad: ");
			    scanf("%s", nombre_sala);
			    printf("¿Cual sera la capacidad?: ");
			    scanf("%s", &capacidad);
			    crea_sucursal(nombre_sala, capacidad);
			    printf("\n");
	    	}
	    }
	    
		else if(!strcmp(instruccion,"salas_cerradas")){ // 2. Devuelve las salas cerradas.
			int estado;
			pid_t pid;
			while((pid = waitpid(-1, &estado, WNOHANG)) > 0){ // Buscamos los procesos terminados.
				for(int i=0; i<n_salas; i++){
					if(pid==salas_actuales[i]){
						printf("La sala %s ha cerrado.\n",nombres_salas[i]);
						salas_actuales[i] = -1; // Los eliminamos de la lista.
					}
				}
			}
			int n = n_salas;
			for(int i=0; i<n; i++){ // Reorganizamos la lista.
				if(salas_actuales[i] == -1){
					n_salas--;
					for(int j=i; j<n-1; j++){
						salas_actuales[j] = salas_actuales[j+1];
						strcpy(nombres_salas[j],nombres_salas[j+1]);
					}
				}
			}
			printf("Listo.\n\n");
		}
		    
		else if(!strcmp(instruccion,"salir")){ // 3. Cerrar la ejecución.
	        printf("\nProceso finalizado\n\n");
	        return 0;
	    }
	    
	    else if(!strcmp(instruccion,"limpiar_panel")){ // 4. Limpia el terminal.
			int estado;
			pid_t proceso = fork();
			switch(proceso){
				case -1: // Error al lanzar proceso.
					printf("No se pudo limpiar la pantalla. Intentelo de nuevo.\n\n");
					break;
				case 0:	// (hijo) Borra la pantalla.
					execlp("clear","clear",NULL);
					printf("No se pudo limpiar la pantalla. Intentelo de nuevo.\n\n");
					break;
				default: // (Padre) Espera a que el hijo borre.
					waitpid(proceso, &estado, 0);
					printf("%s",menu);
					break;
			}
		}
	    
		else printf("Instruccion invalida '%s'. Intente de nuevo.\n\n",instruccion); // Opcion invalida.
	}
}
