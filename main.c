#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include "hw_config.h"
#include "hardware.h"
#include "gest_contexte.h"
#include "shell.h"

/*
void mount(){
	printf("Mounts\n");
	init_hardware("hardware.ini");
	printf("initDisk\n");
	init_disk();
	printf("readMbr\n");
	read_mbr();
	printf("CurrentVol\n");
	load_current_volume();	
        }
*//*
     int main(){
     create_ctx(16384,shell,NULL,"shell");
     load_first_context();
     mount();
     printf("mount fin\n");
     printf("ordonnanceur\n");
     start_sched();
     exit(EXIT_SUCCESS);
     }
   */

int
main ()
{
  create_ctx (16384, shell, NULL, "shell");
  init_hardware ("hardware.ini");
  /*init_disk(); */
  printf ("ordonnanceur\n");
  start_sched ();
  exit (EXIT_SUCCESS);
}
