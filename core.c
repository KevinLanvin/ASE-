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

void steal ();

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
infinityAndODELA (void *args)
{
  while (1);
}

void
activated ()
{


  if (_in (CORE_ID) == 1)
    {
      printf ("Core n°%d activaient\n", _in (CORE_ID));
      _out (CORE_IRQMAPPER + _in (CORE_ID), 0x1 << TIMER_IRQ);
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      create_ctx (16384, infinityAndODELA, NULL, "infinityAndODELA",
		  _in (CORE_ID));
      start_sched ();
    }
  else
    {
      printf ("Core n°%d activaient\n", _in (CORE_ID));
      _out (CORE_IRQMAPPER + _in (CORE_ID), 0x1 << TIMER_IRQ);
      create_ctx (16384, f_ping, NULL, "ping", _in (CORE_ID));
      create_ctx (16384, f_pong, NULL, "pong", _in (CORE_ID));
      start_sched ();
    }
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
steal();
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


void
steal ()
{
   _out (CORE_IRQMAPPER + _in (CORE_ID), 0);
  printf ("%d travail terminaieee\n", _in (CORE_ID));
  printf ("core surchargé : %d\n", max_ring ());
  while (1);
}



int
main ()
{
  init_core_et_un ();
  while (1);
  return 0;
}
