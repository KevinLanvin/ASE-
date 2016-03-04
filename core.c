#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ordonnanceur.h"
#include "hardware.h"
#include "core.h"
#include "hw_config.h"


void f_ping (void *arg);
void f_pong (void *arg);



void
f_ping (void *args)
{
  int i = 0;
  while (i++ < 20)
    printf ("[%d] ping\n", _in (CORE_ID));
  return;
}

void
f_pong (void *args)
{
  int i = 0;
  while (i++ < 20)
    printf ("[%d] pong\n", _in (CORE_ID));

  return;
}


void
activated ()
{
  int id = _in (CORE_ID);
  printf ("Core n°%d activaient\n", id);
  _out (CORE_IRQMAPPER + _in (CORE_ID), 0x1 << TIMER_IRQ);
  create_ctx (16384, f_ping, NULL, "ping", id);
  create_ctx (16384, f_pong, NULL, "pong", id);
  start_sched ();
  while (1);
/*
	while(1){
		if(_in(CORE_LOCK)==1){

			printf("[%d ",_in(CORE_ID));
			int i;
			for(i=0;i<1048576;++i);
			printf("- %d]\n",_in(CORE_ID));

_in(CORE_LOCK);
_out(CORE_UNLOCK,0xFF);
		}
	}
*/
}

void
empty_it ()
{
}

void
init_core_et_un ()
{
  int i;
  init_hardware ("core.ini");
  for (i = 0; i < 16; i++)
    IRQVECTOR[i] = empty_it;
  IRQVECTOR[0] = activated;

  _out (CORE_STATUS, 0xFE);

}



int
main ()
{
  init_core_et_un ();
  while (1);
  return 0;
}
