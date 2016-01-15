# $Id: Makefile 78 2007-05-21 17:33:37Z skaczmarek $
##############################################################################


###------------------------------
### Main rules 
###------------------------------------------------------------
main :main.o gest_contexte.o
	gcc -Wall -ansi -m32 -pedantic -g -o main main.o gest_contexte.o -L./lib -lhardware
main.o :main.c gest_contexte.h  
	gcc -Wall -ansi -m32 -pedantic -g -c main.c 
gest_contexte.o :gest_contexte.c
	gcc -Wall -std=gnu99 -m32 -g -c gest_contexte.c
###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o 
realclean: clean 
	$(RM) main
