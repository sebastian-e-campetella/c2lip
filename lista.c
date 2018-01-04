#include <stdlib.h>
#include <stdio.h>
#include "estructuras.h"

/* muestra la lista con los recursos */
void mostrar_lista(struct nodo_lista *primero) {
	struct nodo_lista *auxiliar; /* lo usamos para recorrer la lista */
	int i;

	i=0;
	auxiliar = primero;
	while (auxiliar!=NULL) {
	    printf( "ip: %s, mem: %ld, swap : %ld, cpu : %ld \n",
		    auxiliar->nodo.ip,auxiliar->nodo.mem,auxiliar->nodo.swap,auxiliar->nodo.cpu);
	    auxiliar = auxiliar->siguiente;
	    i++;
	}
	if (i==0)
		printf( "\nLa lista está vacía!!\n" );
}

/* segun el recurso se ordena la lista */
void ordenar_lista(struct nodo_lista **primero, char *recurso){
	struct nodo_lista *aux1, *aux2;
	struct nodo_info info;  


	if (*primero !=NULL){  
		aux1 = *primero; 
		while (aux1 != NULL){ 
			aux2 = aux1->siguiente; 

			while (aux2 != NULL){ 
				if (strcmp("mem",recurso) ==0) {
					if (aux1->nodo.mem < aux2->nodo.mem){
						info = aux2->nodo; 
						aux2->nodo = aux1->nodo; 
						aux1->nodo = info;
					}
				}else if(strcmp("swap",recurso) ==0) { 
					if (aux1->nodo.swap < aux2->nodo.swap){
						info = aux2->nodo; 
						aux2->nodo = aux1->nodo; 
						aux1->nodo = info;
					}
				}else if(strcmp("cpu",recurso) ==0) { 
					if (aux1->nodo.cpu < aux2->nodo.cpu){
						info = aux2->nodo; 
						aux2->nodo = aux1->nodo; 
						aux1->nodo = info;
					}
				}else if(strcmp("ip",recurso) ==0) { 
					if (strcmp(aux1->nodo.ip,aux2->nodo.ip) > 0 ){
						info = aux2->nodo; 
						aux2->nodo = aux1->nodo; 
						aux1->nodo = info;
					}
				}
				aux2 = aux2->siguiente;
			
			}
			aux1 = aux1->siguiente;
		}
	}
}



// devuelve 0 sino existe, sino devuelve 1
int buscar_en_lista(struct nodo_lista *primero,char *dato){
	struct nodo_lista *r;

	r = primero;
	if (r != NULL){
		while( (r != NULL) && ( strcmp(r->nodo.ip,dato) != 0) ){
			  r = r->siguiente;
		}
		if( r != NULL){
			if ( strcmp(r->nodo.ip,dato) == 0){
				  printf("la ip esta repetida en la lista es %s \n",r->nodo.ip);
				return -1;
			}else{
				  printf("la ip no esta en lista %s \n",r->nodo.ip);
				return 0;
			}
		}else{
			return -1;
		}
	}else{
		return 0;
	}

}



/* eliminar lista completa de informacion de recursos */
int eliminar_lista(struct nodo_lista **primero){
	struct nodo_lista *r;


	while (*primero != NULL){
		r = *primero;
		*primero = r->siguiente;
		free(r);

	}
	if(*primero == NULL){
		printf("\n  lista de nodos vacia  \n");
	}

	return 0;

}



/*lista para verificar la ip unica*/

void agregar_elemento_mac(struct lista_mac **p,struct lista_mac **u, struct nodo_mac nm) {
	struct lista_mac *nuevo;

	/* reservamos memoria para el nuevo elemento */
	nuevo = (struct lista_mac *) malloc (sizeof(struct lista_mac));
	if (nuevo==NULL)
		 printf( "No hay memoria disponible!\n");

	nuevo->nodo = nm;
	/* el campo siguiente va a ser NULL por ser el último elemento
	de la lista */
	nuevo->siguiente = NULL;

	/* ahora metemos el nuevo elemento en la lista. 
	comprobamos si la lista está vacía. si primero==NULL es que no
	hay ningún elemento en la lista. también vale ultimo==NULL */

	if (*p==NULL) {
		*p = nuevo;
		*u = nuevo;
	}else {
		/* el que hasta ahora era el último tiene que apuntar al nuevo */
		(*u)->siguiente = nuevo;
		/* hacemos que el nuevo sea ahora el último */
		*u = nuevo;
	}
}


/* devuelve 0 sino existe, sino devuelve 1 */
int buscar_en_lista_mac(struct lista_mac *primero,char ip[100], char mac[100]){
	struct lista_mac *r;

	r = primero;
	if (r != NULL){
		while( (r != NULL) && ( strcmp(r->nodo.ip,ip) != 0) ){
			  r = r->siguiente;
		}
		if( r != NULL){
			if ( strcmp(r->nodo.ip,ip) == 0){
				if (strcmp(r->nodo.mac,mac) != 0 ){
					return -1;
				}else if (strcmp(r->nodo.mac,mac) == 0) {
					return 2;
				}
			}else{
				// para agregar
				return 0;
			}
		}else{
			// para agregar
			return 1;
		}
	}else{
		// para agregar
		return 1;
	}

}


/* elimina toda la lista con la informacion de las mac*/
int eliminar_lista_mac(struct lista_mac **primero){
	struct lista_mac *r;


	while (*primero != NULL){
		r = *primero;
		*primero = r->siguiente;
		free(r);

	}

	return 0;

}

/* mostrar lista de direcciones mac */
void mostrar_lista_mac(struct lista_mac *primero) {
	struct lista_mac *auxiliar; /* lo usamos para recorrer la lista */
	int i;

	i=0;
	auxiliar = primero;
	printf("\n Mostrando la lista completa de conexiones mac:\n");
	while (auxiliar!=NULL) {
		printf( "ip: %s, mac: %s \n",auxiliar->nodo.ip,auxiliar->nodo.mac);
		auxiliar = auxiliar->siguiente;
		i++;
	}
	if (i==0) 
		printf( "\nLa lista está vacía\n" );
}

/* ordena la lista de direcciones mac */
void ordenar_lista_mac(struct lista_mac **primero){
	struct lista_mac *aux1, *aux2;
	struct nodo_mac mac;  

	if (*primero !=NULL){  
		aux1 = *primero; 
		while (aux1 != NULL){ 
			aux2 = aux1->siguiente; 

			while (aux2 != NULL){ 
				if (strcmp(aux1->nodo.ip,aux2->nodo.ip) > 0 ){
					mac = aux2->nodo; 
					aux2->nodo = aux1->nodo; 
					aux1->nodo = mac;
				}
				aux2 = aux2->siguiente;
			
			}
			aux1 = aux1->siguiente;
		}
	}
}

/* fin de funciones de lista para verificar la ip unica*/



/* devuelve una lista con las direcciones a hacer eco hace una diferencia entre la lista de nodos que 
	enviaron datos y los que hay en la lista temporal de nodos conocidos
 */
int lista_ip_eco(struct lista_mac **pmac, struct nodo_lista **p, struct lista_mac **pn,struct lista_mac **nu){
	struct lista_mac *pm;
	struct nodo_lista *pl;
	struct nodo_mac nm;
	char **lista;	

	ordenar_lista(p,"ip");
	ordenar_lista_mac(pmac);

	pm = *pmac;
	pl = *p;

	int salir=0;
	/*recorro la lista de nodos que enviaron datos en un plazo de tiempo determinado */

	while (pm != NULL){
		/*recorro la lista de nodos que enviaron datos en este plazo */
		while ( (pl != NULL) && salir == 0){
			 if (strcmp(pm->nodo.ip,pl->nodo.ip) <= 0){
				pl = pl->siguiente;
			 }
			 if (pl != NULL){
				/* si no coincide entonces agregar a la lista para enviar eco */
				if (strcmp(pm->nodo.ip,pl->nodo.ip) > 0 ){
					strcpy(nm.ip,pm->nodo.ip);
					agregar_elemento_mac(pn,nu,nm);
					salir = 1;
				}
			 }
		}

		salir = 0;
		pl = *p;
		pm = pm->siguiente;
	}

	return 0;

}

/* mostrar la lista de ip de nodos */
void mostrar_lista_ip(struct lista_mac *lista){

	while (lista != NULL){
		printf("\n %s \n",lista->nodo.ip);
		lista = lista->siguiente;
	
	}

}
