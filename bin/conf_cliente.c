#include "../funciones.h"

// construcye todo el documento xml del cliente que enviará al servidor sigue con la funcion ver_plano
xmlChar* construir_xml_configuracion_cliente(xmlDocPtr *doc, char *ip,char *multicast, int tiempo,int dt, int puerto){
	xmlNodePtr root;

	char buffer[50];

	// crear el nuevo documento
	*doc = xml_nuevo_doc("configuracion");
	root = xmlDocGetRootElement(*doc);

	// crear un hijo con el ip del servidor
	xmlNewChild (root, NULL, "ip_servidor", (xmlChar*)ip);

	// crear un hijo con el ip de multicast
	xmlNewChild (root, NULL, "multicast", (xmlChar*)multicast);


	// crear un hijo con el tiempo de entrega general

	xmlNewChild (root, NULL, "tiempo_servidor",NULL);

	// crear un hijo con el tiempo de entrega general
	sprintf(buffer,"%d",tiempo);
	xmlNewChild (root, NULL, "tiempo",buffer);

	// crear un hijo con el delta tiempo de entrega
	sprintf(buffer,"%d",dt);
	xmlNewChild (root, NULL, "delta_tiempo",buffer);

	// direccion puerto
	sprintf(buffer,"%d",puerto);
	xmlNewChild (root, NULL, "puerto",buffer);


	xmlChar *result=NULL;
	int len;
	// guarda en memoria el documento en el buffer result y la longitud en len
	xmlSaveFile("../conf_cliente.xml", *doc);
	xmlDocDumpMemory(*doc, &result, &len);
	return result;
  
}



void modificar_xml_cliente(xmlChar *res){
	xmlNodePtr root,aux;
	xmlDocPtr cliente,aux_doc;
	char buffer[50];
	cliente = xmlParseFile("../conf_cliente.xml");
	if(!cliente){
		cliente = xml_nuevo_doc("configuracion");
	}
	root = xmlDocGetRootElement(cliente);
	int len = strlen(res);
    aux_doc = xmlParseMemory(res,len);
	aux = xmlDocGetRootElement(aux_doc);
	modificar_xml(root,"ip_servidor",obtener_valor_xml(aux,"ip"));
	modificar_xml(root,"multicast",obtener_valor_xml(aux,"multicast"));
	modificar_xml(root,"puerto",obtener_valor_xml(aux,"puerto"));
	modificar_xml(root,"tiempo",obtener_valor_xml(aux,"tiempo_host"));
	modificar_xml(root,"delta_tiempo",obtener_valor_xml(aux,"delta_tiempo"));
	modificar_xml(root,"tiempo_servidor",obtener_valor_xml(aux,"tiempo"));
	// guarda en memoria el documento en el buffer result y la longitud en len
	xmlSaveFile("../conf_cliente.xml", cliente);
	xmlFreeDoc(cliente);
	xmlFreeDoc(aux_doc);
}

int configurar_cliente(){
   xmlDocPtr doc;

	struct config_cliente c;

	printf("Direccion IP \n");
	scanf("%s",c.ip_servidor);
	printf("Direccion multicast \n");
	scanf("%s",c.multicast);
	printf("Direccion de puerto \n");
	scanf("%d",&c.puerto);
	printf("Tiempo de envío de datos \n");
	scanf("%d",&c.tiempo);
	printf("Tiempo de tolerancia en envío de datos en centecimas \n");
	scanf("%d",&c.delta_tiempo);
       
    /*los campos faltantes tanto el tiempo del servidor y la mac address se configuran al momento de la conexion*/

    printf("%s",construir_xml_configuracion_cliente(&doc,c.ip_servidor,c.multicast, c.tiempo,c.delta_tiempo,c.puerto));
	xmlFreeDoc(doc);
    return 0;
}