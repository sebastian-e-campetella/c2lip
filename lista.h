
/* muestra la lista con los recursos */
void mostrar_lista(struct nodo_lista *primero);

/* eliminar lista completa de informacion de recursos */
int eliminar_lista(struct nodo_lista **primero);

/* segun el recurso se ordena la lista */
void ordenar_lista(struct nodo_lista **primero, char *recurso);

// devuelve 0 sino existe, sino devuelve 1
int buscar_en_lista(struct nodo_lista *primero,char *dato);

/*lista para verificar la ip unica*/
void agregar_elemento_mac(struct lista_mac **p,struct lista_mac **u, struct nodo_mac nm);

/* devuelve 0 sino existe, sino devuelve 1 */
int buscar_en_lista_mac(struct lista_mac *primero,char ip[100], char mac[100]);

/* elimina toda la lista con la informacion de las mac*/
int eliminar_lista_mac(struct lista_mac **primero);

/* mostrar lista de direcciones mac */
void mostrar_lista_mac(struct lista_mac *primero);

/* ordena la lista de direcciones mac */
void ordenar_lista_mac(struct lista_mac **primero);

/* devuelve una lista con las direcciones a hacer eco hace una diferencia entre la lista de nodos que 
	enviaron datos y los que hay en la lista temporal de nodos conocidos
 */

int lista_ip_eco(struct lista_mac **pmac, struct nodo_lista **p, struct lista_mac **pn,struct lista_mac **nu);

/* mostrar la lista de ip de nodos */
void mostrar_lista_ip(struct lista_mac *lista);
