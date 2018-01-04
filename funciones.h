#include <stdio.h>
#include <libxml/parser.h>  /*trabaja con los ficheros xml*/
#include <libxml/tree.h>    /*trabaja con el árbol generado de parsear el xml*/
#include <libxml/xmlmemory.h>
#include <string.h> 
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <libxml/globals.h>

#include "recursos.h"
#include "estructuras.h"
#include "lista.h"

#define LONG_RECURSOS 2000
#define LONG_DOCUMENTO 100000

/* crea la estructura xmlDoc con nodo raiz "name" */
xmlDocPtr xml_nuevo_doc (const gchar *nombre);

/* agrega a un puntero de documento la informacion del microprocesador */
void xml_nueva_entrada_cpu(xmlDocPtr *doc, glibtop_cpu nuevo);

/* agrega a un puntero de documento la informacion de la memoria ram */

void xml_nueva_entrada_ram(xmlDocPtr *doc, glibtop_mem nuevo);
/* agrega a un puntero de documento la informacion de la memoria de intercambio */

void xml_nueva_entrada_swap(xmlDocPtr *doc, glibtop_swap nuevo);

/* Esta funcion es para poder pasarlo por mensaje del protocolo  */
void ver_plano(xmlChar *result);


/* modificar xml existente*/

int modificar_xml(xmlNodePtr aux_root, char *padre, xmlChar *dato);

/*fin modificar xml*/


/* obtener valor de un xml */

xmlChar* obtener_valor_xml(xmlNodePtr aux_root, char *padre);

/* Obtener informacion de configuracion del servidor */

int obtener_conf_servidor(struct config_serv *cs);

/* Obtener informacion de configuracion del cliente */

int obtener_conf_cliente(struct config_cliente *cs);


// construcye todo el documento xml del cliente que enviará al servidor sigue con la funcion ver_plano
int construir_xml_cliente(char *ip, char b[LONG_RECURSOS]);


/* Con esta función añadimos un elemento al final de la lista */
void agregar_elemento(struct nodo_info ni, struct nodo_lista **primero, struct nodo_lista **ultimo);

/* recorrido recursivo del documento */
void armar_lista(xmlNodePtr aux_root, char *padre, struct nodo_info *a, struct nodo_lista **p, struct nodo_lista **u);

/* crear documentos xml ordenados a partir de las listas */

void crear_xml_listas(struct nodo_lista **p,char *tipo_recurso, char *destino);

/* crear listas ordenadas */

//void ordenar_listas_xml(xmlChar *res, struct nodo_lista **p, struct nodo_lista **u, char *destino);
void ordenar_listas_xml(char res[LONG_DOCUMENTO], struct nodo_lista **p, struct nodo_lista **u, char *destino);

/* fin listas ordenadas*/

// crea el documento xml del servidor 
void crear_xml_servidor(xmlDocPtr *doc);
int agregar_xml_servidor(xmlDocPtr doc, char res[LONG_DOCUMENTO]);

/*devuelve la diferencia de los tiempos */
double timeval_diff(struct timeval *a, struct timeval *b);

/* utilizada para el temporizador */
void noresponde();

/* devuelve un string con el archivo configuracion del servidor */
xmlChar* obtener_xml_servidor();

/* devuelve un string con el archivo configuracion del cliente */
xmlChar* obtener_xml_cliente();

/* devuelve la configuracion del servidor en cs */
int obtener_conf_servidor_cadena(struct config_serv *cs, xmlChar *res);

/* devuelve la configuracion del cliente en cs */
int obtener_conf_cliente_cadena(struct config_cliente *cs, xmlChar *res);

/* arma el archivo xml en el orden de la lista */
int armar_xml_orden(char res[LONG_DOCUMENTO], struct nodo_lista **p,struct nodo_lista **u, char *destino);

/* separa la cadena en dos, con la direccion IP y MAC */
void spli_mac_ip(char *cadena,char *separador,char ip[100],char mac[100]);

