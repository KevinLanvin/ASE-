#include <stdlib.h>
#include <stdio.h>
#include "gest_contexte.h"
#include "hardware.h"
#include "hw_config.h"


/*déclaration des fonctions*/





int main(){
	create_ctx(16384,shell,NULL);
	start_sched();

	exit(EXIT_SUCCESS);
}


}


