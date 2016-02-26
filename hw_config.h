/* ------------------------------
   $Id: hw_config.h 86 2007-06-01 14:34:35Z skaczmarek $
   ------------------------------------------------------------

   Fichier de configuration des acces au materiel

   Philippe Marquet, march 2007

   Code au niveau applicatif la description du materiel qui est fournie
   par hardware.ini
   
*/

#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_

#define HARDWARE_INI	"hardware.ini"

/* Horloge */
#define TIMER_CLOCK	0xF0
#define TIMER_PARAM     0xF4
#define TIMER_ALARM     0xF8
#define TIMER_IRQ	2   
/* Coeur */
#define CORE_ID 	0x126
#define CORE_IRQMAPPER	0x82
#define CORE_LOCK	0x98
#define CORE_UNLOCK	0x99
#define CORE_NCORE	8

#endif
