#include "funciones.h"
 
/* crea la estructura xmlDoc con nodo raiz "name" */

xmlDocPtr xml_nuevo_doc (const gchar *nombre) {

	xmlNodePtr root;
	xmlDocPtr doc;
	doc = xmlNewDoc("1.0");
	root = xmlNewDocNode (doc, NULL, nombre, NULL);
	xmlDocSetRootElement (doc, root);

//	printf("\n Documento Creado %s \n",nombre);
	return doc;
}

/* agrega a un puntero de documento la informacion del microprocesador */
void xml_nueva_entrada_cpu(xmlDocPtr *doc, glibtop_cpu nuevo) {
	xmlNodePtr root;
	xmlNodePtr cpu;
	char buffer[50];

	/* nodo raiz */
	root = xmlDocGetRootElement (*doc);
	cpu = xmlNewChild (root, NULL, "cpu", NULL);

	xmlNewChild (cpu, NULL, "unit", "Cicles/s");

	sprintf(buffer,"%ld",nuevo.flags);
	xmlNewChild (cpu, NULL,"flags", buffer);

	sprintf(buffer,"%ld",nuevo.total);
	xmlNewChild (cpu, NULL,"total", buffer);

	sprintf(buffer,"%ld",nuevo.user);
	xmlNewChild (cpu, NULL,"user", buffer);

	sprintf(buffer,"%ld",nuevo.nice);
	xmlNewChild (cpu, NULL,"nice", buffer);

	sprintf(buffer,"%ld",nuevo.sys);
	xmlNewChild (cpu, NULL,"sys", buffer);

	sprintf(buffer,"%ld",nuevo.idle);
	xmlNewChild (cpu, NULL,"idle", buffer);

	sprintf(buffer,"%ld",nuevo.iowait);
	xmlNewChild (cpu, NULL,"iowait", buffer);

	sprintf(buffer,"%ld",nuevo.irq);
	xmlNewChild (cpu, NULL,"irq", buffer);

	sprintf(buffer,"%ld",nuevo.softirq);
	xmlNewChild (cpu, NULL,"softirq", buffer);

	sprintf(buffer,"%ld",nuevo.frequency);
	xmlNewChild (cpu, NULL,"frequency", buffer);

/*	xmlFreeNode(root);
	xmlFreeNode(cpu);
*/
}

/* agrega a un puntero de documento la informacion de la memoria ram */
void xml_nueva_entrada_ram(xmlDocPtr *doc, glibtop_mem nuevo) {
	xmlNodePtr root;
	xmlNodePtr ram;
	char buffer[50];

	/* nodo raiz */
	root = xmlDocGetRootElement (*doc);

	ram = xmlNewChild (root, NULL, "ram", NULL);

	xmlNewChild (ram, NULL, "unit", "Byte's");

	sprintf(buffer,"%ld",nuevo.flags);
	xmlNewChild (ram, NULL, "flags",buffer);

	sprintf(buffer,"%ld",nuevo.total);
	xmlNewChild (ram, NULL, "total", buffer);

	sprintf(buffer,"%ld",nuevo.used);
	xmlNewChild (ram, NULL, "used", buffer);

	sprintf(buffer,"%ld",nuevo.free);
	xmlNewChild (ram, NULL, "free",buffer);

	sprintf(buffer,"%ld",nuevo.shared);
	xmlNewChild (ram, NULL, "shared", buffer);

	sprintf(buffer,"%ld",nuevo.buffer);
	xmlNewChild (ram, NULL, "buffer", buffer);

	sprintf(buffer,"%ld",nuevo.cached);
	xmlNewChild (ram, NULL, "cached", buffer);

	sprintf(buffer,"%ld",nuevo.user);
	xmlNewChild (ram, NULL, "user", buffer);

	sprintf(buffer,"%ld",nuevo.locked);
	xmlNewChild (ram, NULL, "locked", buffer);
}

/* agrega a un puntero de documento la informacion de la memoria de intercambio */

void xml_nueva_entrada_swap(xmlDocPtr *doc, glibtop_swap nuevo) {
	xmlNodePtr root;
	xmlNodePtr swap;
	char buffer[50];

	/* nodo raiz */
	root = xmlDocGetRootElement (*doc);

	swap = xmlNewChild (root, NULL, "swap", NULL);

	xmlNewChild (swap, NULL, "unit", "Byte's");

	sprintf(buffer,"%ld",nuevo.flags);
	xmlNewChild (swap, NULL, "flags",buffer);

	sprintf(buffer,"%ld",nuevo.total);
	xmlNewChild (swap, NULL, "total", buffer);

	sprintf(buffer,"%ld",nuevo.used);
	xmlNewChild (swap, NULL, "used", buffer);

	sprintf(buffer,"%ld",nuevo.free);
	xmlNewChild (swap, NULL, "free", buffer);

	sprintf(buffer,"%ld",nuevo.pagein);
	xmlNewChild (swap, NULL, "pagein", buffer);

	sprintf(buffer,"%ld",nuevo.pageout);
	xmlNewChild (swap, NULL, "pageout", buffer);
}

// construcye todo el documento xml del cliente que enviará al servidor sigue con la funcion ver_plano
int construir_xml_cliente( char *ip, char b[LONG_RECURSOS]){

	xmlDocPtr doc;
	xmlNodePtr root;
	struct recursos r;

	// obtengo los recursos del sistema
	obtener_recursos(&r);
	// crear el nuevo documento
	doc = xml_nuevo_doc("Host");
	root = xmlDocGetRootElement(doc);
	// crear un hijo con el ip del host
	xmlNewChild (root,NULL,  "ip", (xmlChar*)ip);
	// crear un hijo con los datos de cpu del host	
	xml_nueva_entrada_cpu(&doc,r.cpu);
	// crear un hijo con los datos de la memoria ram del host	
	xml_nueva_entrada_ram(&doc,r.memory);
	// crear un hijo con los datos de swap del host	
	xml_nueva_entrada_swap(&doc,r.swap);

	xmlChar *result=NULL;
	int len;
	// guarda en memoria el documento en el buffer result y la longitud en len
	xmlDocDumpFormatMemory(doc, &result, &len,0);

	//copio el string a la cadena por referencia
	strcpy(b,result);

	xmlFree(result);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 0;
}

/* modificar xml existente*/

int modificar_xml(xmlNodePtr aux_root, char *padre, xmlChar *dato){
	  aux_root = aux_root->xmlChildrenNode;
	  while (aux_root != NULL) {
	    if (xmlChildElementCount(aux_root) == 0){ 	
		    if (strcmp(aux_root->name,padre) == 0) {
			xmlNodeSetContent(aux_root,dato);
			return 0; 
		    }
	    }else  if ( xmlChildElementCount(aux_root) > 0) {
			modificar_xml(aux_root,(xmlChar *)aux_root->name,dato);
	    }
	    aux_root = aux_root->next;
	  }
	  return 1;
}
/*fin modificar xml*/


/* obtener valor de un xml */
xmlChar* obtener_valor_xml(xmlNodePtr aux_root, char *padre){
      xmlNodePtr aux;
	  aux = aux_root->xmlChildrenNode;

	  while (aux != NULL) {

	    if (xmlChildElementCount(aux) == 0){ 	
		    if (strcmp(aux->name,padre) == 0) {
			return xmlNodeGetContent(aux);
		    }else{
			obtener_valor_xml(aux,(xmlChar *)aux->name);
		    }
	    }else  if ( xmlChildElementCount(aux) > 0) {
			obtener_valor_xml(aux,(xmlChar *)aux->name);
	    }
	    aux = aux->next;
	  }
}

/* Obtener informacion de configuracion del servidor */
int obtener_conf_servidor(struct config_serv *cs){
	xmlDocPtr doc;
	xmlNodePtr serv;
	doc = xmlParseFile("../conf_servidor.xml");
	if (!doc) {
	    printf("Error al cargar configuracion del servidor XML\n");
	    return -1;
	}else{
		serv = xmlDocGetRootElement(doc);

		if(serv != NULL){

			strcpy(cs->ip,(char *)obtener_valor_xml(serv,"ip") );
			strcpy(cs->multicast, (char *)obtener_valor_xml(serv,"multicast") );
			cs->tiempo = atoi( obtener_valor_xml(serv,"tiempo") );
			cs->delta_tiempo = atoi( obtener_valor_xml(serv,"delta_tiempo") );
			cs->puerto = atoi( obtener_valor_xml(serv,"puerto") );
			strcpy(cs->dir_datos_host, obtener_valor_xml(serv,"dir_datos_host") );
		}
		xmlFreeDoc(doc);
		return 0;
    }
}

/* obtener xml result de la configuracion del servidor */
xmlChar* obtener_xml_servidor(){
	xmlDocPtr doc;

	doc = xmlParseFile("../conf_servidor.xml");
	if (!doc) {
	    printf("Error al cargar configuracion del servidor XML\n");
	    return NULL;
	}else{
	    xmlChar *result=NULL;
	    int len;
	    // guarda en memoria el documento en el buffer result y la longitud en len
	    xmlDocDumpFormatMemory(doc, &result, &len,0);
	     
            xmlFreeDoc(doc);
	    return result;
	}
}

/* obtener xml result de la configuracion del cliente */
xmlChar* obtener_xml_cliente(){
	xmlDocPtr doc;

	doc = xmlParseFile("../conf_cliente.xml");
	if (!doc) {
	    printf("Error al cargar configuracion del servidor XML\n");
	    return NULL;
	}else{
	    xmlChar *result=NULL;
	    int len;
	    // guarda en memoria el documento en el buffer result y la longitud en len
	    xmlDocDumpFormatMemory(doc, &result, &len,0);
	    xmlFreeDoc(doc) ;
	    return result;
	}
}

/* Obtener informacion de configuracion del cliente */

int obtener_conf_cliente(struct config_cliente *cs){
	
	xmlDocPtr doc;
	xmlNodePtr cli;
	doc = xmlParseFile("../conf_cliente.xml");
	if (!doc) {
	    printf("Error al cargar configuracion del cliente XML\n");
	    return -1;
	}else{
		cli = xmlDocGetRootElement(doc);
		if(cli != NULL){
			strcpy(cs->ip_servidor,(char *) obtener_valor_xml(cli,"ip_servidor") );
			strcpy(cs->multicast, (char *)obtener_valor_xml(cli,"multicast") );
			cs->delta_tiempo = atoi( obtener_valor_xml(cli,"delta_tiempo") );
			cs->tiempo_servidor = atoi( obtener_valor_xml(cli,"tiempo_servidor") );
			cs->puerto = atoi( obtener_valor_xml(cli,"puerto") );
			cs->tiempo = atoi( obtener_valor_xml(cli,"tiempo") );
		}
		xmlFreeDoc(doc);
        return 0;
	}
}

/* Obtener informacion de configuracion del cliente por medio de cadena */

int obtener_conf_cliente_cadena(struct config_cliente *cs, xmlChar *res){
	xmlDocPtr doc;
	xmlNodePtr cli;

	int len = strlen(res);
        doc = xmlParseMemory(res,len);

	if (!doc) {
	    printf("Error al cargar configuracion del cliente XML\n");
	    return -1;
	}else{
		cli = xmlDocGetRootElement(doc) ;
		if(cli != NULL){
			strcpy(cs->ip_servidor,(char *) obtener_valor_xml(cli,"ip_servidor") );
			strcpy(cs->multicast, (char *)obtener_valor_xml(cli,"multicast") );
			cs->tiempo = atoi( obtener_valor_xml(cli,"tiempo") );
			cs->delta_tiempo = atoi( obtener_valor_xml(cli,"delta_tiempo") );
			cs->tiempo_servidor = atoi( obtener_valor_xml(cli,"tiempo_servidor") );
			cs->puerto = atoi( obtener_valor_xml(cli,"puerto") );
		}
		xmlFreeDoc(doc);
		xmlCleanupParser();
        return 0;
	}
}

/* Obtener informacion de configuracion del servidor por medio de cadena */

int obtener_conf_servidor_cadena(struct config_serv *cs, xmlChar *res){
	xmlDocPtr doc;

	xmlNodePtr serv;

	int len = strlen(res);
        doc = xmlParseMemory(res,len);
	if (!doc) {
	    printf("Error al cargar configuracion del servidor XML\n");
	    return -1;
	}else{
		serv = xmlDocGetRootElement(doc) ;

		if(serv != NULL){
			strcpy(cs->ip,(char *)obtener_valor_xml(serv,"ip") );
			strcpy(cs->multicast, (char *)obtener_valor_xml(serv,"multicast") );
			cs->tiempo = atoi( obtener_valor_xml(serv,"tiempo") );
			cs->delta_tiempo = atoi( obtener_valor_xml(serv,"delta_tiempo") );
			cs->puerto = atoi( obtener_valor_xml(serv,"puerto") );
			strcpy(cs->dir_datos_host, obtener_valor_xml(serv,"dir_datos_host") );
		}
		xmlFreeDoc(doc);	  
		return 0;
        }
}

/* Con esta función añadimos un elemento al final de la lista */

 void agregar_elemento(struct nodo_info ni, struct nodo_lista **primero, struct nodo_lista **ultimo) {
      struct nodo_lista *nuevo;
 
      /* reservamos memoria para el nuevo elemento */
      nuevo = (struct nodo_lista *) malloc (sizeof(struct nodo_lista));
      if (nuevo==NULL) printf( "No hay memoria disponible!\n");
 
      nuevo->nodo = ni;
      /* el campo siguiente va a ser NULL por ser el último elemento
         de la lista */
      nuevo->siguiente = NULL;
 
      /* comprobamos si la lista está vacía. si primero==NULL es que no
         hay ningún elemento en la lista. también vale ultimo==NULL */
      if (*primero==NULL) {
         *primero = nuevo;
         *ultimo = nuevo;
         }
      else {
           /* el que hasta ahora era el último tiene que apuntar al nuevo */
           (*ultimo)->siguiente = nuevo;
           /* hacemos que el nuevo sea ahora el último */
           *ultimo = nuevo;
      }
}

/* recorrido recursivo del documento */

void armar_lista(xmlNodePtr aux_root, char *padre, struct nodo_info *a, struct nodo_lista **p, struct nodo_lista **u){
  	xmlNodePtr aux;
	aux_root = aux_root->xmlChildrenNode;
	while (aux_root != NULL) {
	    if (xmlChildElementCount(aux_root) == 0){ 	
		    if( (strcmp(padre,"cpu") == 0) && (strcmp(aux_root->name,"idle") == 0)){
			a->cpu = atol(xmlNodeGetContent(aux_root));
		    }else if( (strcmp(padre,"ram") == 0) && (strcmp(aux_root->name,"free") == 0)){
			a->mem = atol(xmlNodeGetContent(aux_root));
		    }else if( (strcmp(padre,"swap") == 0) && (strcmp(aux_root->name,"free") == 0)){
			a->swap = atol(xmlNodeGetContent(aux_root));
		    }else if( (strcmp(padre,"host") == 0) && (strcmp(aux_root->name,"ip") == 0)){
			strcpy(a->ip, xmlNodeGetContent(aux_root));
		    }
		    if ( (strcmp(a->ip,"") != 0) && (a->mem > -1)  && (a->cpu > -1)  && (a->swap > -1)){
			aux = aux_root;
			/* aunque haya llegado mas de un paquete de un mismo host para un procesamiento 
				se toma uno */
			if(buscar_en_lista(*p,a->ip) == 0){
				agregar_elemento(*a,p,u);
			}
			// setear valores de inicio
 			strcpy(a->ip,""); a->mem = -1;a->swap = -1;a->cpu = -1;
		    }
	    }else  if ( xmlChildElementCount(aux_root) > 0) {
			armar_lista(aux_root,(xmlChar *)aux_root->name,a,p,u);
	    }
	    aux_root = aux_root->next;
	}
}
/* fin otra version */
/* fin del recorrido recursivo del documento */

/* crear documentos xml ordenados a partir de las listas */

void crear_xml_listas(struct nodo_lista **p,char *tipo_recurso, char *destino){
	// crear el nuevo documento

	xmlDocPtr doc;
	xmlNodePtr root,host;
	struct nodo_lista *aux;
	char buffer[50];
	aux = *p;
	doc = xml_nuevo_doc(tipo_recurso);
	root = xmlDocGetRootElement(doc);

	while(aux != NULL){
       	host = xmlNewChild (root,NULL,  "host", NULL);
		
		xmlNewChild (host, NULL, "ip", aux->nodo.ip);

		sprintf(buffer,"%ld",aux->nodo.mem);
		xmlNewChild (host, NULL, "mem", buffer);
		sprintf(buffer,"%ld",aux->nodo.swap);
		xmlNewChild (host, NULL, "swap",buffer);
		sprintf(buffer,"%ld",aux->nodo.cpu);
		xmlNewChild (host, NULL, "cpu",buffer);
		
		aux = aux->siguiente;
	}
	char ruta[300] = "../";
	strcpy(ruta,destino);
	// guarda en memoria el documento en el buffer result y la longitud en len
	strcat(ruta,tipo_recurso);
	strcat(ruta,".xml");
	xmlSaveFile(ruta, doc);

	xmlFreeDoc(doc);
	xmlCleanupGlobals();
}

/* crear listas ordenadas por recurso*/

void ordenar_listas_xml(char res[LONG_DOCUMENTO], struct nodo_lista **p, struct nodo_lista **u, char *destino){

	xmlDocPtr doc;
	xmlNodePtr root,hijos,aux_root;
	struct nodo_info a;	

	if(strlen(res) != 0){
		/* recorro los datos de los nodos llegados y los inserto en el documento*/
		int len = strlen(res);

		doc = xmlParseMemory((xmlChar *)res,len);
		if(doc){
			root = xmlDocGetRootElement(doc);
			aux_root = xmlDocGetRootElement(doc);
			root = root->xmlChildrenNode;
			char *padre = "";
			strcpy(a.ip,"");
			a.mem = -1;	
			a.cpu = -1;
			a.swap = -1;

			armar_lista(aux_root,padre,&a,p,u);

			printf("listas por orden de recurso\n");
			ordenar_lista(p,"cpu");
			crear_xml_listas(p,"cpu",destino);
			ordenar_lista(p,"swap");
			crear_xml_listas(p,"swap",destino);
			ordenar_lista(p,"mem");
			crear_xml_listas(p,"mem",destino);

			/*liberar memoria auxiliar */
			xmlFreeDoc(doc);
			xmlCleanupParser();
		}
	}
}
/* fin listas ordenadas*/
// crea el documento xml del servidor 
void crear_xml_servidor(xmlDocPtr *doc){
	// crear el nuevo documento
	*doc = xml_nuevo_doc("Hosts");
}

/*funcion donde agrega la informacion al xml temporal */
/*aqui es donde se produce una perdida de memoria 
  aumque se utilizan las funciones para la liberacion de la memoria
  usada por aux_doc no libera el espacio.
*/

int agregar_xml_servidor(xmlDocPtr doc,char res[LONG_DOCUMENTO]){

	xmlDocPtr aux_doc;
	xmlNodePtr aux_root,root,hijos,host,aux;

	root = xmlDocGetRootElement(doc);

	/* recorro los datos del nodo llegado y los inserto en el documento*/
	int aux_len = strlen(res);
	aux_doc = xmlParseDoc(res);
	aux_root = xmlDocGetRootElement (aux_doc);
	
	aux_root = aux_root->xmlChildrenNode;
  	host = xmlNewChild (root,NULL,  "host", NULL);
	while (aux_root != NULL) {
		if (xmlChildElementCount(aux_root) == 0){ 	
		    xmlNewChild (host,NULL,aux_root->name , xmlNodeGetContent(aux_root));
		}

		if ( xmlChildElementCount(aux_root) > 0) {
			aux = xmlNewChild (host,NULL,aux_root->name , NULL);
			hijos = aux_root->xmlChildrenNode;
			while (hijos != NULL) {
			    xmlNewChild (aux,NULL,hijos->name , xmlNodeGetContent(hijos));
			    hijos = hijos->next;
			}
		}
		aux_root = aux_root->next;
	}
	xmlFreeDoc(aux_doc);
	xmlCleanupParser();
	return 0;
}

/* arma los archivos xml de los recursos e iniciaializa la lista  */
int armar_xml_orden(char res[LONG_DOCUMENTO], struct nodo_lista **p,struct nodo_lista **u,char *destino){
	int i;
	ordenar_listas_xml(res,p,u,destino);
	mostrar_lista(*p);
	return 0;
}
/**/

/*devuelve la diferencia de los tiempos */
double timeval_diff(struct timeval *a, struct timeval *b)
{
    double temp;
    temp = (double)(a->tv_sec + (double)a->tv_usec/1000000) - (double)(b->tv_sec + (double)b->tv_usec/1000000);
    return temp;
}

/* utilizada para el temporizador */
void noresponde(){

}

/* separa la cadena en dos, con la direccion IP y MAC */
void split_mac_ip(char *cadena,char *separador,char ip[100],char mac[100]){
   char aux[200];
   char a[100],b[100];
   strcpy(aux,cadena);
   strcpy(a,(char *)strtok( aux, "," )); 
   strcpy(mac,a); 
   strcpy(b,(char *)strtok( NULL, "," ));
   strcpy(ip,b); 
}