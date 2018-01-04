
/* 
 Este modulo se encarga de obtener la información de los recursos
*/ 
#include "recursos.h"
/* variable para la obtencion de recuros*/

int obtener_recursos(struct recursos *r){

/* inicializa el servidor de glibtop*/
/* comentado porque inicializa automaticamente */
//  glibtop *gl = glibtop_init();
  glibtop_get_cpu (&((*r).cpu)); 
  glibtop_get_mem(&((*r).memory)); 
  glibtop_get_swap(&((*r).swap));

  return 0;
}

void listar_recursos(struct recursos *r){
  printf("CPU TYPE INFORMATIONS \n\n" 
         "Cpu Total : %ld \n" 
         "Cpu User : %ld \n" 
         "Cpu Nice : %ld \n" 
         "Cpu Sys : %ld \n" 
         "Cpu Idle : %ld \n" 
         "Cpu Frequences : %ld \n", 
         (unsigned long)(*r).cpu.total, 
         (unsigned long)(*r).cpu.user, 
         (unsigned long)(*r).cpu.nice, 
         (unsigned long)(*r).cpu.sys, 
         (unsigned long)(*r).cpu.idle, 
         (unsigned long)(*r).cpu.frequency); 

  printf("MEMORIA TYPE INFORMATIONS \n\n" 
         "mem Total : %ld \n" 
         "mem Used : %ld \n" 
         "mem free : %ld \n" 
         "mem shared : %ld \n" 
         "mem buffer : %ld \n" 
         "mem cached : %ld \n"
         "mem user : %ld \n" 
         "mem locked : %ld \n",  
         (unsigned long)(*r).memory.total, 
         (unsigned long)(*r).memory.used, 
         (unsigned long)(*r).memory.free, 
         (unsigned long)(*r).memory.shared, 
         (unsigned long)(*r).memory.buffer, 
         (unsigned long)(*r).memory.cached,
         (unsigned long)(*r).memory.user, 
         (unsigned long)(*r).memory.locked );

  printf("MEMORIA SWAP INFORMATIONS \n\n" 
         "mem Banderas : %ld \n" 
         "mem Total : %ld \n" 
         "mem Usado : %ld \n" 
         "mem Libre : %ld \n" 
         "mem Paginado : %ld \n" 
         "mem No paginado : %ld \n",
         (unsigned long) (*r).swap.flags,
         (unsigned long) (*r).swap.total,		
         (unsigned long) (*r).swap.used,		
         (unsigned long) (*r).swap.free,		
         (unsigned long) (*r).swap.pagein,		
         (unsigned long) (*r).swap.pageout);
}

int m(){ 
  struct recursos rec;

  obtener_recursos(&rec);
  listar_recursos(&rec);
  return 0; 
}