#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>		//Manejo de señales


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libxml/parser.h>  /*trabaja con los ficheros xml*/
#include <libxml/tree.h>    /*trabaja con el árbol generado de parsear el xml*/
#include <libxml/xmlmemory.h>


#include "conf_servidor.h"
#include "../funciones.h"



int procesar_ciclo(xmlDocPtr *doc,struct nodo_lista **p,struct nodo_lista **u,struct config_serv c,struct lista_mac **pmac,struct lista_mac **rp,struct lista_mac **ru, int *restaurar_lista, int sd){

	char documento[LONG_DOCUMENTO];
	xmlChar *resultado_orden=NULL;
	int xd,xc;
	struct lista_mac *aux=NULL;
	char buffer[P_SIZE];
	struct pirlh *paquete;
	struct sockaddr *cliente;
	

	paquete = (struct pirlh *)buffer;

	memset(documento,0,strlen(documento));

	xmlDocDumpFormatMemory(*doc, &resultado_orden, &xd,0);
	strcpy(documento,resultado_orden);



	if (armar_xml_orden(documento,p,u,c.dir_datos_host) < 0){
		perror("\n error al armar los listas y archivos ordenados \n ");
	}

	xmlFree(resultado_orden);
	xmlFreeDoc(*doc);

	xmlCleanupGlobals();

	crear_xml_servidor(doc);


	xc = sizeof(cliente);

	lista_ip_eco(pmac,p,rp,ru);
	aux = *rp;
	while (aux !=NULL){
	   //responde eco con oper=21
	   paquete->operation = 21;
	   strcpy(paquete->msg,aux->nodo.ip);
	   paquete->leng = strlen(paquete->msg);
	   printf("enviando eco  %s \n",paquete->msg);
	   sendto ( sd , buffer , P_SIZE ,0, (struct sockaddr *)  &cliente , xc );

	   aux = aux->siguiente;
	}
	eliminar_lista_mac(rp);
	eliminar_lista(p);
	if (*restaurar_lista > 500){
		eliminar_lista_mac(pmac);
		*restaurar_lista = 0;
	}

	return 0;

}


/* configurar el archivo de configuracion por consola*/
int configurar_por_consola(int c,char *arg){
	if(c > 1){
	    if (strcmp(arg,"-config") == 0){
		printf("configuracion\n");
		if (configurar_servidor() != 0){
			perror("\n error al configurar servidor por consola \n");
		}
		exit(1);
	    }else{
		perror("\n parametro desconocido, ingrese: -config \n");
		exit(0);
	    }
	}
	return 0;

}



/* selector de operaciones */
int selector_de_operaciones(xmlDocPtr doc, struct pirlh *paquete,struct lista_mac **pmac,struct lista_mac **umac){
	switch(paquete->operation){
		case 01:{/* responder el multicast del cliente con los datos para el cliente oper=20
			  le envia los datos de configuracion */
			struct nodo_mac nm;


			printf("EL MENSAJE COMPLETO QUE LLEGA ES %s \n",paquete->msg);
			split_mac_ip(paquete->msg,",",nm.ip,nm.mac);

			int orden = buscar_en_lista_mac(*pmac,nm.ip,nm.mac);

			printf("LA ORDEN ES %d \n",orden);
			if ( orden == 0 ||orden == 1 ){

				agregar_elemento_mac(pmac,umac,nm);

				paquete->operation = 20;
				printf("informacion de servidor %s \n",(char *)obtener_xml_servidor());
				strcpy(paquete->msg,(char *)obtener_xml_servidor());	
				paquete->leng = strlen(paquete->msg);
			}else if(orden != 2){
				paquete->operation = 22;
				strcpy(paquete->msg,"");
				paquete->leng = strlen(paquete->msg);
			}else if(orden == 2){
				paquete->operation = 20;
				printf("informacion de servidor %s \n",(char *)obtener_xml_servidor());
				strcpy(paquete->msg,(char *)obtener_xml_servidor());
			}

		}
		break;
		case 02:{
				struct nodo_mac nm;
		 	        split_mac_ip(paquete->msg,",",nm.ip,nm.mac);
				agregar_elemento_mac(pmac,umac,nm);

				if (paquete->leng == strlen(paquete->msg)){
					printf("\n ok \n");
				}

			}
			break;
		case 03:{ 
				/* agrega los datos al servidor */
				agregar_xml_servidor(doc,(xmlChar*)paquete->msg); 
			}
			break;
		default:
			break;

	    } 
}




/* funcion principal*/

int main(int cantidad, char* argumentos[]) {
	xmlDocPtr doc;
	struct nodo_lista *p, *u;
	p = NULL; u = NULL;
	struct lista_mac *pmac, *umac;
	pmac = NULL; umac = NULL;
	/* para modificar el original*/
	u_int si=1; 
	int sd,restaurar_lista,len;
	socklen_t lon;
	char buffer[P_SIZE];
	struct sigaction ss;  /* Controlar las señales */
	struct sockaddr_in server;
	struct sockaddr_in cliente;
	struct ip_mreq mreq; /* sirve para decirle al kernel que estoy interesado en el grupo */
	struct pirlh *paquete;
	struct config_serv conf;

	/* variables para controlar el tiempo de entrega de los nodos*/
	struct timeval t_ini, t_fin;
	double secs;
	struct lista_mac *ru=NULL,*rp=NULL;

	/*termina definición de variables*/
	/* configurar por consola */
	configurar_por_consola(cantidad,argumentos[1]);
	/*construir xml configuracion del servidor*/
	if ( obtener_conf_servidor(&conf) == 0){
		printf("\n ip %s \n multicas %s \n puerto %d \n tiempo %d \n tiempo_delta %d \n dir_datos %s \n",conf.ip,conf.multicast,conf.puerto,conf.tiempo,conf.delta_tiempo,conf.dir_datos_host);
	}

	/* obtener el descriptor */
	sd = socket ( PF_INET , SOCK_DGRAM , IPPROTO_UDP );
	/* inicio de configuracion multicast */

	if (setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&si,sizeof(si)) < 0) {
		perror("Fallo direccion de multicast");
		exit(1);
	}

	/* configuracion de direccion de destino */
	memset(&server,0,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=htonl(INADDR_ANY);
	server.sin_port=htons(conf.puerto);
	/* asociar el socket con el SO*/
	bind ( sd , (struct sockaddr *)  &server , sizeof (server) );
	/* setsockopt() le avisa al kernel que se una a un grupo multicast */
	mreq.imr_multiaddr.s_addr=inet_addr(conf.multicast);
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);

	if (setsockopt(sd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
		perror("setsockopt\n");
		exit(1);
	}
	/* creamos el documento temporal para los recursos que llegan */
	crear_xml_servidor(&doc);   
	/* tiempo inicial */
	gettimeofday(&t_ini, NULL);
	/* Establecemos la funcion sin_respuestas como manejadora de la señal SIGALRM */
	ss.sa_handler=noresponde;
	ss.sa_flags=SA_ONESHOT;

	if(sigaction(SIGALRM, &ss, NULL) == -1){
	perror("\n Error al establecer manejadora de señal: 1\n");
	}
	lon = sizeof (cliente);
	restaurar_lista = 0;
	while (1) {
		/* Si no recibimos respuestas, la señal SIGALRM nos desbloqueara de recvfrom tras timeout 			segundos */
		alarm(conf.tiempo);
		memset(buffer,0,strlen(buffer));
		if (recvfrom ( sd , buffer , P_SIZE ,0, (struct sockaddr *)  &cliente ,  &lon ) < 0){
			perror("\nError al recibir datos\n");
		}
		ss.sa_handler=SIG_IGN;		//Si recibimos algo
		ss.sa_flags=SA_ONESHOT;
		if(sigaction(SIGALRM, &ss, NULL) == -1){
			perror("\nError estableciendo manejadora de señal 2:\n");
		}
		/* realizamos cast del paquete al buffer */
		paquete = (struct pirlh *) buffer;
		/* tomamos el tiempo de finalizacion */
		gettimeofday(&t_fin, NULL);
		/* calculamos diferencia de tiempo */
		secs = timeval_diff(&t_fin,&t_ini);
		if(secs >= conf.tiempo   + conf.delta_tiempo/1000){
			/* armo los xml de los recursos ordenados 
			   si la lista de nodos que entregaron difiere con la de activos enviar hello! */
			if(doc){
				procesar_ciclo(&doc,&p,&u,conf,&pmac,&rp,&ru,&restaurar_lista,sd);
				restaurar_lista++;
			}
			/* volver a tomar tiempo inicial */
			gettimeofday(&t_ini, NULL);
		}
     	/* Establecemos la funcion sin_respuestas como manejadora de la señal SIGALRM */
		ss.sa_handler=noresponde;		
		ss.sa_flags=SA_ONESHOT;
		if(sigaction(SIGALRM, &ss, NULL) == -1){
			perror("\nError estableciendo manejadora de señal 2:\n");
		}
   
		/* cast del buffer en estructura pirlh */
		printf("ESTA ES EL NUMERO DE LA OPERACION %d \n",paquete->operation);	

		/* a traves de paquete lleva la operacion a procesar*/
		selector_de_operaciones(doc,paquete,&pmac,&umac);

		/* envia mensaje segun el contenido de paquete*/
		sendto ( sd , buffer , P_SIZE ,0, (struct sockaddr *)  &cliente , lon );
		memset(buffer,0,strlen(buffer));
    }
   	return 0;
}