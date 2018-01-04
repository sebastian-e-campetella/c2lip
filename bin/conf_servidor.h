#include <stdio.h>
#include <libxml/parser.h>  /*trabaja con los ficheros xml*/
#include <libxml/tree.h>    /*trabaja con el árbol generado de parsear el xml*/
#include <libxml/xmlmemory.h>
#include <string.h>
#include <glib.h>
/* construcye todo el documento xml de la configuracion que enviará el servidor al cliente */

xmlChar* construir_xml_configuracion_servidor(xmlDocPtr *doc, char *ip,char *multicast ,int tiempo,int dt,char *dir_datos_host, int puerto, int th);


/* configuracion por consola */
int configurar_servidor();





