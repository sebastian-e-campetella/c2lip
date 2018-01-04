#include <stdio.h>
#include <glib.h>
#include <libxml/parser.h>  /*trabaja con los ficheros xml*/
#include <libxml/tree.h>    /*trabaja con el árbol generado de parsear el xml*/
#include <libxml/xmlmemory.h>
#include <string.h>

/* construcye todo el documento xml con la configuracion del cliente, se altera cuando el servidor le envia datos */

xmlChar* construir_xml_configuracion_cliente(xmlDocPtr *doc, char *ip,char *multicast, int tiempo,int dt, int puerto);


/* configurar por consola */
int configurar_cliente();

void modificar_xml_cliente(xmlChar *res);
