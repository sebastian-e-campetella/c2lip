#include <stdio.h>
#include <libxml/parser.h>  /*trabaja con los ficheros xml*/
#include <libxml/tree.h>    /*trabaja con el árbol generado de parsear el xml*/
#include <libxml/xmlmemory.h>
#include <string.h>
#include <glib.h>
#include "../funciones.h"

/* construcye todo el documento xml del cliente que enviará al servidor sigue con la funcion ver_plano */

xmlChar* construir_xml_configuracion_servidor(xmlDocPtr *doc, char *ip,char *multicast ,int tiempo,int dt,char *dir_datos_host, int puerto, int th){
	xmlNodePtr root;

	char buffer[150];

	// crear el nuevo documento
	*doc = xml_nuevo_doc("configuracion");
	root = xmlDocGetRootElement(*doc);

	// crear un hijo con el ip del servidor
	xmlNewChild (root, NULL, "ip", (xmlChar*)ip);

	// crear un hijo con el ip de multicast
	xmlNewChild (root, NULL, "multicast", (xmlChar*)multicast);

	// crear un hijo con el tiempo de entrega general
	sprintf(buffer,"%d",tiempo);
	xmlNewChild (root, NULL, "tiempo",buffer);

	// crear un hijo con el delta tiempo de entrega
	sprintf(buffer,"%d",dt);
	xmlNewChild (root, NULL, "delta_tiempo",buffer);

	// crear un hijo con el delta tiempo de entrega
	sprintf(buffer,"%d",th);
	xmlNewChild (root, NULL, "tiempo_host",buffer);


	// direccion puerto
	sprintf(buffer,"%d",puerto);
	xmlNewChild (root, NULL, "puerto",buffer);

	// direccion archivo de configuracion
	sprintf(buffer,"%s",dir_datos_host);
	xmlNewChild (root, NULL, "dir_datos_host",buffer);


	xmlChar *result=NULL;
	int len;
	// guarda en memoria el documento en el buffer result y la longitud en len
	xmlSaveFile("../conf_servidor.xml", *doc);
	xmlDocDumpMemory(*doc, &result, &len);
	return result;
  
}




int configurar_servidor(){
	xmlDocPtr doc;

	struct config_serv c;

	printf("Direccion IP \n");
	scanf("%s",c.ip);
	printf("Direccion multicast \n");
	scanf("%s",c.multicast);
	printf("Direccion de puerto \n");
	scanf("%d",&c.puerto);
	printf("Tiempo de recepcion de datos \n");
	scanf("%d",&c.tiempo);

	printf("Tiempo de host \n");
	scanf("%d",&c.tiempo_host);

	printf("Tiempo de tolerancia en recepcion de datos en centecimas \n");
	scanf("%d",&c.delta_tiempo);
	printf("Directorio a guardar informacion de recursos \n");
	scanf("%s",c.dir_datos_host);

	printf("%s",construir_xml_configuracion_servidor(&doc, c.ip,c.multicast,c.tiempo,c.delta_tiempo,c.dir_datos_host,c.puerto,c.tiempo_host));


	xmlFreeDoc(doc);

        return 0;
}





