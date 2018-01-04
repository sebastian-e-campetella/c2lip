#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>		//Manejo de señales
#include <sys/ioctl.h>
#include <net/if.h>
#include "conf_cliente.h"
#include "../funciones.h"
 
int enviarUDP (struct sockaddr_in server, char *buffer, int sd, socklen_t lon){
	return sendto (sd, buffer, P_SIZE, 0, (struct sockaddr *) &server, lon);
} 
 
/* envia los recursos al servidor */
int enviarrecursos (struct sockaddr_in server, int sd, socklen_t lon,struct nodo_mac m){
  	/*preparar los recursos a enviar*/ 
	char buffer[P_SIZE];
	char recursos[LONG_RECURSOS];
	struct pirlh *dato;
	/* cast de la estructura a buffer para enviarlo */
	dato = (struct pirlh *) buffer;
	dato->operation = 03;
	
	/* copia en la cadena los recursos obtenidos en formato xml por la funcion */
	memset(recursos,0,strlen(recursos));
	if (construir_xml_cliente (m.ip,recursos) ==  0){
		if (strcpy (dato->msg,recursos) < 0 ){
			perror("\n error al obtener y copiar los recuros \n");
		}
		dato->leng = strlen (dato->msg);
  
		printf("\n Enviando mensaje: \n %s \n",dato->msg);

		if ( enviarUDP (server, buffer, sd, lon) < 0){
			perror("\n error al enviar mensaje \n");
		}else{
			memset(buffer,0,P_SIZE);
		}
	}
	/* fin preparar dato a enviar */ 
	return 0;
}
 
 /* se encarga de realizar las tareas segun el numero de operacion que hay llegado */
int operaciones (struct sockaddr_in server, int sd, socklen_t lon,struct pirlh *dato, double *ter, struct config_cliente *c, struct nodo_red *nr){
  	char buffer[P_SIZE];
	switch (dato->operation){
    	case 20:{ 
			/* configuro el cliente con los datos recibidos del servidor */
			if (dato->leng == strlen(dato->msg)){	
			    printf("---------el mensaje es:--------\n %s \n",(char *)dato->msg);
			    modificar_xml_cliente((xmlChar *)dato->msg);
			    if ( obtener_conf_cliente(c) < 0){
				perror("\n Error al obtener configuracion inicial del cliente \n");
			    }
			    *ter = c->tiempo + ( (c->delta_tiempo/1000) * drand48());		       	 
			}else{
				perror("\n Error al recibir configuracion del servidor \n");

			}
			}
			break;
	    		
		case 21:{
			/* enviar eco con la IP y la MAC address */
			if (dato->leng = strlen (dato->msg)){
				    memset(buffer,0,P_SIZE);
				    dato = (struct pirlh *) buffer;
                                    dato->operation = 02;
				    strcpy(dato->msg,(char *)nr->red);
				    dato->leng = strlen(dato->msg);
				    sendto (sd, buffer, P_SIZE, 0, (struct sockaddr *) &server, lon);
			}
	      
			} 
			break;
	    
		case 22:{
				printf("cambiar de ip, se cierra el socket \n");
				close(sd);
				exit(0);

			}
			break;
	}
	return 0;
}

void preparar_ip_mac(int sd, struct ifreq ifr, struct nodo_red *nr){
    /*establecemos parametros para obtener la MAC */
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

    ioctl(sd, SIOCGIFHWADDR, &ifr);
    sprintf(nr->red, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
			(unsigned char)ifr.ifr_hwaddr.sa_data[0],
			(unsigned char)ifr.ifr_hwaddr.sa_data[1],
			(unsigned char)ifr.ifr_hwaddr.sa_data[2],
			(unsigned char)ifr.ifr_hwaddr.sa_data[3],
			(unsigned char)ifr.ifr_hwaddr.sa_data[4],
			(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
    strcat(nr->red,",");
    ioctl(sd, SIOCGIFADDR, &ifr);
    strcat(nr->red,inet_ntoa(((struct sockaddr_in *) &(ifr.ifr_addr))->sin_addr));
}
 
int main(int cantidad, char* argumentos[]){
    int sd;
    socklen_t lon;
    char buffer[P_SIZE];
    char buffer_msg[2000];
    struct sockaddr_in server;
    struct pirlh *dato, *aux;
    struct nodo_mac nmac;
    struct config_cliente conf_cli;
    struct sigaction ss;	//Controlar las señales
    struct timeval t_ini, t_fin;
    struct nodo_red nr; // info de ip y mac
    struct ifreq ifr;  // obtenemos el dato de la MAC ADDRESS
    double secs=0,tiempo_enviar_recursos;
    int enviado=0;  
 
    if(cantidad > 1){
	    if (strcmp(argumentos[1],"-config") == 0){
		printf("configuracion\n");
		configurar_cliente();
		exit(1);
	    }else{
		perror("\n parametro desconocido, ingrese: -config \n");
		exit(0);
	    }
    }

    if ( obtener_conf_cliente(&conf_cli) < 0){
	    perror("\n Error al obtener configuracion inicial del cliente \n");
    }
    printf("\n %s %s %d %d %d %d \n",conf_cli.ip_servidor,conf_cli.multicast,conf_cli.puerto,conf_cli.tiempo,conf_cli.delta_tiempo,conf_cli.tiempo_servidor);
    memset (&server, 0, sizeof (server));
    server.sin_family = AF_INET;
    server.sin_port = htons (conf_cli.puerto);
 //          server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_addr.s_addr = inet_addr (conf_cli.multicast);
    sd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    lon = sizeof (server);
    memset(buffer_msg,0,2000);  
    preparar_ip_mac(sd, ifr,&nr);
    strcpy(buffer_msg,(char *)&nr);
    memset(buffer,0,P_SIZE);
    aux = (struct pirlh *) buffer;
    aux->operation = 01;
    strcpy(aux->msg,buffer_msg);
    aux->leng = strlen(aux->msg);
    printf ("operacion %d \n",aux->operation);
    printf ("msg %s \n",(char *)aux->msg);
    printf ("leng %d \n",aux->leng);
    split_mac_ip(aux->msg,",",nmac.ip, nmac.mac);
    printf ("\n IP %s \n MAC %s \n",nmac.ip, nmac.mac);
    if (enviarUDP (server, buffer, sd, lon) < 0){
        perror ("error al enviar multicast\n");
	    exit(0);
    }else{
       printf("ENVIO SEÑAL MULTICAST\n");
    }
  
    // le enviamos al servidor directamente ya sin hacer multicast
    server.sin_addr.s_addr = inet_addr(conf_cli.ip_servidor);
    lon = sizeof(server);
 
    //Establecemos la funcion sin_respuestas como manejadora de la señal SIGALRM
    ss.sa_handler = noresponde;	
    ss.sa_flags = SA_ONESHOT;
    if (sigaction (SIGALRM, &ss, NULL) == -1){
      perror ("\nError estableciendo manejadora de señal: 1\n");
    }
    alarm(15);
    if (recvfrom(sd, buffer, P_SIZE, 0, (struct sockaddr *) &server, &lon) < 0){
       perror ("error al recibir datos de configuración \n");
    }
    ss.sa_handler = SIG_IGN;	//Si recibimos algo
    ss.sa_flags = SA_ONESHOT;
    if (sigaction (SIGALRM, &ss, NULL) == -1){
        perror ("\nError estableciendo manejadora de señal 2:\n");
    }
    tiempo_enviar_recursos = conf_cli.tiempo + ( (conf_cli.delta_tiempo/1000) * drand48());
    aux = (struct pirlh *) buffer;
    if(aux->operation != 20){
      perror ("error al recibir configuración \n");
      exit(0);
    }else{
       printf("\n comienza configuración automatica \n");
       operaciones (server, sd, lon, aux,&tiempo_enviar_recursos, &conf_cli,&nr);
    }
    /* tiempo inicial */ 
    gettimeofday (&t_ini, NULL);
    while (1){
    	alarm(conf_cli.tiempo + conf_cli.delta_tiempo/1000);
	    memset(buffer,0,sizeof(buffer));
        if (recvfrom(sd, buffer, P_SIZE, 0, (struct sockaddr *) &server, &lon) < 0){
            perror ("sin datos en este periodo \n");
	
        }else{
            /* cast del buffer en estructura pirlh */ 	  
            dato = (struct pirlh *) buffer;
            operaciones (server, sd, lon, dato,&tiempo_enviar_recursos, &conf_cli,&nr);
        }
        ss.sa_handler = SIG_IGN;	//Si recibimos algo
        ss.sa_flags = SA_ONESHOT; 
        if (sigaction (SIGALRM, &ss, NULL) == -1){
	        perror ("\nError estableciendo manejadora de señal 2:\n");
        }
        gettimeofday(&t_fin, NULL);
    	printf("\n  %f \n",secs);
        secs = timeval_diff(&t_fin, &t_ini);
        if(secs >= (conf_cli.tiempo_servidor) ){
            enviado = 0;
        }
        if ( (secs >= tiempo_enviar_recursos) && enviado == 0){
	    /* envio el xml de los recursos */ 
	    /* si la lista de nodos que entregaron difiere con la de activos enviar hello! */ 
        enviarrecursos (server, sd, lon,nmac);
 	    enviado = 1;
	    /* bloquea el cliente para tratar de no enviar mas de un paquete por periodo */
	    usleep(1);
	    /* tomo el tiempo nuevamente */	
            gettimeofday (&t_ini, NULL);
        }
        ss.sa_handler = noresponde;
        ss.sa_flags = SA_ONESHOT;
    
        if (sigaction (SIGALRM, &ss, NULL) == -1){
            perror ("\nError estableciendo manejadora de señal 2:\n");
        }
    }
    close(sd);
    return 0;
}