#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include "gest_contexte.h"
#include "hardware.h"
#include "hw_config.h"
#include "shell.h"


int main(){
	create_ctx(16384,shell,NULL,"shell");
	start_sched();
	exit(EXIT_SUCCESS);
}


