###------------------------------
### Main rules 
###------------------------------------------------------------
main :main.o gest_contexte.o shell.o
	gcc -Wall -ansi -m32 -pedantic -g -o main main.o gest_contexte.o shell.o -L./lib -lhardware
main.o :main.c gest_contexte.h shell.h
	gcc -Wall -ansi -m32 -pedantic -g -c main.c 
gest_contexte.o : gest_contexte.c gest_contexte.h
	gcc -Wall -std=gnu99 -m32 -g -c gest_contexte.c
shell.o : shell.c shell.h
	gcc -Wall -std=gnu99 -m32 -g -c shell.c
###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o 
realclean: clean 
	$(RM) main
