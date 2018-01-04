#include <glibtop.h>
#include <glibtop/cpu.h> 
#include <glibtop/mem.h> 
#include <glibtop/proclist.h> 
#include <glibtop/swap.h> 
#include <glibtop/sysinfo.h>

/* estructura para obtener los recursos del host*/
struct recursos {
	glibtop_cpu cpu; 
	glibtop_mem memory; 
	glibtop_swap swap;
};

int obtener_recursos(struct recursos *r);
void listar_recursos(struct recursos *r);