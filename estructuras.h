#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

#define P_SIZE sizeof(struct pirlh)

/* estructura del protocolo de la aplicacion*/
struct pirlh {
	char operation; //1 byte para las operaciones
	int16_t leng; // 2 bytes para la longitud del mensaje
	char msg[2000];
};

/*con estos datos se armaran los archivos por recurso, en base a la lista de ellos */
struct nodo_info {
    long mem,swap,cpu;
    char ip[35]; 
};

/* contrendra la direccion IP y MAC en una misma cadena*/
struct nodo_red {
    char red[200]; 
};

/* contrendra la ip y mac por separado, se usan para direfentes propositos */
struct nodo_mac {
    char mac[48],ip[35];
};

/* lista de nodos con sus respectivas direcciones */
struct lista_mac {
    struct nodo_mac nodo;
    struct lista_mac *siguiente;
};

/* lista con la inforacion de recursos de los nodos */
struct nodo_lista {
    struct nodo_info nodo;
    struct nodo_lista *siguiente;  
};

/* estructura configuracion servidor*/
struct config_serv{
	char ip[35];
	char multicast[35];
	int puerto;
	int tiempo_host,tiempo,delta_tiempo;
	char dir_datos_host[200];
};

/* estructura configuracion cliente*/
struct config_cliente{
	char ip_servidor[35],multicast[35];
	int puerto,tiempo,delta_tiempo,tiempo_servidor;
};